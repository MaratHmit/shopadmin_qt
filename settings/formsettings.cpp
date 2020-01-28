#include "formsettings.h"
#include "ui_formsettings.h"
#include "apiadapter.h"
#include "seconfig.h"
#include "dialogaddparampaysystem.h"
#include "dialogsetpayparamvalue.h"
#include "userpermission.h"
#include "dialogmigrationimages.h"

#include <QKeyEvent>
#include <QMessageBox>
#include <QDebug>
#include <QTableView>
#include <QHBoxLayout>
#include <QDir>
#include <QFileDialog>
#include <QSettings>
#include <QFile>

FormSettings::FormSettings(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormSettings)
{
    ui->setupUi(this);

    mIsShowed = false;
    mPaySystems = new ListDataItems();
    mPayReqs = new ListDataItems();
    ui->tabWidgetMainSettings->setCurrentIndex(0);
    ui->groupBoxYandex->hide();
    ui->pushButtonLicenses->hide();
    ui->pushButtonSiteLangs->hide();
    ui->toolButtonSaveIntegrations->hide();
    ui->lineSaveIntegration->hide();
    ui->labelInfo->hide();    
    ui->pushButtonClearCache->hide();
    ui->pushButtonMainSettings->hide();
    ui->tabWidgetMainSettings->widget(0)->hide();
    ui->pushButtonStores->setVisible(!SEConfig::isExtHosting);
    ui->groupBoxImages->setVisible(!SEConfig::isExtHosting);
    ui->pushButtonGeotargeting->setVisible(SEConfig::dbTables.contains("shop_contacts"));
    api = new Api;

    QSettings settings;
    mFolderDumps = SEConfig::getFolderDumps();
    mFolderDumps = settings.value("dumpsFolder", mFolderDumps).toString();

    if (SEConfig::isExtHosting) {
        ui->pushButtonMeasures->hide();
        ui->pushButtonDelImagesFromBD->hide();
    }

    initValidators();
    initPermissions();
}

FormSettings::~FormSettings()
{
    if (mIsShowedMainPaySystems) {
         SEConfig::saveSplitterLayout(ui->splitterPaysSystems);
         SEConfig::saveTableState(ui->treeViewPayParams->header(),
                                         ui->treeViewPayParams->objectName());
    }
    delete mPayReqs;
    delete mPaySystems;
    delete ui;
    if (mIntegrations)
        delete mIntegrations;    
    mSaveIntegrations.deleteItems();
}

void FormSettings::saveMainSettings()
{    
    setDataMain();    
    ApiAdapter::getApi()->saveMainSettings(mDataMain);
}

void FormSettings::saveIntegrations()
{
    setCursor(Qt::WaitCursor);    
    ui->tabIntegration->setEnabled(false);

    for (int i = 0; i < mIntegrations->size(); ++i) {
        DataIntegration *dataSave = new DataIntegration(mIntegrations->at(i));
        dataSave->listParameters()->setModified();
        ApiAdapter::getApi()->saveIntegrations(dataSave);
        mSaveIntegrations.append(dataSave);
    }

    ui->tabIntegration->setEnabled(true);
    setCursor(Qt::ArrowCursor);
}

void FormSettings::saveDataShopParameters()
{
    setCursor(Qt::WaitCursor);
    for (int i = 0; i < mListShopParametersTree.size(); ++i)
        mListShopParametersTree.at(i)->setEnabled(false);

    ListIntegrations *shopParameters = SEConfig::getShopParameters();
    for (int i = 0; i < shopParameters->size(); ++i) {
        if (shopParameters->at(i)->isModified()) {
            DataIntegration *dataSave = new DataIntegration(shopParameters->at(i));
            dataSave->listParameters()->setModified();
            ApiAdapter::getApi()->saveShopParameters(dataSave);
            mSaveIntegrations.append(dataSave);
        }
    }

    for (int i = 0; i < mListShopParametersTree.size(); ++i)
        mListShopParametersTree.at(i)->setEnabled(true);
    setCursor(Qt::ArrowCursor);
}

void FormSettings::on_pushButtonMainSettings_clicked()
{
    ui->stackedWidgetMainSettings->setCurrentIndex(0);    
}

void FormSettings::on_pushButtonDelivery_clicked()
{
    if (!mFormDeliveries) {
        mFormDeliveries = new FormDeliveries(this);
        ui->stackedWidgetMainSettings->addWidget(mFormDeliveries);
    }
    ui->stackedWidgetMainSettings->setCurrentWidget(mFormDeliveries);
}

void FormSettings::on_pushButtonPaySystems_clicked()
{
    if (!mFormPaySystems) {
        mFormPaySystems = new FormPaySystems(this);
        ui->stackedWidgetMainSettings->addWidget(mFormPaySystems);
    }
    ui->stackedWidgetMainSettings->setCurrentWidget(mFormPaySystems);
}

void FormSettings::showEvent(QShowEvent *)
{
    startTimer(1);
}

void FormSettings::timerEvent(QTimerEvent *e)
{
    killTimer(e->timerId());
    if (!mIsShowed) {
        mDataMain = SEConfig::getMainInfo();        
        if (UserPermission::isRead(UserPermission::SETTINGS)) {
            ui->pushButtonMainSettings->show();
            ui->tabWidgetMainSettings->widget(0)->show();
            ui->stackedWidgetMainSettings->setCurrentIndex(0);
            ui->tabWidgetMainSettings->setCurrentIndex(0);
            refreshData();
        }
        initVariables();
        initSignals();
        mIsShowed = true;
        if (ui->pushButtonMainSettings->isVisible())
            return;
        if (ui->pushButtonDelivery->isVisible()) {
            ui->pushButtonDelivery->click();
            return;
        };
        if (ui->pushButtonPaySystems->isVisible()) {
            ui->pushButtonPaySystems->click();
            return;
        };
        if (ui->pushButtonMailTemplates->isVisible()) {
            ui->pushButtonMailTemplates->click();
            return;
        };
        if (ui->pushButtonCurrencies->isVisible()) {
            ui->pushButtonCurrencies->click();
            return;
        };
    }
}

bool FormSettings::eventFilter(QObject *object, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        if (object == ui->treeViewPayParams)
            return eventKeysTreePayParams(keyEvent);
    }
    return false;
}

bool FormSettings::eventKeysTreePayParams(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Insert:
        if (ui->toolButtonAddPayParam->isEnabled())
            ui->toolButtonAddPayParam->click();
        return true;
    case Qt::Key_Delete:
        if (event->modifiers() & Qt::ControlModifier &&
                ui->toolButtonDeletePayParam->isEnabled()) {
            ui->toolButtonDeletePayParam->click();
            return true;
        }
    }
    return false;
}

void FormSettings::initVariables()
{
    mIsShowed = 0;
    mIsShowedMainPaySystems = 0;
    mIsShowedShopParameters = 0;
    mPaysSystemsModel = 0;
    mFormLicenses = 0;    
    mFormDeliveries = 0;    
    mFormPaySystems = 0;
    mFormTemplateMails = 0;
    mFormCurrencies = 0;    
    mIntegrations = 0;    
    mFormInterface = 0;
    mFormStores = 0;
    mFormSEOVariables = 0;
    mFormPermission = 0;
    mFormGeotargeting = 0;
    mFormMeasures = 0;
}

void FormSettings::initValidators()
{
    QRegExp regExpr("[a-zA-Z0-9_-]*");
    QRegExpValidator *validatorLatin = new QRegExpValidator(regExpr, this);
    ui->lineEditShopFolder->setValidator(validatorLatin);
    mPreloader = new Preloader(ui->widgetPreloader);
}

void FormSettings::initSignals()
{
    QObjectList list = ui->tabSettingsShop->children();
    list.append(ui->tabSettingsCompany->children());
    for (int i = 0; i < list.size(); i++)
        if (QLineEdit *lineEdit = qobject_cast<QLineEdit *>(list.at(i))) {
            connect(lineEdit, SIGNAL(editingFinished()),
                    SLOT(saveMainSettings()));
        }
    connect(ui->checkBoxIsYAStore, SIGNAL(stateChanged(int)), SLOT(saveMainSettings()));
    connect(ui->checkBoxIsYAPickup, SIGNAL(stateChanged(int)), SLOT(saveMainSettings()));
    connect(ui->checkBoxIsYADelivery, SIGNAL(stateChanged(int)), SLOT(saveMainSettings()));
    connect(ui->doubleSpinBoxCostYALocalDelivery, SIGNAL(valueChanged(double)), SLOT(saveMainSettings()));
    connect(ui->comboBoxBaseCurrency, SIGNAL(currentIndexChanged(int)), SLOT(saveMainSettings()));
    connect(ui->tabWidgetMainSettings, SIGNAL(currentChanged(int)), SLOT(onTabMainSettingsChanged(int)));
    connect(ui->toolButtonAddPayParam, SIGNAL(clicked()), SLOT(onAddParamClicked()));
    connect(ui->toolButtonDeletePayParam, SIGNAL(clicked()), SLOT(onDeleteParamClicked()));
    connect(ui->toolButtonSetValuePayParam, SIGNAL(clicked()), SLOT(onSetValueClicked()));
    connect(ui->treeViewPayParams, SIGNAL(doubleClicked(QModelIndex)), ui->toolButtonSetValuePayParam, SLOT(click()));
    connect(ui->toolButtonRefreshIntegrations, &QAbstractButton::clicked, this, &FormSettings::refreshIntegrations);
    connect(ui->toolButtonSaveIntegrations, &QAbstractButton::clicked, this, &FormSettings::saveIntegrations);
    connect(ui->toolButtonRefreshParameters, &QAbstractButton::clicked, this, &FormSettings::refreshShopParameters);
    connect(ui->pushButtonClearCache, &QAbstractButton::clicked, this, &FormSettings::clearCache);
    connect(ui->pushButtonCreateDump, SIGNAL(clicked(bool)), SLOT(onCreateDump()));
    connect(ui->pushButtonRestoreDump, SIGNAL(clicked(bool)), SLOT(onRestoreDump()));
    connect(ui->pushButtonImagesMigration, SIGNAL(clicked(bool)), SLOT(onImagesMigration()));
    connect(ui->pushButtonDelImagesFromBD, SIGNAL(clicked(bool)), SLOT(onImagesDelFromBD()));
}

void FormSettings::initPermissions()
{
    for (int i = 0; i < ui->tabWidgetMainSettings->children().count(); ++i)
        for (int j = 0; j < ui->tabWidgetMainSettings->children().at(i)->children().count(); ++j) {
            if (QWidget *widget = qobject_cast <QWidget *> (ui->tabWidgetMainSettings->
                                                            children().at(i)->children().at(j)))
                widget->setEnabled(UserPermission::isEdit(UserPermission::SETTINGS));
        }
    if (!UserPermission::isRead(UserPermission::DELIVERIES))
        ui->pushButtonDelivery->hide();
    if (!UserPermission::isRead(UserPermission::PAYSYSTEMS))
        ui->pushButtonPaySystems->hide();
    if (!UserPermission::isRead(UserPermission::MAILS))
        ui->pushButtonMailTemplates->hide();
    if (!UserPermission::isRead(UserPermission::CURRENCIES))
        ui->pushButtonCurrencies->hide();
    if (!UserPermission::isAdmin) {
        ui->pushButtonPermission->hide();
        ui->groupBoxDB->hide();
    }
}

void FormSettings::setDataMain()
{
    mDataMain->setNameCompany(ui->lineEditNameCompany->text());
    if (mDataMain->shopName() != ui->lineEditShopName->text().trimmed())
        emit changedShopName(ui->lineEditShopName->text().trimmed());
    mDataMain->setShopName(ui->lineEditShopName->text());
    mDataMain->setSubName(ui->lineEditSubName->text());
    mDataMain->setURLLogo(ui->lineEditUrlLogotip->text());
    mDataMain->setDomain(ui->lineEditDomain->text());
    mDataMain->setEmailSales(ui->lineEditEmailSales->text());
    mDataMain->setEmailSupport(ui->lineEditEmailSupport->text());    
    mDataMain->setHead(ui->lineEditHead->text());
    mDataMain->setPostHead(ui->lineEditPostHead->text());
    mDataMain->setBookkeeper(ui->lineEditBookkeeper->text());
    mDataMain->setMailAddress(ui->lineEditMailAddress->text());
    mDataMain->setMainAddress(ui->lineEditMainAddress->text());
    mDataMain->setPhone(ui->lineEditPhone->text());
    mDataMain->setFax(ui->lineEditFax->text());
    mDataMain->setNDS(ui->lineEditNDS->text().toFloat());
    mDataMain->setIsYAStore(ui->checkBoxIsYAStore->isChecked());
    mDataMain->setIsYAPickup(ui->checkBoxIsYAPickup->isChecked());
    mDataMain->setIsYADelivery(ui->checkBoxIsYADelivery->isChecked());
    mDataMain->setCostYALocalDelivery(ui->doubleSpinBoxCostYALocalDelivery->value());
    mDataMain->setShopFolder(ui->lineEditShopFolder->text().trimmed());

    for (int i = 0; i < mDataMain->listCurrency()->size(); i++) {
        DataItem *curr = mDataMain->listCurrency()->at(i);
        if (ui->comboBoxBaseCurrency->currentText() ==
                curr->getName()) {
            mDataMain->setBaseCurrency(curr->getCode());
            break;
        }
    }
}

void FormSettings::fillControlsForMain()
{
    ui->lineEditNameCompany->setText(mDataMain->nameCompany());
    ui->lineEditShopName->setText(mDataMain->shopName());
    ui->lineEditSubName->setText(mDataMain->subName());
    ui->lineEditUrlLogotip->setText(mDataMain->urlLogo());
    ui->lineEditDomain->setText(mDataMain->domain());
    ui->lineEditEmailSales->setText(mDataMain->emailSales());
    ui->lineEditEmailSupport->setText(mDataMain->emailSupport());    
    ui->lineEditHead->setText(mDataMain->head());
    ui->lineEditPostHead->setText(mDataMain->postHead());
    ui->lineEditBookkeeper->setText(mDataMain->bookkeeper());
    ui->lineEditMailAddress->setText(mDataMain->mailAddress());
    ui->lineEditMainAddress->setText(mDataMain->mainAddress());
    ui->lineEditPhone->setText(mDataMain->phone());
    ui->lineEditFax->setText(mDataMain->fax());
    ui->lineEditNDS->setText(QString::number(mDataMain->nds()));
    ui->checkBoxIsYAStore->setChecked(mDataMain->isYAStore());
    ui->checkBoxIsYAPickup->setChecked(mDataMain->isYAPickup());
    ui->checkBoxIsYADelivery->setChecked(mDataMain->isYADelivery());
    ui->doubleSpinBoxCostYALocalDelivery->setValue(mDataMain->costYALocalDelivery());
    ui->lineEditShopFolder->setText(mDataMain->shopFolder());

    ui->comboBoxBaseCurrency->clear();
    int j = -1;
    for (int i = 0; i < mDataMain->listCurrency()->size(); i++) {
        DataItem *curr = mDataMain->listCurrency()->at(i);
        if (curr->getCode() == mDataMain->baseCurrency())
            j = i;
        ui->comboBoxBaseCurrency->addItem(curr->getName());
    }
    ui->comboBoxBaseCurrency->setCurrentIndex(j);
    mDataMain->setUnModified();
}

void FormSettings::refreshData()
{
    setCursor(QCursor(Qt::WaitCursor));
    if (ApiAdapter::getApi()->getInfoMain(mDataMain))
        fillControlsForMain();    
    setCursor(QCursor(Qt::ArrowCursor));
}

void FormSettings::refreshPayParams()
{
    if (!ui->treeViewPaySystems->currentIndex().isValid())
        return;

    mRequest.clear();
    mRequest.setIdPayment(mPaysSystemsModel->
                          idData(ui->treeViewPaySystems->currentIndex()));
    mPayReqsModel->getItems()->deleteItems();
    mPayReqsModel->beginReset();
    ApiAdapter::getApi()->getListPayrequisites(mRequest, mPayReqsModel->getItems());
    mPayReqsModel->endReset();

    if (mPayReqsModel->getItems()->size())
        ui->treeViewPayParams->
                setCurrentIndex(mPayReqsModel->index(0, 0));
}

void FormSettings::showMainPaySystems()
{
    if (!mPaysSystemsModel) {
        SEConfig::loadSplitterLayout(ui->splitterPaysSystems);
        mPaysSystemsModel = new NameTableModel(this);
        mPaysSystemsModel->setListData(mPaySystems);
        ui->treeViewPaySystems->setModel(mPaysSystemsModel);
        mPayReqsModel = new NameTableModel(this, mPayReqs);
        mPayReqsModel->clearFields();
        mPayReqsModel->appendField(new Field("code", "Код"));
        mPayReqsModel->appendField(new Field("name", "Заголовок"));
        mPayReqsModel->appendField(new Field("value", "Значение"));
        ui->treeViewPayParams->setModel(mPayReqsModel);
        SEConfig::restoreTableState(ui->treeViewPayParams->header(),
                                        ui->treeViewPayParams->objectName());        
        ui->treeViewPayParams->setSelectionMode(QAbstractItemView::ExtendedSelection);
        ui->treeViewPayParams->installEventFilter(this);
        connect(ui->treeViewPaySystems->selectionModel(),
                SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
                SLOT(onChangeTreePaySystems()));
        connect(ui->treeViewPayParams->selectionModel(),
                SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
                SLOT(onChangeTreePayParams()));
    }

    mPaysSystemsModel->beginReset();
    ApiAdapter::getApi()->getListPaySystems(reinterpret_cast <ListDataPaySystems *> (mPaysSystemsModel->getItems()));
    mPaysSystemsModel->endReset();

    if (mPaysSystemsModel->getItems()->size())
        ui->treeViewPaySystems->
                setCurrentIndex(mPaysSystemsModel->index(0, 0));

    mIsShowedMainPaySystems = 1;
}

void FormSettings::showIntegrations()
{
    for (int i = 0; i < ui->toolBoxIntegrations->count(); i++)
        ui->toolBoxIntegrations->removeItem(i);
    mListIntegrationsModels.clear();
    mListIntegrationsNoteLabels.clear();
    mListIntegrationsTree.clear();

    setCursor(Qt::WaitCursor);

    if (!mIntegrations)
        mIntegrations = new ListIntegrations;

    if (ApiAdapter::getApi()->getListIntegrations(mIntegrations)) {
        for (int i = 0; i < mIntegrations->size(); ++i) {
            ui->toolBoxIntegrations->addItem(new QWidget(ui->toolBoxIntegrations), mIntegrations->at(i)->getName());
            QHBoxLayout *layout = new QHBoxLayout;
            QTableView *table = new QTableView(this);            
            table->horizontalHeader()->setStretchLastSection(true);
            table->horizontalHeader()->setHighlightSections(false);
            table->setEditTriggers(QAbstractItemView::AllEditTriggers);
            IntegrationsModel *model = new IntegrationsModel(this, mIntegrations->at(i)->listParameters());
            connect(model, SIGNAL(modified()), SLOT(onChangeDataIntegrations()));
            mListIntegrationsModels.append(model);
            mListIntegrationsTree.append(table);
            table->setModel(model);
            IntegrationsDelegate *delegate = new IntegrationsDelegate(this, model);
            table->setItemDelegate(delegate);
            layout->addWidget(table);
            layout->setMargin(0);
            table->resizeColumnsToContents();
            QGroupBox *box = new QGroupBox(this);
            box->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            box->setTitle(tr("Примечание по параметру"));
            QHBoxLayout *layoutBox = new QHBoxLayout;
            QLabel *labelNote = new QLabel(this);
            layoutBox->addWidget(labelNote);
            box->setLayout(layoutBox);
            labelNote->setWordWrap(true);
            labelNote->setAlignment(Qt::AlignTop);
            labelNote->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            labelNote->setFont(QFont(labelNote->font().family(), 12));
            mListIntegrationsNoteLabels.append(labelNote);
            layout->addWidget(box);
            connect(table->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
                    SLOT(onChangeTreeIntegrations()));
            connect(table, SIGNAL(activated(QModelIndex)), table, SLOT(edit(QModelIndex)));
            ui->toolBoxIntegrations->widget(i)->setLayout(layout);
        }
        if (mIntegrations->size()) {
            ui->toolBoxIntegrations->setCurrentIndex(ui->toolBoxIntegrations->count() - 1);
            if (mListIntegrationsTree.size() && mIntegrations->at(0)->listParameters()->size()) {
                mListIntegrationsTree.at(0)->selectionModel()->setCurrentIndex(
                            mListIntegrationsModels.at(0)->index(0, 0), QItemSelectionModel::SelectCurrent);
                mListIntegrationsTree.at(0)->setFocus();
            }
        }
    }
    setCursor(Qt::ArrowCursor);
}

void FormSettings::refreshIntegrations()
{
    showIntegrations();
}

void FormSettings::showShopParameters()
{
    if (mIsShowedShopParameters)
        return;

    setCursor(Qt::WaitCursor);
    ListIntegrations *shopParameters = SEConfig::getShopParameters();
    if (shopParameters->size() || ApiAdapter::getApi()->getListShopParameters(shopParameters)) {
        mIsShowedShopParameters = true;
        for (int i = 0; i < shopParameters->size(); ++i) {
            QWidget *page = new QWidget(ui->tabWidgetParameters);
            page->setAutoFillBackground(true);
            ui->tabWidgetParameters->addTab(page, shopParameters->at(i)->getName());
            QHBoxLayout *layout = new QHBoxLayout;
            QTableView *table = new QTableView(this);
            connect(table, SIGNAL(clicked(QModelIndex)), SLOT(onClickTreeShopParameters(QModelIndex)));
            table->horizontalHeader()->setStretchLastSection(true);
            table->horizontalHeader()->setHighlightSections(false);
            table->setEditTriggers(QAbstractItemView::AllEditTriggers);
            IntegrationsModel *model = new IntegrationsModel(this, shopParameters->at(i)->listParameters());
            model->setIsParametersOnOffMode();
            connect(model, SIGNAL(modified()), SLOT(onChangeDataShopParameters()));
            mListShopParametersModels.append(model);
            mListShopParametersTree.append(table);
            table->setModel(model);
            IntegrationsDelegate *delegate = new IntegrationsDelegate(this, model);
            delegate->setIsParametersOnOffMode();
            table->setItemDelegate(delegate);
            layout->addWidget(table);
            layout->setMargin(0);
            table->resizeColumnsToContents();
            QGroupBox *box = new QGroupBox(this);
            box->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            box->setTitle(tr("Примечание по параметру"));
            QHBoxLayout *layoutBox = new QHBoxLayout;
            QLabel *labelNote = new QLabel(this);
            layoutBox->addWidget(labelNote);
            box->setLayout(layoutBox);
            labelNote->setFont(QFont(labelNote->font().family(), 12));
            labelNote->setWordWrap(true);
            labelNote->setAlignment(Qt::AlignTop);
            labelNote->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            mListShopParametersNoteLabels.append(labelNote);
            layout->addWidget(box);            
            connect(table->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
                    SLOT(onChangeTreeShopParameters()));
            connect(table, SIGNAL(activated(QModelIndex)), table, SLOT(edit(QModelIndex)));
            page->setLayout(layout);
            if (!i && shopParameters->at(i)->listParameters()->size()) {
                table->setCurrentIndex(model->index(0, 0));
                table->setFocus();
            }
        }        
    }    
    setCursor(Qt::ArrowCursor);
}

void FormSettings::refreshShopParameters()
{
    ui->tabWidgetParameters->clear();
    mListShopParametersModels.clear();
    mListShopParametersTree.clear();
    mListShopParametersNoteLabels.clear();
    mIsShowedShopParameters = false;
    SEConfig::getShopParameters()->clear();
    showShopParameters();
}

void FormSettings::onTabMainSettingsChanged(int index)
{
    if (index == 2)
        showShopParameters();
    if (index == 3)
        showMainPaySystems();
    if (index == 4)
        showIntegrations();
}

void FormSettings::onChangeTreePaySystems()
{
    ui->toolButtonAddPayParam->setEnabled(ui->treeViewPaySystems->currentIndex().isValid());
    ui->toolButtonDeletePayParam->setEnabled(false);
    ui->toolButtonSetValuePayParam->setEnabled(false);
    refreshPayParams();
}

void FormSettings::onChangeTreePayParams()
{
    ui->toolButtonDeletePayParam->setEnabled(ui->treeViewPayParams->currentIndex().isValid());
    ui->toolButtonSetValuePayParam->setEnabled(ui->treeViewPayParams->currentIndex().isValid());
}

void FormSettings::onChangeTreeIntegrations()
{
    QModelIndex index = mListIntegrationsTree.at(ui->toolBoxIntegrations->currentIndex())->currentIndex();

    QString note =
            mIntegrations->at(ui->toolBoxIntegrations->currentIndex())->listParameters()->at(index.row())->property("note").toString();
    mListIntegrationsNoteLabels.at(ui->toolBoxIntegrations->currentIndex())->setText(note);
}

void FormSettings::onChangeDataIntegrations()
{
    ui->toolButtonSaveIntegrations->setEnabled(1);
    saveIntegrations();
}

void FormSettings::onChangeTreeShopParameters()
{
    if (!mListShopParametersTree.size())
        return;
    QModelIndex index = mListShopParametersTree.at(ui->tabWidgetParameters->currentIndex())->currentIndex();
    if (!index.isValid())
        return;

    ListIntegrations *shopParameters = SEConfig::getShopParameters();
    QString note =
            shopParameters->at(ui->tabWidgetParameters->currentIndex())->listParameters()->at(index.row())->property("note").toString();
    mListShopParametersNoteLabels.at(ui->tabWidgetParameters->currentIndex())->setText(note);
}

void FormSettings::onChangeDataShopParameters()
{
    saveDataShopParameters();
}

void FormSettings::onClickTreeShopParameters(const QModelIndex &index)
{
    QTableView *table = qobject_cast <QTableView *> (sender());
    if (table) {
        IntegrationsModel *model = qobject_cast <IntegrationsModel *> (table->model());
        if (model && index.column() == 1) {
            DataItem *item = model->dataItem(index);
            if (item) {
                item->setIsActive(!item->getIsActive());
                model->repaintModel();
                onChangeDataShopParameters();
            }
        }
        table->selectionModel()->setCurrentIndex(index, QItemSelectionModel::Current);
    }
}


void FormSettings::onAddParamClicked()
{
    if (!ui->treeViewPaySystems->currentIndex().isValid())
        return;

    DialogAddParamPaySystem dialogParam(this);
    dialogParam.setIdPayment(mPaysSystemsModel->
                            idData(ui->treeViewPaySystems->currentIndex()));
    if (dialogParam.exec() == QDialog::Accepted)
        refreshPayParams();
}

void FormSettings::onSetValueClicked()
{
    if (!ui->treeViewPayParams->currentIndex().isValid())
        return;

    DialogSetPayParamValue dialogValue;
    DataItem *item = mPayReqsModel->
            dataItem(ui->treeViewPayParams->currentIndex());
    dialogValue.setParam(item);
    if (dialogValue.exec() == QDialog::Accepted)
        item->setValue(dialogValue.value());
}

void FormSettings::onDeleteParamClicked()
{
    if (!ui->treeViewPayParams->currentIndex().isValid())
        return;

    DataItem *item = mPayReqsModel->
            dataItem(ui->treeViewPayParams->currentIndex());

    bool isMulti = ui->treeViewPayParams->selectionModel()->
            selectedRows().count() > 1;

    QString msg;
    if (isMulti)
        msg = tr("Удалить выбранные параметры?");
    else msg = tr("Удалить параметр: ") + item->getName();

    QMessageBox::StandardButton mesRepl;
    mesRepl = QMessageBox::question(this, tr("Подтверждение удаления"),
                                    msg, QMessageBox::Yes | QMessageBox::No);
    if(mesRepl == QMessageBox::Yes) {
        Request request;
        QModelIndexList list = ui->treeViewPayParams->selectionModel()->selectedRows();

        for (int i = 0; i < list.count(); i++)
            request.addId(mPayReqsModel->idData(list.at(i)));
            ApiAdapter::getApi()->deletePayParams(request);
        refreshPayParams();
    }
}

void FormSettings::onCreateDump()
{
    QString folderDumps = QFileDialog::getExistingDirectory(this, tr("Папка для резервной копии"), mFolderDumps);
    if (!folderDumps.isEmpty()) {
        mFolderDumps = folderDumps;
        parentWidget()->parentWidget()->setEnabled(false);
        ui->widgetPreloader->show();
        mPreloader->startAnimation();
        ui->pushButtonRestoreDump->hide();
        ui->pushButtonCreateDump->setEnabled(false);
        ui->labelInfo->setText("Идёт творческий процесс создания резервной копии БД");
        ui->labelInfo->show();
        QSettings settings;
        settings.setValue("dumpsFolder", mFolderDumps);
        settings.sync();
        QString fileDump = mFolderDumps + QDir::separator() + SEConfig::projectHostName + ".sql.gz";
        Request request;
        request.addStringValue("command", "createDump");
        QByteArray data;
        bool result = ApiAdapter::getApi()->getBinaryData(SEConsts::API_MIGRATION, SEConsts::APIMETHOD_COMMAND, request, &data);        
        if (result) {            
            QFile file(fileDump);
            if (file.open(QFile::WriteOnly))
                file.write(data);
            file.close();            
        }
        parentWidget()->parentWidget()->setEnabled(true);
        mPreloader->stopAnimation();
        ui->pushButtonRestoreDump->show();
        ui->pushButtonCreateDump->setEnabled(true);
        ui->labelInfo->setText(QString());
        ui->labelInfo->hide();
        if (result)
            QMessageBox::information(this, "Резервная копия БД", "Создание резервной копии БД завершено!");
        else QMessageBox::critical(this, "Резервная копия БД", "Не удаётся создать резервную копию БД! Возможно БД содержит большой объём данных.");
    }    
}

void FormSettings::onRestoreDump()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Файл резервной копии"), mFolderDumps,
                                         "Файлы дампа (*.sql *.gz)");
    if (fileName.isEmpty())
        return;

    QFile *file = new QFile(fileName);
    if (!file->exists()) {
        QMessageBox::critical(0, tr("Ошибка чтения файла"), tr("Не удаётся найти файл") + ": " + fileName + "!");
        delete file;
        return;
    }
    if(!file->open(QIODevice::ReadOnly))
    {
        QMessageBox::critical(0, tr("Ошибка чтения файла"), tr("Не удаётся прочитать файл") + ": " + fileName + "!");
        delete file;
        return;
    }

    parentWidget()->parentWidget()->setEnabled(false);
    ui->widgetPreloader->show();
    mPreloader->startAnimation();
    ui->pushButtonCreateDump->hide();
    ui->labelInfo->setText("Идёт творческий процесс восстановление БД из резервной копии.");
    ui->labelInfo->show();

    QByteArray data(file->readAll());
    QFileInfo fileInfo(fileName);
    QString result = ApiAdapter::getApi()->sendBinaryData(SEConsts::API_MIGRATION, SEConsts::APIMETHOD_COMMAND,
                                                    QString("command=restoreDump"), &data, fileInfo.fileName());  
    parentWidget()->parentWidget()->setEnabled(true);
    mPreloader->stopAnimation();
    ui->pushButtonCreateDump->show();
    ui->labelInfo->setText(QString());
    ui->labelInfo->hide();

    if (result == "ok") {
        QMessageBox::information(this, "Резервная копия БД",
                                 "Восстановление БД из резервной копии завершено! Программа будет перезапущена.");
        emit reloadProject();
    } else QMessageBox::critical(this, "Резервная копия БД",
                                 "Не удаётся восстановить БД из резервной копии! Обратитесь в тех. поддержку.");
}

void FormSettings::onImagesMigration()
{
    ui->pushButtonImagesMigration->setEnabled(false);
    DialogMigrationImages dialogMigration(this);
    if (dialogMigration.exec() == QDialog::Accepted) {
        QMessageBox::information(this, "Миграция изображений",
                                 "Копирования изображений на проект: " +
                                 dialogMigration.getProjectMigration() + " завершено!");
    }
    ui->pushButtonImagesMigration->setEnabled(true);
}

void FormSettings::onImagesDelFromBD()
{
    QString msg;
    msg = tr("Удалить записи в БД для не найденных изображений?");

    QMessageBox::StandardButton mesRepl;
    mesRepl = QMessageBox::question(this, tr("Подтверждение удаления"),
                                    msg, QMessageBox::Yes | QMessageBox::No);
    if(mesRepl == QMessageBox::Yes) {

        ui->pushButtonDelImagesFromBD->setEnabled(false);

        QJsonObject answerJSON;
        Request request;
        request.addStringValue("command", "delImagesInBD");
        api->executeAPI(SEConsts::API_MIGRATION, SEConsts::APIMETHOD_COMMAND, request.getAsJsonString(), &answerJSON);

        ui->pushButtonDelImagesFromBD->setEnabled(true);

    }
}

void FormSettings::clearCache()
{
    ApiAdapter::getApi()->clearCache();
}

void FormSettings::on_pushButtonLicenses_clicked()
{
    if (!mFormLicenses) {
        mFormLicenses = new FormLicense(this);
        ui->stackedWidgetMainSettings->addWidget(mFormLicenses);
    }
    ui->stackedWidgetMainSettings->setCurrentWidget(mFormLicenses);

}

void FormSettings::on_pushButtonMailTemplates_clicked()
{
    if (!mFormTemplateMails) {
        mFormTemplateMails = new FormTemplateMails(this);
        ui->stackedWidgetMainSettings->addWidget(mFormTemplateMails);
    }
    ui->stackedWidgetMainSettings->setCurrentWidget(mFormTemplateMails);
}

void FormSettings::on_pushButtonCurrencies_clicked()
{
    if (!mFormCurrencies) {
        mFormCurrencies = new FormCurrencies(this, mDataMain);
        ui->stackedWidgetMainSettings->addWidget(mFormCurrencies);
        connect(mFormCurrencies, &FormCurrencies::changeIndexCurrencyBox, ui->comboBoxBaseCurrency,
                &QComboBox::setCurrentIndex);
    }
    ui->stackedWidgetMainSettings->setCurrentWidget(mFormCurrencies);
}

void FormSettings::on_pushButtonInterface_clicked()
{
    if (!mFormInterface) {
        mFormInterface = new FormSettingsInterface(this);
        ui->stackedWidgetMainSettings->addWidget(mFormInterface);
    }
    ui->stackedWidgetMainSettings->setCurrentWidget(mFormInterface);
}

void FormSettings::on_pushButtonStores_clicked()
{
    if (!mFormStores) {
        mFormStores = new FormIntegrationsStores(this);
        ui->stackedWidgetMainSettings->addWidget(mFormStores);
    }
    ui->stackedWidgetMainSettings->setCurrentWidget(mFormStores);
}

void FormSettings::on_pushButtonSEOVariables_clicked()
{
    if (!mFormSEOVariables) {
        mFormSEOVariables = new FormSEOVariables(this);
        ui->stackedWidgetMainSettings->addWidget(mFormSEOVariables);
    }
    ui->stackedWidgetMainSettings->setCurrentWidget(mFormSEOVariables);

}

void FormSettings::on_pushButtonPermission_clicked()
{
    if (!mFormPermission) {
        mFormPermission = new FormPermission(this);
        ui->stackedWidgetMainSettings->addWidget(mFormPermission);
    }
    ui->stackedWidgetMainSettings->setCurrentWidget(mFormPermission);
}

void FormSettings::on_pushButtonGeotargeting_clicked()
{
    if (!mFormGeotargeting) {
        mFormGeotargeting = new FormCatalog(this, SEConsts::API_GEOTARGETING_CONTACTS,
                                            UserPermission::SETTINGS);
        mFormGeotargeting->setWindowTitle("Геотаргетинг");
        mFormGeotargeting->setTitleButtonAdd("Создать контакт", "Создать новый контакт");
        mFormGeotargeting->addField(new Field("id", "Ид.", Qt::AlignRight | Qt::AlignVCenter));
        mFormGeotargeting->addField(new Field("name", "Наименование"));
        mFormGeotargeting->addField(new Field("address", "Адрес"));
        mFormGeotargeting->addField(new Field("city", "Город"));
        mFormGeotargeting->addField(new Field("phone", "Телефон"));
        mFormGeotargeting->addField(new Field("additionalPhones", "Доп. телефоны"));
        mFormGeotargeting->addField(new Field("domain", "Домен"));
        mFormGeotargeting->addField(new Field("sortIndex", "Порядок", Qt::AlignRight | Qt::AlignVCenter));
        mFormGeotargeting->addField(new Field("description", "Примечание"));
        DialogCardGeotageting *dialogCard = new DialogCardGeotageting(mFormGeotargeting);
        mFormGeotargeting->setDialogCard(dialogCard);
        ui->stackedWidgetMainSettings->addWidget(mFormGeotargeting);
    }
    ui->stackedWidgetMainSettings->setCurrentWidget(mFormGeotargeting);
}

void FormSettings::on_pushButtonMeasures_clicked()
{
    if (!mFormMeasures) {
        mFormMeasures = new FormMeasures(this);
        ui->stackedWidgetMainSettings->addWidget(mFormMeasures);
    }
    ui->stackedWidgetMainSettings->setCurrentWidget(mFormMeasures);
}
