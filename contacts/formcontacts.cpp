#include "formcontacts.h"
#include "ui_formcontacts.h"
#include "seconfig.h"
#include "apiadapter.h"
#include "formcontactcard.h"
#include "formcompanycard.h"
#include "dialogexport.h"
#include "dialogimport.h"
#include "userpermission.h"
#include "dialogpricetype.h"

#include <QApplication>
#include <QClipboard>
#include <QMenu>
#include <QMessageBox>
#include <QMouseEvent>
#include <QDebug>
#include <QDir>
#include <QDesktopServices>

FormContacts::FormContacts(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FormContacts)
{
    ui->setupUi(this);

    isSelectMode = false;
    selectedTypeContacts = 0;
    selectMode = All;
    ui->tabWidget->setCurrentIndex(0);
    initVariables();
    initSignals();
    initActions();
    initPermissions();
}

FormContacts::FormContacts(QWidget *parent, const bool &isMultiSelect, SelectMode mode) :
    QDialog(parent),
    ui(new Ui::FormContacts)
{
    ui->setupUi(this);

    setWindowFlags(Qt::Window);
    selectedTypeContacts = 0;
    selectMode = mode;
    ui->tabWidget->setCurrentIndex(0);
    isSelectMode = true;
    initVariables();
    initSignals();
    initActions();
    if (isMultiSelect)
        ui->treeViewContacts->setSelectionMode(QAbstractItemView::ExtendedSelection);
}

const ListContacts *FormContacts::getSelectedContacts()
{
    if (selectedContacts)
        return selectedContacts;

    selectedContacts = new ListContacts();
    QModelIndexList list;
    if (ui->tabWidget->currentIndex()) {
        selectedTypeContacts = 1;
        list = ui->treeViewCompanies->selectionModel()->selectedRows();
        for (int i = 0; i < list.size(); i++)
            selectedContacts->append(companies->at(list.at(i).row()));
    } else {
        selectedTypeContacts = 0;
        list = ui->treeViewContacts->selectionModel()->selectedRows();
        for (int i = 0; i < list.size(); i++)
            selectedContacts->append(contacts->at(list.at(i).row()));
    }
    return selectedContacts;
}

FormContacts::SelectMode FormContacts::getSelectMode() const
{
    return selectMode;
}

int FormContacts::getSelectedTypeContacts() const
{
    return selectedTypeContacts;
}

FormContacts::~FormContacts()
{
    if (isShown) {
        SEConfig::saveSplitterLayout(ui->splitter);
        ui->tabWidget->currentIndex() ?
                    SEConfig::saveTableState(ui->treeViewCompanies->header(), ui->treeViewCompanies->objectName()) :
                    SEConfig::saveTableState(ui->treeViewContacts->header(), ui->treeViewContacts->objectName());
    }        

    delete ui;        
    contacts->deleteItems();
    if (selectedContacts)
        delete selectedContacts;    
    apiContacts->deleteLater();
}

void FormContacts::onAddEditContact()
{
    int isNew = false;
    isNew = sender() == ui->actionAdd;
    QString id;
    if (ui->tabWidget->currentIndex()) {
        FormCompanyCard companyCard(this);
        if (!isNew && ui->treeViewCompanies->currentIndex().isValid()) {
            id = modelCompanies->idData(ui->treeViewCompanies->currentIndex());
            companyCard.setIdCompany(id);
        }
        connect(&companyCard, SIGNAL(saved(const DataCompany*,bool)),
                SLOT(onSaveCompany(const DataCompany*,bool)));
        companyCard.exec();

    } else {
        FormContactCard contactCard(this);
        if (!isNew && ui->treeViewContacts->currentIndex().isValid()) {
            id = modelContacts->idData(ui->treeViewContacts->currentIndex());
            contactCard.setIdContact(id);
        }
        connect(&contactCard, SIGNAL(saved(const DataContact*,bool)),
                SLOT(onSaveContact(const DataContact*,bool)));
        contactCard.exec();
    }
}

void FormContacts::onDeleteContacts()
{
    if (!UserPermission::isDelete(UserPermission::CONTACTS))
        return;

    QString msg;

    if (ui->tabWidget->currentIndex()) {
        if (ui->treeViewCompanies->selectionModel()->selectedRows().count() > 1)
            msg = tr("Удалить выбранные компании?");
        else msg = tr("Удалить компанию") + ": " +
                modelCompanies->dataItem(ui->treeViewCompanies->currentIndex())->getName() + "?";

        QMessageBox::StandardButton mesRepl;
        mesRepl = QMessageBox::question(this, tr("Подтверждение удаления"),
                                        msg, QMessageBox::Yes | QMessageBox::No);
        if(mesRepl == QMessageBox::Yes)        {
            Request request;
            QModelIndexList list = ui->treeViewCompanies->selectionModel()->selectedRows();
            for (int i = 0; i < list.count(); i++)
                    request.addId(companies->at(list.at(i).row())->getId());
            if (list.count()) {
                ApiAdapter::getApi()->deleteCompanies(request);
                refreshDataCompanies();
            }
        }
    } else {
        if (ui->treeViewContacts->selectionModel()->selectedRows().count() > 1)
            msg = tr("Удалить выбранные контакты?");
        else msg = tr("Удалить контакт") + ": " +
                modelContacts->dataItem(ui->treeViewContacts->currentIndex())->display() + "?";

        QMessageBox::StandardButton mesRepl;
        mesRepl = QMessageBox::question(this, tr("Подтверждение удаления"),
                                        msg, QMessageBox::Yes | QMessageBox::No);
        if(mesRepl == QMessageBox::Yes)        {
            Request request;
            QModelIndexList list = ui->treeViewContacts->selectionModel()->selectedRows();
            for (int i = 0; i < list.count(); i++)
                    request.addId(contacts->at(list.at(i).row())->getId());
            if (list.count()) {
                ApiAdapter::getApi()->deleteContacts(request);
                refreshDataContacts();
            }
        }
    }
}

void FormContacts::onClickHeaderItem(int )
{
    refreshDataContacts();
}

void FormContacts::refreshData()
{
    tabChanged(ui->tabWidget->currentIndex());
}

void FormContacts::refreshDataContacts()
{    
    if (!UserPermission::isRead(UserPermission::CONTACTS))
        return;

    resetRequestContacts();

    SEConfig::saveStateModel(ui->treeViewContacts);
    preloaderContacts->startAnimation();
    apiContacts->requestInThread(SEConsts::API_CONTACTS, SEConsts::APIMETHOD_FETCH, requestContacts);
}

void FormContacts::refreshDataCompanies()
{
    if (!UserPermission::isRead(UserPermission::CONTACTS))
        return;

    resetRequestCompanies();

    SEConfig::saveStateModel(ui->treeViewCompanies);
    preloaderCompanies->startAnimation();
    apiCompanies->requestInThread(SEConsts::API_COMPANIES, SEConsts::APIMETHOD_FETCH, requestCompanies);
}

void FormContacts::onRunSearchContacts()
{
    if (timerSearchContacts->isActive())
        timerSearchContacts->stop();
    numPageContacts = 0;
    timerSearchContacts->start(300);
}

void FormContacts::onRunSearchCompanies()
{
    if (timerSearchCompanies->isActive())
        timerSearchCompanies->stop();
    numPageCompanies = 0;
    timerSearchCompanies->start(300);
}

void FormContacts::onChangeContact()
{
    if (ui->treeViewContacts->currentIndex().isValid())
        ui->widgetNavigator->setNumRecord(numPageContacts * ui->widgetNavigator->getLimit() +
                                          ui->treeViewContacts->currentIndex().row() + 1,
                                          countContacts);
    else ui->widgetNavigator->setNumRecord(0, 0);

    ui->actionAdd->setEnabled(UserPermission::isAdd(UserPermission::CONTACTS));
    ui->actionEdit->setEnabled(ui->treeViewContacts->currentIndex().isValid() &&
                               UserPermission::isEdit(UserPermission::CONTACTS));
    ui->actionDelete->setEnabled(ui->treeViewContacts->currentIndex().isValid() &&
                                 UserPermission::isDelete(UserPermission::CONTACTS));
    ui->actionSelect->setEnabled(ui->treeViewContacts->currentIndex().isValid() &&
                                 UserPermission::isRead(UserPermission::CONTACTS));
    ui->actionSetGroups->setEnabled(UserPermission::isEdit(UserPermission::CONTACTS));
    ui->toolButtonContactInfo->setEnabled(ui->treeViewContacts->currentIndex().isValid());
}

void FormContacts::onChangeCompany()
{
    if (ui->treeViewCompanies->currentIndex().isValid())
        ui->widgetNavigatorCompany->setNumRecord(numPageCompanies * ui->widgetNavigatorCompany->getLimit() +
                                          ui->treeViewCompanies->currentIndex().row() + 1,
                                          countCompanies);
    else ui->widgetNavigatorCompany->setNumRecord(0, 0);

    ui->actionAdd->setEnabled(UserPermission::isAdd(UserPermission::CONTACTS));
    ui->actionEdit->setEnabled(ui->treeViewCompanies->currentIndex().isValid() &&
                               UserPermission::isEdit(UserPermission::CONTACTS));
    ui->actionDelete->setEnabled(ui->treeViewCompanies->currentIndex().isValid() &&
                                 UserPermission::isDelete(UserPermission::CONTACTS));
    ui->actionSelect->setEnabled(ui->treeViewCompanies->currentIndex().isValid() &&
                                 UserPermission::isRead(UserPermission::CONTACTS));
    ui->actionSetGroups->setEnabled(UserPermission::isEdit(UserPermission::CONTACTS));
}

void FormContacts::onCopyCellText()
{
    if (ui->tabWidget->currentIndex()) {
        if (!ui->treeViewCompanies->currentIndex().isValid())
            return;
        QApplication::clipboard()->setText(modelCompanies->data(ui->treeViewCompanies->currentIndex(), Qt::DisplayRole).toString());
    } else {
        if (!ui->treeViewContacts->currentIndex().isValid())
            return;
        QApplication::clipboard()->setText(modelContacts->data(ui->treeViewContacts->currentIndex(), Qt::DisplayRole).toString());
    }
}

void FormContacts::showContextMenu(QPoint point)
{
    onChangeContact();
    QPoint globalPos;

    if (sender()->inherits("QAbstractScrollArea"))
        globalPos = ((QAbstractScrollArea*)sender())->viewport()->mapToGlobal(point);
    else globalPos = ((QWidget*)sender())->mapToGlobal(point);

    QMenu menu;

    menu.addAction(ui->actionAdd);
    menu.addAction(ui->actionEdit);
    menu.addAction(ui->actionDelete);
    menu.addAction(ui->actionSetGroups);
    if (!ui->tabWidget->currentIndex())
        menu.addAction(ui->actionSetPriceType);
    menu.addSeparator();
    menu.addAction(ui->actionRefresh);
    menu.addSeparator();
    menu.addAction(ui->actionSelect);
    menu.addSeparator();
    menu.addAction(ui->actionCopy);

    menu.exec(globalPos);
}

void FormContacts::onSaveContact(const DataContact *contact, const bool isNew)
{
    if (!isNew && ui->treeViewContacts->currentIndex().isValid()) {
         DataContact *editContact = qobject_cast <DataContact * >
                 (modelContacts->dataItem(ui->treeViewContacts->currentIndex()));
         editContact->copy(contact);
         modelContacts->setData(modelContacts->index(0, ui->treeViewContacts->currentIndex().row()),
                                contact->getId(), Qt::EditRole);
    }
    if (isNew) {        
        DataContact *newContact = new DataContact(contact);        
        modelContacts->addItem(newContact, false);
        countContacts++;
        ui->treeViewContacts->selectionModel()->clearSelection();
        goToRowContactsById(0);
    }
}

void FormContacts::onSaveCompany(const DataCompany *company, const bool isNew)
{
    if (!isNew && ui->treeViewCompanies->currentIndex().isValid()) {
         DataContact *editCompany = qobject_cast <DataContact * >
                 (modelCompanies->dataItem(ui->treeViewCompanies->currentIndex()));
         if (editCompany) {
            editCompany->copy(company);
            modelCompanies->setData(modelCompanies->index(0, ui->treeViewCompanies->currentIndex().row()),
                                    company->getId(), Qt::EditRole);
         }
    }
    if (isNew) {
        DataCompany *newCompany = new DataCompany(company);
        modelCompanies->addItem(newCompany, false);
        countCompanies++;
        ui->treeViewCompanies->selectionModel()->clearSelection();
        goToRowCompaniesById(0);
    }
}

void FormContacts::goNextPage()
{
    if (sender() == ui->widgetNavigator) {
        numPageContacts++;
        if (numPageContacts > int(countContacts / ui->widgetNavigator->getLimit()))
            numPageContacts = int(countContacts / ui->widgetNavigator->getLimit());
        refreshDataContacts();
    }
    numPageCompanies++;
    if (numPageCompanies > int(countCompanies / ui->widgetNavigator->getLimit()))
        numPageCompanies = int(countCompanies / ui->widgetNavigator->getLimit());
    refreshDataCompanies();
}

void FormContacts::goPreviosPage()
{
    if (sender() == ui->widgetNavigator) {
        numPageContacts--;
        if (numPageContacts < 0)
            numPageContacts = 0;
        refreshDataContacts();
    }
    numPageCompanies--;
    if (numPageCompanies < 0)
        numPageCompanies = 0;
    refreshDataCompanies();
}

void FormContacts::goLastPage()
{
     if (sender() == ui->widgetNavigator) {
        numPageContacts = int(countContacts / ui->widgetNavigator->getLimit());
        refreshDataContacts();
     }
     numPageCompanies = int(countCompanies / ui->widgetNavigator->getLimit());
     refreshDataCompanies();
}

void FormContacts::goFirstPage()
{
    if ((sender() == ui->widgetNavigator) || (sender() == ui->comboBoxManagers)) {
        numPageContacts = 0;
        refreshDataContacts();
    }
    numPageCompanies = 0;
    refreshDataCompanies();
}

void FormContacts::printContactInfo()
{
    if (!ui->treeViewContacts->currentIndex().isValid())
        return;

    setCursor(Qt::WaitCursor);
    DataContact *contact = qobject_cast <DataContact *> (modelContacts->dataItem(ui->treeViewContacts->currentIndex()));
    Request request;
    request.addId(contact->getId());
    QByteArray *bytes = new QByteArray;

    if (ApiAdapter::getApi()->exportItemData(SEConsts::API_CONTACTS, request, bytes)) {
        QString folderExport = SEConfig::getFolderExport();
        SEConfig::createDir(folderExport);
        QString fileName = folderExport + QDir::separator() + "contact_" + contact->getId() + ".xlsx";
        QFile fileExport(fileName);
        if (fileExport.exists())
            fileExport.remove();
        if (fileExport.open(QFile::WriteOnly))
            fileExport.write(*bytes);
        fileExport.close();
        QDesktopServices::openUrl(QUrl::fromLocalFile(fileName));
    }
    setCursor(Qt::ArrowCursor);
}

void FormContacts::tabChanged(int index)
{
    switch (index) {
    case 0:
        if (isLoadedLayoutsCompanies)
            SEConfig::saveTableState(ui->treeViewCompanies->header(), ui->treeViewCompanies->objectName());
        ui->actionAdd->setText("Добавить контакт");
        ui->actionAdd->setToolTip("Добавить новый контакт");
        refreshDataContacts();
        break;
    case 1:
        if (isLoadedLayoutsContacts)
            SEConfig::saveTableState(ui->treeViewContacts->header(), ui->treeViewContacts->objectName());
        ui->actionAdd->setText("Добавить компанию");
        ui->actionAdd->setToolTip("Добавить новую компанию");
        refreshDataCompanies();
        break;
    default:
        break;
    }
}

void FormContacts::finishedRefreshDataContacts(const bool status, const QString message)
{
    if (status && ApiAdapter::getApi()->getListContacts(requestContacts, contacts)) {
        countContacts = contacts->countInBase();
        modelContacts->resetModel();
        isShown = true;
        if (!isLoadedLayoutsContacts)
            isLoadedLayoutsContacts =
                    SEConfig::restoreTableState(ui->treeViewContacts->header(),
                                                ui->treeViewContacts->objectName());
        if (!isLoadedLayoutsContacts)
            for (int i = 0; i < modelContacts->columnCount() - 1; i++)
                ui->treeViewContacts->resizeColumnToContents(i);
        isLoadedLayoutsContacts = true;
    }
    preloaderContacts->stopAnimation();
    SEConfig::restoreStateModel(ui->treeViewContacts);
    if (!ui->treeViewContacts->currentIndex().isValid() && modelContacts->getItems()->size())
        ui->treeViewContacts->selectionModel()->setCurrentIndex(
                    modelContacts->index(0, 0), QItemSelectionModel::SelectCurrent | QItemSelectionModel::Rows);
    if (!status)
        QMessageBox::critical(0, tr("Ой, ошибка запроса!"), message);
}

void FormContacts::finishedRefreshDataCompanies(const bool status, const QString message)
{
    if (status && ApiAdapter::getApi()->getListData(SEConsts::API_COMPANIES, companies, requestCompanies)) {
        countCompanies = companies->countInBase();
        modelCompanies->resetModel();
        isShown = true;
        if (!isLoadedLayoutsCompanies)
            isLoadedLayoutsCompanies =
                    SEConfig::restoreTableState(ui->treeViewCompanies->header(),
                                                ui->treeViewCompanies->objectName());
        if (!isLoadedLayoutsCompanies)
            for (int i = 0; i < modelCompanies->columnCount() - 1; i++)
                ui->treeViewCompanies->resizeColumnToContents(i);
        isLoadedLayoutsCompanies = true;
    }
    preloaderCompanies->stopAnimation();
    SEConfig::restoreStateModel(ui->treeViewCompanies);
    if (!ui->treeViewCompanies->currentIndex().isValid() && modelCompanies->getItems()->size())
        ui->treeViewCompanies->selectionModel()->setCurrentIndex(
                    modelCompanies->index(0, 0), QItemSelectionModel::SelectCurrent | QItemSelectionModel::Rows);
    if (!status)
        QMessageBox::critical(0, tr("Ой, ошибка запроса!"), message);
}

void FormContacts::reject()
{
    if (isSelectMode)
        QDialog::reject();
}

void FormContacts::showEvent(QShowEvent *)
{
     if (!isShown)
        SEConfig::loadSplitterLayout(ui->splitter);          
}

void FormContacts::resetRequestContacts()
{
    requestContacts.clear();
    requestContacts.setOffset(numPageContacts * ui->widgetNavigator->getLimit());
    requestContacts.setLimit(ui->widgetNavigator->getLimit());

    requestContacts.clearFilter();
    QString filter = QString();
    if (ui->widgetGroups->idCheckedGroups().size()) {
        QString idsGroups;
        for (int i = 0; i < ui->widgetGroups->idCheckedGroups().size(); i++) {
            if (!idsGroups.isEmpty())
                idsGroups += ",";
            idsGroups += "'" + ui->widgetGroups->idCheckedGroups().at(i) + "'";
        }
        filter = "idGroup in (" +
                idsGroups + ")";        
    }

    // менеджеры
    if (ui->comboBoxManagers->currentIndex()) {
        QString code = ui->comboBoxManagers->currentData().toString();
        if (!filter.isEmpty())
            filter += " and ";
        filter += "idManager = '" + code + "'";
    }

    requestContacts.setFilter(filter);

    QString sortField = fieldsContacts->at(ui->treeViewContacts->header()->sortIndicatorSection())->name();
    requestContacts.setSortBy(sortField);
    requestContacts.setSearchText(ui->widgetSearch->searchText().trimmed());
    requestContacts.setAscendingOrder(ui->treeViewContacts->header()->sortIndicatorOrder() ==
                                      Qt::AscendingOrder);
}

void FormContacts::resetRequestCompanies()
{
    requestCompanies.clear();
    requestCompanies.setOffset(numPageCompanies * ui->widgetNavigatorCompany->getLimit());
    requestCompanies.setLimit(ui->widgetNavigatorCompany->getLimit());

    requestCompanies.clearFilter();
    if (ui->widgetGroups->idCheckedGroups().size()) {
        QString idsGroups;
        for (int i = 0; i < ui->widgetGroups->idCheckedGroups().size(); i++) {
            if (!idsGroups.isEmpty())
                idsGroups += ",";
            idsGroups += "'" + ui->widgetGroups->idCheckedGroups().at(i) + "'";
        }
        QString filter = "idGroup in (" +
                idsGroups + ")";
        requestCompanies.setFilter(filter);
    }

    QString sortField = fieldsCompanies->at(ui->treeViewCompanies->header()->sortIndicatorSection())->name();
    requestCompanies.setSortBy(sortField);
    requestCompanies.setSearchText(ui->widgetSearchCompany->searchText().trimmed());
    requestCompanies.setAscendingOrder(ui->treeViewCompanies->header()->sortIndicatorOrder() ==
                                      Qt::AscendingOrder);
}

void FormContacts::initActions()
{
    ui->toolButtonAdd->setDefaultAction(ui->actionAdd);
    ui->toolButtonAddCompany->setDefaultAction(ui->actionAdd);
    ui->toolButtonEdit->setDefaultAction(ui->actionEdit);
    ui->toolButtonEditCompany->setDefaultAction(ui->actionEdit);
    ui->toolButtonDelete->setDefaultAction(ui->actionDelete);
    ui->toolButtonDeleteCompany->setDefaultAction(ui->actionDelete);
    ui->toolButtonRefresh->setDefaultAction(ui->actionRefresh);
    ui->toolButtonRefreshCompanies->setDefaultAction(ui->actionRefresh);
    ui->toolButtonSelect->setDefaultAction(ui->actionSelect);
    ui->toolButtonSelectCompany->setDefaultAction(ui->actionSelect);
    ui->toolButtonSetGroups->setDefaultAction(ui->actionSetGroups);
    ui->toolButtonSetGroupsCompany->setDefaultAction(ui->actionSetGroups);
}

void FormContacts::initTreeContacts()
{
    ui->treeViewContacts->setModel(modelContacts);
    ui->treeViewContacts->header()->setSortIndicator(0, Qt::DescendingOrder);
    ui->treeViewContacts->header()->setSectionsMovable(true);
    ui->treeViewContacts->setAlternatingRowColors(true);
    ui->treeViewContacts->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->treeViewContacts, SIGNAL(customContextMenuRequested(QPoint)),
            SLOT(showContextMenu(QPoint)));

    ui->treeViewCompanies->setModel(modelCompanies);
    ui->treeViewCompanies->header()->setSortIndicator(0, Qt::DescendingOrder);
    ui->treeViewCompanies->header()->setSectionsMovable(true);
    ui->treeViewCompanies->setAlternatingRowColors(true);
    ui->treeViewCompanies->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->treeViewCompanies, SIGNAL(customContextMenuRequested(QPoint)),
            SLOT(showContextMenu(QPoint)));
}

void FormContacts::initFields()
{
    fieldsContacts = new QVector<Field *>();
    fieldsContacts->append(new Field("id", "Ид.", Qt::AlignRight | Qt::AlignVCenter));
    fieldsContacts->append(new Field("regDate", "Дата рег."));
    fieldsContacts->append(new Field("login", "Логин"));
    fieldsContacts->append(new Field("display", "Фамилия Имя Отчество"));
    fieldsContacts->append(new Field("email", "E-mail"));
    fieldsContacts->append(new Field("phone", "Телефоны"));
    fieldsContacts->append(new Field("countOrders", "Заказы", Qt::AlignRight | Qt::AlignVCenter));
    fieldsContacts->append(new Field("priceTypeDisplay", "Тип цены"));
    fieldsContacts->append(new Field("note", "Примечание"));

    fieldsCompanies = new QVector<Field *>();
    fieldsCompanies->append(new Field("id", "Ид.", Qt::AlignRight | Qt::AlignVCenter));
    fieldsCompanies->append(new Field("regDate", "Дата рег."));
    fieldsCompanies->append(new Field("name", "Название компании"));
    fieldsCompanies->append(new Field("inn", "ИНН"));
    fieldsCompanies->append(new Field("kpp", "КПП"));
    fieldsCompanies->append(new Field("email", "E-mail"));
    fieldsCompanies->append(new Field("phone", "Телефоны"));
    fieldsCompanies->append(new Field("contact", "Ф.И.О. контакта"));
    fieldsCompanies->append(new Field("countOrders", "Заказы", Qt::AlignRight | Qt::AlignVCenter));
    fieldsCompanies->append(new Field("note", "Примечание"));
}

void FormContacts::initVariables()
{
    if (!isSelectMode) {
        ui->actionSelect->setVisible(false);        
        ui->toolButtonSelect->hide();
        ui->toolButtonSelectCompany->hide();
    } else {
        ui->actionDelete->setVisible(false);
        ui->toolButtonDelete->hide();
        ui->toolButtonDeleteCompany->hide();
        switch (selectMode) {
        case Contacts:
            ui->tabWidget->setTabEnabled(1, false);
            break;
        case Companies:
            ui->tabWidget->setCurrentIndex(1);
            ui->tabWidget->setTabEnabled(0, false);
            break;
        default:
            break;
        }
    }

    apiContacts = new Api;
    apiCompanies = new Api;
    isShown = false;
    isLoadedLayoutsContacts = false;
    isLoadedLayoutsCompanies = false;
    countContacts = 0;
    countCompanies = 0;
    numPageContacts = 0;
    numPageCompanies = 0;
    selectedContacts = 0;
    formSetColumnsContacts = 0;
    formSetColumnsCompanies = 0;
    timerSearchContacts = new QTimer(this);
    timerSearchContacts->setSingleShot(true);
    timerSearchCompanies = new QTimer(this);
    timerSearchCompanies->setSingleShot(true);
    contacts = new ListContacts();
    modelContacts = new NameTableModel(this, reinterpret_cast<ListDataItems *> (contacts), isSelectMode);    
    modelContacts->clearFields();
    initFields();
    foreach (Field *f, *fieldsContacts)
        modelContacts->appendField(f);
    companies = new ListContacts();
    modelCompanies = new NameTableModel(this, reinterpret_cast<ListDataItems *> (companies), isSelectMode);
    modelCompanies->clearFields();
    foreach (Field *f, *fieldsCompanies)
        modelCompanies->appendField(f);
    preloaderContacts = new Preloader(ui->treeViewContacts);
    preloaderCompanies = new Preloader(ui->treeViewCompanies);
    ui->widgetSearch->setIsHotSearchMode();
    ui->widgetSearchCompany->setIsHotSearchMode();

    ui->comboBoxManagers->addItem(tr("Все"));
    ListContacts listManagers;
    if (ApiAdapter::getApi()->getListUsers(Request(), &listManagers)) {
        for (int i = 0; i < listManagers.size(); ++i) {
            ui->comboBoxManagers->addItem(listManagers.at(i)->displayName(),
                                         listManagers.at(i)->getId());
        }
    }
    ui->comboBoxManagers->setCurrentIndex(0);


    initTreeContacts();

}

void FormContacts::initSignals()
{
    ui->actionEdit->setEnabled(false);
    ui->actionDelete->setEnabled(false);
    ui->actionSelect->setEnabled(false);
    connect(ui->actionAdd, SIGNAL(triggered()), SLOT(onAddEditContact()));
    connect(ui->actionEdit, SIGNAL(triggered()), SLOT(onAddEditContact()));
    connect(ui->actionDelete, SIGNAL(triggered()), SLOT(onDeleteContacts()));
    connect(ui->actionRefresh, SIGNAL(triggered()), SLOT(refreshData()));
    connect(ui->actionSelect, SIGNAL(triggered()), SLOT(accept()));
    connect(ui->toolButtonSetColumns, &QToolButton::clicked, this, &FormContacts::setVisibleColumns);
    connect(ui->toolButtonSetColumnsCompany, &QToolButton::clicked, this, &FormContacts::setVisibleColumns);
    connect(ui->actionSetGroups, &QAction::triggered, this, &FormContacts::onSetGroups);    
    connect(ui->actionSetPriceType, &QAction::triggered, this, &FormContacts::setPriceType);
    connect(ui->actionCopy, &QAction::triggered, this, &FormContacts::onCopyCellText);

    connect(ui->widgetNavigator, SIGNAL(goNextPage()), this, SLOT(goNextPage()));
    connect(ui->widgetNavigator, SIGNAL(goPreviosPage()), this, SLOT(goPreviosPage()));
    connect(ui->widgetNavigator, SIGNAL(goLastPage()), this, SLOT(goLastPage()));
    connect(ui->widgetNavigator, SIGNAL(goFirstPage()), this, SLOT(goFirstPage()));
    connect(ui->widgetNavigator, SIGNAL(refreshDataStart()), this, SLOT(goFirstPage()));

    connect(ui->comboBoxManagers, SIGNAL(currentIndexChanged(int)), this, SLOT(goFirstPage()));

    connect(ui->widgetNavigatorCompany, SIGNAL(goNextPage()), this, SLOT(goNextPage()));
    connect(ui->widgetNavigatorCompany, SIGNAL(goPreviosPage()), this, SLOT(goPreviosPage()));
    connect(ui->widgetNavigatorCompany, SIGNAL(goLastPage()), this, SLOT(goLastPage()));
    connect(ui->widgetNavigatorCompany, SIGNAL(goFirstPage()), this, SLOT(goFirstPage()));
    connect(ui->widgetNavigatorCompany, SIGNAL(refreshDataStart()), this, SLOT(goFirstPage()));

    connect(ui->widgetGroups, SIGNAL(checkedGroups()), SLOT(refreshData()));
    connect(ui->treeViewContacts->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
            SLOT(onChangeContact()));
    connect(ui->treeViewCompanies->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
            SLOT(onChangeCompany()));

    if (isSelectMode) {
        connect(ui->treeViewContacts, SIGNAL(doubleClicked(QModelIndex)), ui->actionSelect, SLOT(trigger()));
        connect(ui->treeViewCompanies, SIGNAL(doubleClicked(QModelIndex)), ui->actionSelect, SLOT(trigger()));
    }
    else {
        connect(ui->treeViewContacts, SIGNAL(doubleClicked(QModelIndex)), SLOT(onAddEditContact()));
        connect(ui->treeViewCompanies, SIGNAL(doubleClicked(QModelIndex)), SLOT(onAddEditContact()));
    }
    connect(ui->treeViewContacts->header(), SIGNAL(sectionClicked(int)), SLOT(onClickHeaderItem(int)));
    connect(ui->widgetSearch, SIGNAL(runSearch()), SLOT(onRunSearchContacts()));
    connect(ui->widgetSearchCompany, SIGNAL(runSearch()), SLOT(onRunSearchCompanies()));
    connect(ui->toolButtonExport, &QAbstractButton::clicked, this, &FormContacts::exportData);
    connect(ui->toolButtonImport, &QAbstractButton::clicked, this, &FormContacts::importData);
    connect(apiContacts, SIGNAL(finishedRequest(bool,QString)), SLOT(finishedRefreshDataContacts(bool,QString)));
    connect(apiCompanies, SIGNAL(finishedRequest(bool,QString)), SLOT(finishedRefreshDataCompanies(bool,QString)));
    connect(ui->toolButtonContactInfo, SIGNAL(clicked(bool)), SLOT(printContactInfo()));

    connect(timerSearchContacts, SIGNAL(timeout()), SLOT(refreshDataContacts()));
    connect(timerSearchCompanies, SIGNAL(timeout()), SLOT(refreshDataCompanies()));

    connect(ui->tabWidget, SIGNAL(currentChanged(int)), SLOT(tabChanged(int)));
}

void FormContacts::initPermissions()
{
    ui->toolButtonExport->setEnabled(UserPermission::isRead(UserPermission::CONTACTS));
    ui->toolButtonImport->setEnabled(UserPermission::isAdd(UserPermission::CONTACTS) &&
                                     UserPermission::isEdit(UserPermission::CONTACTS));
    ui->actionAdd->setEnabled(UserPermission::isAdd(UserPermission::CONTACTS));
    ui->actionEdit->setEnabled(UserPermission::isEdit(UserPermission::CONTACTS));
    ui->actionDelete->setEnabled(UserPermission::isDelete(UserPermission::CONTACTS));
    ui->actionSetGroups->setEnabled(UserPermission::isEdit(UserPermission::CONTACTS));
    ui->actionSetPriceType->setEnabled(UserPermission::isEdit(UserPermission::CONTACTS));
}

void FormContacts::goToRowContactsById(const QString &id)
{
    if (!id.isEmpty()) {
            for (int i = 0; i < contacts->size(); i ++)
                if (contacts->at(i)->getId() == id) {
                    ui->treeViewContacts->selectionModel()->setCurrentIndex(
                                modelContacts->index(i, 0), QItemSelectionModel::SelectCurrent |
                                QItemSelectionModel::Rows);
                    ui->treeViewContacts->setFocus();
                    return;
                }
    }
    if (contacts->size())
        ui->treeViewContacts->selectionModel()->setCurrentIndex(
                    modelContacts->index(0, 0), QItemSelectionModel::SelectCurrent |
                    QItemSelectionModel::Rows);
    else onChangeContact();
    ui->treeViewContacts->setFocus();
}

void FormContacts::goToRowCompaniesById(const QString &id)
{
    if (!id.isEmpty()) {
            for (int i = 0; i < companies->size(); i ++)
                if (companies->at(i)->getId() == id) {
                    ui->treeViewCompanies->selectionModel()->setCurrentIndex(
                                modelCompanies->index(i, 0), QItemSelectionModel::SelectCurrent |
                                QItemSelectionModel::Rows);
                    ui->treeViewCompanies->setFocus();
                    return;
                }
    }
    if (companies->size())
        ui->treeViewCompanies->selectionModel()->setCurrentIndex(
                    modelCompanies->index(0, 0), QItemSelectionModel::SelectCurrent |
                    QItemSelectionModel::Rows);
    else onChangeCompany();
    ui->treeViewCompanies->setFocus();
}

void FormContacts::setVisibleColumns()
{
    if (ui->tabWidget->currentIndex()) {
        if (!formSetColumnsCompanies)
            formSetColumnsCompanies = new FormSetVisibleHeaderColumns(this, ui->treeViewCompanies->header());
        formSetColumnsCompanies->exec();

    } else {
        if (!formSetColumnsContacts)
            formSetColumnsContacts = new FormSetVisibleHeaderColumns(this, ui->treeViewContacts->header());
        formSetColumnsContacts->exec();
    }
}

void FormContacts::onSetGroups()
{
    if (!UserPermission::isEdit(UserPermission::CONTACTS))
        return;

    QModelIndexList list = ui->tabWidget->currentIndex() ? ui->treeViewCompanies->selectionModel()->selectedRows() :
                                                           ui->treeViewContacts->selectionModel()->selectedRows();
    ListContacts *contacts = new ListContacts();

    for (int i = 0; i < list.size(); i++) {
        DataContact *contact = ui->tabWidget->currentIndex() ? new DataContact(this->companies->at(list.at(i).row())) :
                                                               new DataContact(this->contacts->at(list.at(i).row()));
        contacts->append(contact);
    }

    FormSetGroups *formSetGroups =  new FormSetGroups(this, contacts, ui->tabWidget->currentIndex());
    if (formSetGroups->exec() ==  QDialog::Accepted)
        ui->tabWidget->currentIndex() ? refreshDataCompanies() : refreshDataContacts();
    delete formSetGroups;
    delete contacts;
}

void FormContacts::setPriceType()
{
    if (!UserPermission::isEdit(UserPermission::CONTACTS))
        return;

    QModelIndexList list = ui->treeViewContacts->selectionModel()->selectedRows();
    if (!list.size())
        return;

    DataContact *contact = new DataContact();
    for (int i = 0; i < list.size(); i++)
        contact->addId(contacts->at(list.at(i).row())->getId());
    contact->setPriceType(contacts->at(list.at(0).row())->priceType());

    DialogPriceType *dialogPriceType =  new DialogPriceType(this, contact);
    if (dialogPriceType->exec() == QDialog::Accepted)
        refreshDataContacts();
    delete dialogPriceType;
    delete contact;
}

void FormContacts::exportData()
{
    if (!UserPermission::isRead(UserPermission::CONTACTS))
        return;

    DialogExport dlgExport(this);    
    dlgExport.setApiCatalogName(SEConsts::API_CONTACTS);
    dlgExport.setFilter(requestContacts);
    dlgExport.setFileName("contacts");
    dlgExport.exec();
}

void FormContacts::importData()
{
    if (!UserPermission::isAdd(UserPermission::CONTACTS) ||
            !UserPermission::isEdit(UserPermission::CONTACTS))
        return;

    DialogImport dlgImport(this);
    dlgImport.setApiCatalogName(SEConsts::API_CONTACTS);
    if (dlgImport.exec() == QDialog::Accepted) {
        ui->widgetGroups->refreshData();
    }
}
