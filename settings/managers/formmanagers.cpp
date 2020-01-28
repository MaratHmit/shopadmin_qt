#include "formmanagers.h"
#include "ui_formmanagers.h"

#include "apiadapter.h"
#include "seconfig.h"

#include <QMenu>

FormManagers::FormManagers(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormManagers)
{
    ui->setupUi(this);

    initVariables();
    initActions();
    initSignals();
}

void FormManagers::setModelRoles(const NameTableModel *modelRoles)
{
    this->modelRoles = modelRoles;
}

FormManagers::~FormManagers()
{
    if (isShown)
        SEConfig::saveTableState(ui->treeViewManagers->header(), ui->treeViewManagers->objectName());
    delete ui;
    delete apiFetch;
}

void FormManagers::filterManagers(QStringList ids)
{
    request.clear();
    QString filter = ids.join(",");
    if (!filter.isEmpty()) {
        filter = "idRole IN (" + filter +")";
        request.setFilter(filter);
    }
    refreshData();
}

void FormManagers::showEvent(QShowEvent *)
{
    if (!isShown)
        refreshData();
}

void FormManagers::checkEnabled()
{
    ui->actionAdd->setEnabled(true);
    ui->actionEdit->setEnabled(ui->treeViewManagers->currentIndex().isValid());
    ui->actionDel->setEnabled(ui->treeViewManagers->currentIndex().isValid());
    ui->actionSetRoles->setEnabled(ui->treeViewManagers->currentIndex().isValid());
}

void FormManagers::showContextMenu(QPoint point)
{
    QPoint globalPos;

    if (sender()->inherits("QAbstractScrollArea"))
        globalPos = ((QAbstractScrollArea*)sender())->viewport()->mapToGlobal(point);
    else globalPos = ((QWidget*)sender())->mapToGlobal(point);

    QMenu menu;

    menu.addAction(ui->actionAdd);
    menu.addAction(ui->actionSetRoles);
    menu.addSeparator();
    menu.addAction(ui->actionEdit);    
    menu.addAction(ui->actionDel);

    menu.exec(globalPos);
}

void FormManagers::finishedRefreshData(const bool status, const QString message)
{
    if (status && ApiAdapter::getApi()->getListUsers(request, managers)) {
        modelUsers->resetModel();
        isShown = true;
        if (!isLoadedLayouts)
            isLoadedLayouts =
                    SEConfig::restoreTableState(ui->treeViewManagers->header(),
                                                ui->treeViewManagers->objectName());
        if (!isLoadedLayouts)
            for (int i = 0; i < modelUsers->columnCount() - 1; i++)
                ui->treeViewManagers->resizeColumnToContents(i);
        isLoadedLayouts = true;
    }
    preloader->stopAnimation();
    SEConfig::restoreStateModel(ui->treeViewManagers);
    if (!ui->treeViewManagers->currentIndex().isValid() && modelUsers->getItems()->size())
        ui->treeViewManagers->selectionModel()->setCurrentIndex(
                    modelUsers->index(0, 0), QItemSelectionModel::SelectCurrent | QItemSelectionModel::Rows);
    if (!status)
        QMessageBox::critical(0, tr("Ой, ошибка запроса!"), message);

}

void FormManagers::addContact()
{
    ui->actionAdd->setEnabled(false);

    setCursor(Qt::WaitCursor);

    FormContacts formContacts(this, true);
    if (formContacts.exec() == QDialog::Accepted) {
        const ListContacts *list = formContacts.getSelectedContacts();
        DataContact contact;
        for (int i = 0; i < list->size(); ++i)
            contact.addId(list->at(i)->getId());
        contact.setModified("ids");
        ApiAdapter::getApi()->saveManager(&contact);
        QString idRole;
        for (int i = 0; i < modelRoles->getItems()->size(); ++i) {
            if (modelRoles->getItems()->at(i)->getIsChecked()) {
                idRole = modelRoles->getItems()->at(i)->getId();
                break;
            }
        }
        if (!idRole.isEmpty()) {
            QStringList idsRoles;
            idsRoles << idRole;
            contact.setProperty("idsRoles", idsRoles);
            ApiAdapter::getApi()->saveManager(&contact);
        }
        refreshData();
    }

    setCursor(Qt::ArrowCursor);
}

void FormManagers::editContact()
{
    if (!ui->treeViewManagers->currentIndex().isValid())
        return;

    FormContactCard contactCard(this);
    QString id = modelUsers->idData(ui->treeViewManagers->currentIndex());
    contactCard.setIdContact(id);
    if (contactCard.exec() == QDialog::Accepted)
        refreshData();
}

void FormManagers::delContact()
{
    QString msg;
    if (ui->treeViewManagers->selectionModel()->selectedRows().count() > 1)
        msg = tr("Удалить выбранных пользователей из списка?");
    else msg = tr("Удалить пользователя") + ": " +
            modelUsers->dataItem(ui->treeViewManagers->currentIndex())->display() + " из списка?";

    QMessageBox::StandardButton mesRepl;
    mesRepl = QMessageBox::question(this, tr("Подтверждение удаления"),
                                    msg, QMessageBox::Yes | QMessageBox::No);
    if(mesRepl != QMessageBox::Yes)
        return;

    Request request;

    QModelIndexList list = ui->treeViewManagers->selectionModel()->selectedRows();
    for (int i = 0; i < list.size(); ++i)
        request.addId(modelUsers->dataItem(list.at(i))->getId());

    if (ApiAdapter::getApi()->deleteManager(request))
        refreshData();
}

void FormManagers::setRoles()
{
    DialogSetRoles dialogRoles;
    QStringList idsRoles;
    for (int i = 0; i < modelRoles->getItems()->size(); ++i)
        idsRoles.append(modelRoles->getItems()->at(i)->getId());
    QModelIndexList list = ui->treeViewManagers->selectionModel()->selectedRows();
    for (int i = 0; i < list.size(); ++i) {
        QStringList idsUserRoles = modelUsers->dataItem(list.at(i))->
                property("idsRoles").toString().split(",");
        for (int j = idsRoles.size() - 1; j >= 0; --j) {
            if (!idsUserRoles.contains(idsRoles.at(j)))
                idsRoles.removeAt(j);
        }
    }
    dialogRoles.setModel(*modelRoles, idsRoles);

    if (dialogRoles.exec() == QDialog::Accepted) {
        DataContact user;
        for (int i = 0; i < list.size(); ++i)
            user.addId(modelUsers->dataItem(list.at(i))->getId());
        user.setProperty("idsRoles", dialogRoles.selectedIdsRoles());
        if (ApiAdapter::getApi()->saveManager(&user))
            refreshData();
    }
}

void FormManagers::onChangeCurrentItem()
{
    checkEnabled();
}

void FormManagers::initVariables()
{
    isShown = false;
    isLoadedLayouts = false;
    apiFetch = new Api;
    preloader = new Preloader(this);
    managers = new ListContacts();
    modelUsers = new NameTableModel(this, reinterpret_cast<ListDataItems *> (managers));
    modelUsers->clearFields();
    initFields();
    foreach (Field *f, *fields)
        modelUsers->appendField(f);
    ui->treeViewManagers->setModel(modelUsers);
    ui->toolButtonEdit->hide();
}

void FormManagers::initSignals()
{
    ui->treeViewManagers->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->treeViewManagers, SIGNAL(customContextMenuRequested(QPoint)), SLOT(showContextMenu(QPoint)));
    connect(ui->treeViewManagers->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
            SLOT(onChangeCurrentItem()));
    connect(ui->treeViewManagers, SIGNAL(doubleClicked(QModelIndex)), ui->actionEdit,
            SLOT(trigger()));
    connect(ui->toolButtonRefresh, &QAbstractButton::clicked, this, &FormManagers::refreshData);
    connect(ui->actionAdd, SIGNAL(triggered()), SLOT(addContact()));
    connect(ui->actionDel, SIGNAL(triggered()), SLOT(delContact()));
    connect(ui->actionEdit, SIGNAL(triggered()), SLOT(editContact()));
    connect(ui->actionSetRoles, SIGNAL(triggered(bool)), SLOT(setRoles()));
    connect(apiFetch, SIGNAL(finishedRequest(bool,QString)), SLOT(finishedRefreshData(bool,QString)));
}

void FormManagers::initActions()
{
    ui->toolButtonAdd->setDefaultAction(ui->actionAdd);
    ui->toolButtonEdit->setDefaultAction(ui->actionEdit);
    ui->toolButtonDelete->setDefaultAction(ui->actionDel);
    ui->toolButtonSetRoles->setDefaultAction(ui->actionSetRoles);
}

void FormManagers::initFields()
{
    fields = new QVector<Field *>();
    fields->append(new Field("id", "Ид.", Qt::AlignRight | Qt::AlignVCenter));
    fields->append(new Field("regDate", "Дата рег."));
    fields->append(new Field("login", "Логин"));
    fields->append(new Field("isActive", "Активен"));
    fields->append(new Field("display", "Фамилия Имя Отчество"));
    fields->append(new Field("roles", "Роли"));
}

void FormManagers::goToRowById(QString id)
{
    QModelIndex index = modelUsers->getModelIndexById(id);

    if (index.isValid()) {
        ui->treeViewManagers->selectionModel()->setCurrentIndex(
                    index, QItemSelectionModel::SelectCurrent |
                    QItemSelectionModel::Rows);
        ui->treeViewManagers->setFocus();
        return;
    } else {
        if (managers->size())
            ui->treeViewManagers->selectionModel()->setCurrentIndex(
                        modelUsers->index(0, 0), QItemSelectionModel::SelectCurrent |
                        QItemSelectionModel::Rows);
        else onChangeCurrentItem();
        ui->treeViewManagers->setFocus();
    }
}

void FormManagers::refreshData()
{    
    SEConfig::saveStateModel(ui->treeViewManagers);
    preloader->startAnimation();
    apiFetch->requestInThread(SEConsts::API_PERMISSION_USERS, SEConsts::APIMETHOD_FETCH, request);
}
