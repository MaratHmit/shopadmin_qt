#include "formcarddelivery.h"
#include "ui_formcarddelivery.h"
#include "apiadapter.h"
#include "seconfig.h"
#include "dataproductsgroup.h"
#include "datatypedelivery.h"
#include "dialogdelivcondition.h"
#include "dialogdelivregion.h"
#include "field.h"
#include "formdeliveries.h"
#include "userpermission.h"

#include <QSettings>

FormCardDelivery::FormCardDelivery(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FormCardDelivery)
{
    ui->setupUi(this);

    setWindowFlags(Qt::Window);    
    initVariables();
    initSignals();
    initPermissions();
}

FormCardDelivery::~FormCardDelivery()
{    
    delete ui;   
}

bool FormCardDelivery::save()
{       
    mDelivery->setIsSetAllFields(true);    
    return ApiAdapter::getApi()->saveDelivery(mDelivery);
}

void FormCardDelivery::setDelivery(const DataDelivery *delivery)
{
    mDelivery->copy(delivery);
}

void FormCardDelivery::on_tabWidgetDelivery_currentChanged(int index)
{
    switch (index) {
    case 1:
        showConditions();
        break;
    case 2:
        showPaySystems();
        break;
    case 3:
        showGroups();    
    case 4:
        showGeoLocationsRegions();
    default:
        break;
    }
}

void FormCardDelivery::initVariables()
{
    ui->tabWidgetDelivery->setCurrentIndex(0);
    mIsShowedPaySystems = 0;
    mIsShowedGroups = 0;
    mIsShowedConditions = 0;
    mIsShowedGeoLocationsRegions = 0;
    mModelPays = 0;
    mModelGroups = 0;
    mModelConditionsParameters = 0;
    mModelConditionsRegions = 0;
    mModelGeoLocationsRegions = 0;
    mIsChildMode = 0;    
    mCountries = 0;
    mRegions = 0;

    mCities = new ListDataItems;
    mDelivery = new DataDelivery;
    ui->comboBoxType->clear();
    for (int i = 0; i < SEConfig::getTypesDeliveries()->size(); ++i)
        ui->comboBoxType->addItem(SEConfig::getTypesDeliveries()->at(i)->getName(),
                                  SEConfig::getTypesDeliveries()->at(i)->getCode());
    setValidators();
    onChangeTypeDelivery();
}

void FormCardDelivery::initCurrencies()
{    
    mListCurrencies = new ListCurrencies();
    if (ApiAdapter::getApi()->getListCurrencies(mListCurrencies)) {
        ui->comboBoxCurrencies->clear();
        for (int i = 0; i < mListCurrencies->size(); i++) {
            ui->comboBoxCurrencies->addItem(mListCurrencies->at(i)->getName(),
                                            mListCurrencies->at(i)->getCode());
            if (mListCurrencies->at(i)->getCode() == SEConfig::getMainInfo()->baseCurrency())
                ui->comboBoxCurrencies->setCurrentIndex(i);
        }
    }
}

void FormCardDelivery::initSignals()
{
    connect(ui->comboBoxType, SIGNAL(currentIndexChanged(int)), SLOT(onChangeTypeDelivery()));
}

void FormCardDelivery::initRegions()
{
    setCursor(QCursor(Qt::WaitCursor));
    if (!mCountries)
        mCountries = SEConfig::getCountries();
    if (!mRegions)
        mRegions = SEConfig::getRegions();
    setCursor(QCursor(Qt::ArrowCursor));
}

void FormCardDelivery::initPermissions()
{

}

void FormCardDelivery::setValidators()
{
    QRegExp regExprNums("[\\d]*");
    QRegExpValidator *regNumbers = new QRegExpValidator(regExprNums, this);
    ui->lineEditPeriod->setValidator(regNumbers);
    ui->lineEditMaxVolume->setValidator(regNumbers);

    QRegExp regExprFloat("\\d*\\.{1}\\d*$|^\\d*$");
    QRegExpValidator *regFloats = new QRegExpValidator(regExprFloat, this);
    ui->lineEditPrice->setValidator(regFloats);
    ui->lineEditMaxWeight->setValidator(regFloats);
}

void FormCardDelivery::showPaySystems()
{
    if (mIsShowedPaySystems)
        return;

    mListPaySystems = new ListDataPaySystems();
    mModelPays = new NameTableModel(this, reinterpret_cast<ListDataItems *>(mListPaySystems));    
    mModelPays->setCheckedMode(true);
    ApiAdapter::getApi()->getListPaySystems(mListPaySystems);    
    setPaySystems();
    ui->treeViewPaySystems->setModel(mModelPays);
    connect(mModelPays, SIGNAL(checked(QString,bool)), SLOT(onCheckedPaySystem()));
    mIsShowedPaySystems = true;
}

void FormCardDelivery::setPaySystems()
{
    if (!mModelPays)
        return;

    for (int i = 0; i < mDelivery->idsPaySystems()->size(); i++){
        for (int j = 0; j < mModelPays->getItems()->size(); j++) {
            DataItem *item = mModelPays->getItems()->at(j);
            if (mDelivery->idsPaySystems()->at(i) == item->getId())
                item->setIsChecked(true);
        }
   }
}

void FormCardDelivery::showGroups()
{
    if (mIsShowedGroups)
        return;

    mListGroups = new ListProductsGroups();
    mModelGroups = new NameTableModel(this, reinterpret_cast<ListDataItems *>(mListGroups));    
    mModelGroups->setCheckedMode(true);            
    ApiAdapter::getApi()->getListProductsGroups(Request(), mListGroups);    
    setCheckedGroups();
    ui->treeViewGroups->setModel(mModelGroups);
    connect(mModelGroups, SIGNAL(checked(QString,bool)), SLOT(onCheckedGroups(QString,bool)));
    mIsShowedGroups = true;
}

void FormCardDelivery::showConditions()
{
    if (mIsShowedConditions)
        return;

    initRegions();
    if (!mModelConditionsParameters) {
        mModelConditionsParameters = new NameTableModel(this, reinterpret_cast <ListDataItems *> (mDelivery->conditionsParams()));
        mModelConditionsParameters->clearFields();
        mModelConditionsParameters->appendField(new Field("decriptionPrice", tr("Формируемая цена")));
        mModelConditionsParameters->appendField(new Field("typeParam", tr("Аргумент")));
        mModelConditionsParameters->appendField(new Field("minValue", tr("Мин.знач.")));
        mModelConditionsParameters->appendField(new Field("maxValue", tr("Макс.знач.")));
        mModelConditionsParameters->appendField(new Field("priority", tr("Приоритет")));
        ui->tableViewConditions->setModel(mModelConditionsParameters);
        ui->tableViewConditions->resizeColumnsToContents();
        ui->tableViewConditions->setSelectionMode(
                    QAbstractItemView::ExtendedSelection);
        ui->tableViewConditions->setSelectionBehavior(
                    QAbstractItemView::SelectRows);
        connect(ui->toolButtonAddCondition, &QToolButton::clicked, this,
                &FormCardDelivery::onAddEditCondition);
        connect(ui->toolButtonEditCondition, &QToolButton::clicked, this,
                &FormCardDelivery::onAddEditCondition);
        connect(ui->toolButtonDelCondition, &QToolButton::clicked, this,
                &FormCardDelivery::onDeleteCondition);
        connect(ui->tableViewConditions->selectionModel(),
                SIGNAL(currentChanged(QModelIndex,QModelIndex)),
                SLOT(onChangeCondition()));
        connect(ui->tableViewConditions, SIGNAL(doubleClicked(QModelIndex)),
                ui->toolButtonEditCondition, SLOT(click()));
    }

    if (!mModelConditionsRegions) {
        mModelConditionsRegions = new NameTableModel(this, reinterpret_cast <ListDataItems *>(mDelivery->conditionsRegions()));
        mModelConditionsRegions->clearFields();
        mModelConditionsRegions->appendField(new Field("geoDisplay", tr("Страна/город")));
        mModelConditionsRegions->appendField(new Field("price", tr("Цена"), Qt::AlignRight | Qt::AlignVCenter));
        mModelConditionsRegions->appendField(new Field("period", tr("Срок"), Qt::AlignRight | Qt::AlignVCenter));
        mModelConditionsRegions->appendField(new Field("note", tr("Адрес склада/примечание")));
        ui->treeViewConditionsRegions->setModel(mModelConditionsRegions);
        connect(ui->treeViewConditionsRegions->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
                SLOT(onChangeConditionRegion()));
        connect(ui->toolButtonAddCondtitionRegion, &QToolButton::clicked, this, &FormCardDelivery::onAddEditConditionRegion);
        connect(ui->toolButtonEditConditionRegion, &QToolButton::clicked, this, &FormCardDelivery::onAddEditConditionRegion);
        connect(ui->toolButtonDelConditionRegion, &QToolButton::clicked, this, &FormCardDelivery::onDeleteConditionRegion);
        connect(ui->treeViewConditionsRegions, &QAbstractItemView::doubleClicked, ui->toolButtonEditConditionRegion, &QToolButton::click);
        if (mDelivery->conditionsRegions())
            ui->treeViewConditionsRegions->setCurrentIndex(mModelConditionsRegions->index(0, 0));
        for (int i = 0; i < mModelConditionsRegions->columnCount(); ++i)
            ui->treeViewConditionsRegions->resizeColumnToContents(i);
        ui->treeViewConditionsRegions->setFocus();
    }
}

void FormCardDelivery::showGeoLocationsRegions()
{
    if (mIsShowedGeoLocationsRegions)
        return;

    initRegions();
    if (!mModelGeoLocationsRegions) {
        mModelGeoLocationsRegions = new NameTableModel(this, reinterpret_cast <ListDataItems *>(mDelivery->geoLocationsRegions()));
        mModelGeoLocationsRegions->clearFields();
        mModelGeoLocationsRegions->appendField(new Field("geoDisplay", tr("Страна/регион/город")));
        ui->treeViewGeo->setModel(mModelGeoLocationsRegions);
        connect(ui->treeViewGeo->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
                SLOT(onChangeGeoLocationRegion()));
        connect(ui->toolButtonAddGeo, &QToolButton::clicked, this, &FormCardDelivery::onAddEditGeo);
        connect(ui->toolButtonEditGeo, &QToolButton::clicked, this, &FormCardDelivery::onAddEditGeo);
        connect(ui->toolButtonDelGeo, &QToolButton::clicked, this, &FormCardDelivery::onDeleteGeo);
        connect(ui->treeViewGeo, &QAbstractItemView::doubleClicked, ui->toolButtonEditGeo, &QToolButton::click);
        if (mDelivery->geoLocationsRegions())
            ui->treeViewGeo->setCurrentIndex(mModelGeoLocationsRegions->index(0, 0));
        ui->treeViewGeo->setFocus();
        mIsShowedGeoLocationsRegions = true;
    }
}

bool FormCardDelivery::setCheckedGroupById(const QString &id)
{
    for (int i = 0; i < mListGroups->size(); ++i)
        if (mListGroups->at(i)->getId() == id) {
            mListGroups->at(i)->setIsChecked(true);
            break;
        }
    return true;
}

void FormCardDelivery::setCheckedGroups()
{
    for (int i = 0; i < mDelivery->getIdsGroups()->size(); i++)
        setCheckedGroupById(mDelivery->getIdsGroups()->at(i));
}

void FormCardDelivery::onCheckedPaySystem()
{
    mDelivery->setModified("idsPaySystems");
}

void FormCardDelivery::onCheckedGroups(const QString &, const bool &)
{
    SEConfig::saveStateModel(ui->treeViewGroups);
    mDelivery->setModified("idsGroups");
    mModelGroups->resetModel();
    SEConfig::restoreStateModel(ui->treeViewGroups);
}

void FormCardDelivery::onAddEditCondition()
{
    bool isNew = sender() == ui->toolButtonAddCondition;
    if (!isNew &&
        !ui->tableViewConditions->currentIndex().isValid())
        return;

    DataDeliveryConditions *condition;
    if (isNew)
        condition = new DataDeliveryConditions();
    else condition = new DataDeliveryConditions(
                mDelivery->conditionsParams()->at(ui->tableViewConditions->
                                            currentIndex().row()));
    DialogDelivCondition form(this, condition);
    if (form.exec() == QDialog::Accepted) {
        mModelConditionsParameters->beginReset();
        if (isNew)
            mDelivery->conditionsParams()->append(condition);
        else {
            mDelivery->conditionsParams()->at(ui->tableViewConditions->
                                        currentIndex().row())->copy(condition);
            delete condition;
        }
        mModelConditionsParameters->endReset();
    } else {
        delete condition;
    }
}

void FormCardDelivery::onDeleteCondition()
{
    if (!ui->tableViewConditions->currentIndex().isValid())
        return;

    mModelConditionsParameters->beginReset();
    QModelIndexList list = ui->tableViewConditions->selectionModel()->selectedRows();
    for (int i = 0; i < list.size(); i++)
        mDelivery->conditionsParams()->at(list.at(i).row())->setIsSelected(true);
    for (int i = 0; i < mDelivery->conditionsParams()->size(); i++)
        if (mDelivery->conditionsParams()->at(i)->getIsSelected())
            delete mDelivery->conditionsParams()->takeAt(i);
    mModelConditionsParameters->endReset();
}

void FormCardDelivery::onAddEditConditionRegion()
{
    bool isNew = sender() == ui->toolButtonAddCondtitionRegion;
    if (!isNew &&!ui->treeViewConditionsRegions->currentIndex().isValid())
        return;

    ui->toolButtonAddCondtitionRegion->setEnabled(false);
    ui->toolButtonEditConditionRegion->setEnabled(false);
    ui->toolButtonDelConditionRegion->setEnabled(false);
    DataDelivery *region;

    if (isNew) {
        region = new DataDelivery;
        region->setPrice(mDelivery->price());
        region->setMaxVolume(mDelivery->maxVolume());
        region->setMaxWeight(mDelivery->maxWeight());
        region->setPeriod(mDelivery->period());
        region->setIsActive(true);
        region->setCurrency(mDelivery->currency());
    } else region = mDelivery->conditionsRegions()->at(ui->treeViewConditionsRegions->currentIndex().row());

    DialogDelivRegion dialog(this, region);
    dialog.setExistRegions(mDelivery->conditionsRegions());

    if (dialog.exec() == QDialog::Accepted)  {
        mModelConditionsRegions->beginReset();
        if (isNew)
            mDelivery->conditionsRegions()->append(region);
        mModelConditionsRegions->endReset();
        for (int i = 0; i < mModelConditionsRegions->columnCount() - 1; ++i)
            ui->treeViewConditionsRegions->resizeColumnToContents(i);
        ui->treeViewConditionsRegions->setCurrentIndex(mModelConditionsRegions->index(mDelivery->conditionsRegions()->size() - 1, 0));
    } else {
        if (isNew)
            delete region;
        onChangeConditionRegion();
    }
}

void FormCardDelivery::onDeleteConditionRegion()
{
    if (!ui->treeViewConditionsRegions->currentIndex().isValid())
        return;

    mModelConditionsRegions->beginReset();
    QModelIndexList list = ui->treeViewConditionsRegions->selectionModel()->selectedRows();
    for (int i = 0; i < list.size(); i++)
        mDelivery->conditionsRegions()->at(list.at(i).row())->setIsSelected(true);
    for (int i = 0; i < mDelivery->conditionsRegions()->size(); i++)
        if (mDelivery->conditionsRegions()->at(i)->getIsSelected())
            delete mDelivery->conditionsRegions()->takeAt(i);
    mModelConditionsRegions->endReset();
}

void FormCardDelivery::onAddEditGeo()
{
    bool isNew = sender() == ui->toolButtonAddGeo;
    if (!isNew &&!ui->treeViewGeo->currentIndex().isValid())
        return;

    ui->toolButtonAddGeo->setEnabled(false);
    ui->toolButtonEditGeo->setEnabled(false);
    ui->toolButtonDelGeo->setEnabled(false);
    DataDelivery *region;

    if (isNew)
        region = new DataDelivery;
    else region = mDelivery->geoLocationsRegions()->at(ui->treeViewGeo->currentIndex().row());

    DialogDelivRegion dialog(this, region);
    dialog.setGeoMode();
    dialog.setExistRegions(mDelivery->geoLocationsRegions());

    if (dialog.exec() == QDialog::Accepted)  {
        mModelGeoLocationsRegions->beginReset();
        if (isNew) {
            mDelivery->geoLocationsRegions()->append(region);
            ui->treeViewGeo->setCurrentIndex(mModelGeoLocationsRegions->index(mDelivery->geoLocationsRegions()->size() - 1, 0));
        }
        mModelGeoLocationsRegions->endReset();
    } else {
        if (isNew)
            delete region;
        onChangeGeoLocationRegion();
    }
}

void FormCardDelivery::onDeleteGeo()
{
    if (!ui->treeViewGeo->currentIndex().isValid())
        return;

    mModelGeoLocationsRegions->beginReset();
    QModelIndexList list = ui->treeViewGeo->selectionModel()->selectedRows();
    for (int i = 0; i < list.size(); i++)
        mDelivery->geoLocationsRegions()->at(list.at(i).row())->setIsSelected(true);
    for (int i = 0; i < mDelivery->geoLocationsRegions()->size(); i++)
        if (mDelivery->geoLocationsRegions()->at(i)->getIsSelected())
            delete mDelivery->geoLocationsRegions()->takeAt(i);
    mModelGeoLocationsRegions->endReset();
}

void FormCardDelivery::onChangeCondition()
{
    ui->toolButtonEditCondition->setEnabled(
                ui->tableViewConditions->currentIndex().isValid());
    ui->toolButtonDelCondition->setEnabled(
                ui->tableViewConditions->currentIndex().isValid());
}

void FormCardDelivery::onChangeConditionRegion()
{
    ui->toolButtonAddCondtitionRegion->setEnabled(true);
    ui->toolButtonEditConditionRegion->setEnabled(ui->treeViewConditionsRegions->currentIndex().isValid());
    ui->toolButtonDelConditionRegion->setEnabled(ui->treeViewConditionsRegions->currentIndex().isValid());
}

void FormCardDelivery::onChangeGeoLocationRegion()
{
    ui->toolButtonAddGeo->setEnabled(true);
    ui->toolButtonEditGeo->setEnabled(ui->treeViewGeo->currentIndex().isValid());
    ui->toolButtonDelGeo->setEnabled(ui->treeViewGeo->currentIndex().isValid());
}

void FormCardDelivery::on_pushButtonCancel_clicked()
{
    reject();
}

void FormCardDelivery::showEvent(QShowEvent *)
{    
    initCurrencies();    
    ui->lineEditName->setFocus();    
    getData();
}

void FormCardDelivery::getData()
{    
    if (mDelivery->getId().isEmpty())
        return;

    setCursor(Qt::WaitCursor);
    QString cityFrom = mDelivery->cityFrom();
    if (ApiAdapter::getApi()->getInfoDelivery(mDelivery)) {
        mDelivery->setCityFrom(cityFrom);
        fillControls();    
    }
    getDisplayGeo();
    setCursor(Qt::ArrowCursor);
}

void FormCardDelivery::getDisplayGeo()
{ 
    if (mDelivery->conditionsRegions()->size() || mDelivery->geoLocationsRegions()->size()) {
        initRegions();
        Request request;
        bool isLoadCities = false;
        for (int i = 0; i < mDelivery->conditionsRegions()->size(); ++i)
            if (!mDelivery->conditionsRegions()->at(i)->idCityTo().isEmpty()) {
                request.addId(mDelivery->conditionsRegions()->at(i)->idCityTo());
                isLoadCities = true;
            }
        for (int i = 0; i < mDelivery->geoLocationsRegions()->size(); ++i)
            if (!mDelivery->geoLocationsRegions()->at(i)->idCityTo().isEmpty()) {
                request.addId(mDelivery->geoLocationsRegions()->at(i)->idCityTo());
                isLoadCities = true;
            }

        ListDataItems cities;
        if (isLoadCities && ApiAdapter::getApi()->getListCities(request, &cities) && cities.size()) {
            for (int i = 0; i < mDelivery->conditionsRegions()->size(); ++i) {
                for (int j = 0; j < cities.size(); ++j)
                    if (cities.at(j)->getId() == mDelivery->conditionsRegions()->at(i)->idCityTo()) {
                        mDelivery->conditionsRegions()->at(i)->setCityTo(cities.at(j)->getName());
                        mDelivery->conditionsRegions()->at(i)->setIdCountryTo(cities.at(j)->getIdMain());
                        mDelivery->conditionsRegions()->at(i)->setIdRegionTo(cities.at(j)->getIdGroup());
                        break;
                    }
            }
        }
        for (int i = 0; i < mDelivery->conditionsRegions()->size(); ++i) {
            for (int j = 0; j < mRegions->size(); ++j)
                if (mRegions->at(j)->getId() == mDelivery->conditionsRegions()->at(i)->idRegionTo()) {
                    mDelivery->conditionsRegions()->at(i)->setRegionTo(mRegions->at(j)->getName());
                    mDelivery->conditionsRegions()->at(i)->setIdCountryTo(mRegions->at(j)->getIdMain());
                    break;
                }
            for (int j = 0; j < mCountries->size(); ++j)
                if (mCountries->at(j)->getId() == mDelivery->conditionsRegions()->at(i)->idCountryTo()) {
                    mDelivery->conditionsRegions()->at(i)->setCountryTo(mCountries->at(j)->getName());                    
                    break;
                }
        }
        // регионы геолокации
        for (int i = 0; i < mDelivery->geoLocationsRegions()->size(); ++i) {
            for (int j = 0; j < cities.size(); ++j)
                if (cities.at(j)->getId() == mDelivery->geoLocationsRegions()->at(i)->idCityTo()) {
                    mDelivery->geoLocationsRegions()->at(i)->setCityTo(cities.at(j)->getName());
                    mDelivery->geoLocationsRegions()->at(i)->setIdCountryTo(cities.at(j)->getIdMain());
                    mDelivery->geoLocationsRegions()->at(i)->setIdRegionTo(cities.at(j)->getIdGroup());
                    break;
                }
            for (int j = 0; j < mRegions->size(); ++j)
                if (mRegions->at(j)->getId() == mDelivery->geoLocationsRegions()->at(i)->idRegionTo()) {
                    mDelivery->geoLocationsRegions()->at(i)->setRegionTo(mRegions->at(j)->getName());
                    mDelivery->geoLocationsRegions()->at(i)->setIdCountryTo(mRegions->at(j)->getIdMain());
                    break;
                }
            for (int j = 0; j < mCountries->size(); ++j)
                if (mCountries->at(j)->getId() == mDelivery->geoLocationsRegions()->at(i)->idCountryTo()) {
                    mDelivery->geoLocationsRegions()->at(i)->setCountryTo(mCountries->at(j)->getName());
                    break;
                }
        }
    }
}

void FormCardDelivery::setData()
{
    mDelivery->setName(ui->lineEditName->text());
    mDelivery->setPeriod(ui->lineEditPeriod->text().toInt());
    mDelivery->setPrice(ui->lineEditPrice->text().toFloat());
    mDelivery->setProperty("note", ui->plainTextEditNote->document()->toPlainText());
    mDelivery->setMaxVolume(ui->lineEditMaxVolume->text().toFloat());
    mDelivery->setMaxWeight(ui->lineEditMaxWeight->text().toFloat());
    mDelivery->setNeedAddress(ui->checkBoxNeedAddress->isChecked());
    mDelivery->setIsActive(ui->checkBoxIsActive->isChecked());
    mDelivery->setOnePosition(ui->checkBoxOnePosition->isChecked());
    QString s("0000000");
    if (ui->checkBoxMonday->isChecked())
        s[0] = '1';
    if (ui->checkBoxTuesday->isChecked())
        s[1] = '1';
    if (ui->checkBoxWednesday->isChecked())
        s[2] = '1';
    if (ui->checkBoxThursday->isChecked())
        s[3] = '1';
    if (ui->checkBoxFriday->isChecked())
        s[4] = '1';
    if (ui->checkBoxSaturday->isChecked())
        s[5] = '1';
    if (ui->checkBoxSunday->isChecked())
        s[6] = '1';
    mDelivery->setWeek(s);

    if (ui->comboBoxCurrencies->currentData().isValid())
        mDelivery->setCurrency(ui->comboBoxCurrencies->
                               currentData().toString());

    if (mIsShowedGroups) {        
        mDelivery->clearGroups();
        for (int i = 0; i < mModelGroups->idsChecked().count(); i++)
            mDelivery->addIdGroup(mModelGroups->idsChecked().at(i));
    }

    if (mIsShowedPaySystems) {        
        mDelivery->idsPaySystems()->clear();
        for (int i = 0; i < mModelPays->idsChecked().count(); i++)
            mDelivery->idsPaySystems()->push_back(mModelPays->
                                                  idsChecked().at(i));
    }

}

void FormCardDelivery::fillControls()
{
    ui->lineEditName->setText(mDelivery->getName());
    ui->lineEditPeriod->setText(QString::number(mDelivery->period()));
    ui->lineEditPrice->setText(QString::number(mDelivery->price(),'f', 2));
    ui->plainTextEditNote->setPlainText(mDelivery->property("note").toString());
    ui->lineEditMaxVolume->setText(QString::number(mDelivery->maxVolume()));
    ui->lineEditMaxWeight->setText(QString::number(mDelivery->maxWeight(), 'f',
                                                   2));
    ui->checkBoxNeedAddress->setChecked(mDelivery->needAddress());
    ui->checkBoxIsActive->setChecked(mDelivery->getIsActive());
    ui->checkBoxOnePosition->setChecked(mDelivery->onePostion());
    if (mDelivery->week().size()) {
        ui->checkBoxMonday->setChecked(mDelivery->week().at(0) == '1');
        ui->checkBoxTuesday->setChecked(mDelivery->week().at(1) == '1');
        ui->checkBoxWednesday->setChecked(mDelivery->week().at(2) == '1');
        ui->checkBoxThursday->setChecked(mDelivery->week().at(3) == '1');
        ui->checkBoxFriday->setChecked(mDelivery->week().at(4) == '1');
        ui->checkBoxSaturday->setChecked(mDelivery->week().at(5) == '1');
        ui->checkBoxSunday->setChecked(mDelivery->week().at(6) == '1');
    }

    for (int i = 0; i < ui->comboBoxCurrencies->count(); i++) {
        QString code = ui->comboBoxCurrencies->itemData(i).toString();
        if (code == mDelivery->currency()) {
            ui->comboBoxCurrencies->setCurrentIndex(i);
            break;
        }
    }

    for (int i = 0; i < ui->comboBoxType->count(); i++) {
        QString code = ui->comboBoxType->itemData(i).toString();
        if (code == mDelivery->getCode()) {
            ui->comboBoxType->setCurrentIndex(i);
            break;
        }
    }
}

void FormCardDelivery::on_pushButtonOk_clicked()
{
    if (ui->lineEditName->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, tr("Тип доставки"), tr("Укажите наименование доставки!"));
        ui->tabWidgetDelivery->setCurrentIndex(0);
        ui->lineEditName->setFocus();
        return;
    }

    ui->pushButtonCancel->setEnabled(false);
    ui->pushButtonOk->setEnabled(false);

    setData();
    if (save())
        accept();

    ui->pushButtonCancel->setEnabled(true);
    ui->pushButtonOk->setEnabled(true);
}

void FormCardDelivery::onChangeTypeDelivery()
{
    DataTypeDelivery *typeDeliv = 0;
    for (int i = 0; i < SEConfig::getTypesDeliveries()->size(); i++)
        if (ui->comboBoxType->currentData().toString() == SEConfig::getTypesDeliveries()->at(i)->getCode()) {
            typeDeliv = SEConfig::getTypesDeliveries()->at(i);
            break;
        }
    if (!typeDeliv)
        return;

    ui->tabWidgetDelivery->setTabEnabled(1, typeDeliv->isNeedConditions());
    ui->tabWidgetDelivery->setTabEnabled(4, typeDeliv->isNeedRegion());        
    mDelivery->setCode(typeDeliv->getCode());
}

