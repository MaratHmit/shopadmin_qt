#include "dialogdelivregion.h"
#include "ui_dialogdelivregion.h"
#include "apiadapter.h"
#include "seconfig.h"

#include <QSettings>

DialogDelivRegion::DialogDelivRegion(QWidget *parent, DataDelivery *region) :
    QDialog(parent),
    ui(new Ui::DialogDelivRegion)
{
    ui->setupUi(this);

    mRegion = region;
    initVariables();
    initSignals();
}

DialogDelivRegion::~DialogDelivRegion()
{
    delete ui;
    mCities->deleteItems();
    delete mCities;
}

void DialogDelivRegion::setExistRegions(const ListDeliveries *regions)
{
    mExistRegions = regions;
}

void DialogDelivRegion::setGeoMode()
{
    ui->groupBox->hide();
    ui->checkBoxIsActive->hide();
    ui->labelPeriod->hide();
    ui->spinBoxPeriod->hide();
    ui->labelPeriodHint->hide();
    setMaximumHeight(150);
    setMaximumWidth(300);
}

void DialogDelivRegion::onChangeCountry()
{
    if (ui->comboBoxCountries->currentIndex() < 0)
        return;

    disconnect(ui->comboBoxRegions, SIGNAL(currentIndexChanged(int)));
    ui->comboBoxRegions->clear();
    ui->comboBoxCities->clear();
    mCities->deleteItems();
    ui->comboBoxRegions->setEnabled(false);
    ui->comboBoxCities->setEnabled(false);
    QString idCountry = ui->comboBoxCountries->currentData().toString();
    storeCountry(idCountry);
    int index = -1;
    ui->comboBoxRegions->setCurrentIndex(-1);
    ui->comboBoxCities->setCurrentIndex(-1);
    for (int i = 0; i < mRegions->size(); ++i) {
        if (mRegions->at(i)->getIdMain() == idCountry) {
            ui->comboBoxRegions->addItem(mRegions->at(i)->getName(), mRegions->at(i)->getId());
            if (mRegions->at(i)->getId() == mIdRegionDefault)
                index = ui->comboBoxRegions->count() - 1;
        }
    }
    if (ui->comboBoxRegions->count()) {
        ui->comboBoxRegions->setCurrentIndex(index);
        ui->comboBoxRegions->setEnabled(true);
        if (index + 1)
            onChangeRegion();
    } else
        onChangeRegion();
    connect(ui->comboBoxRegions, SIGNAL(currentIndexChanged(int)), SLOT(onChangeRegion()));
}

void DialogDelivRegion::onChangeRegion()
{
    if (ui->comboBoxRegions->count()) {
        if (ui->comboBoxRegions->currentIndex() < 0)
            return;

        storeRegion(ui->comboBoxRegions->currentData().toString());
        loadCities(ui->comboBoxCountries->currentData().toString(),
                    ui->comboBoxRegions->currentData().toString());
    } else loadCities(ui->comboBoxCountries->currentData().toString());
}

void DialogDelivRegion::showEvent(QShowEvent *)
{
    setCursor(Qt::WaitCursor);
    mCountries = SEConfig::getCountries();
    mRegions = SEConfig::getRegions();
    restoreIdsCountryRegionCity();
    fillControls();    
    setCursor(Qt::ArrowCursor);
}

void DialogDelivRegion::onOk()
{
    if (ui->comboBoxCountries->currentIndex() < 0) {
        QMessageBox::warning(this, tr("Страна доставки"), tr("Укажите страну доставки!"));
        ui->comboBoxCountries->setFocus();
        return;
    }

    mRegion->setIdCountryTo(ui->comboBoxCountries->currentData().toString());
    mRegion->setCountryTo(ui->comboBoxCountries->currentText());
    if (!ui->comboBoxRegions->currentText().trimmed().isEmpty() && (ui->comboBoxRegions->currentIndex() + 1)) {
        mRegion->setIdRegionTo(ui->comboBoxRegions->currentData().toString());
        mRegion->setRegionTo(ui->comboBoxRegions->currentText());
    } else {
        mRegion->setIdRegionTo(QString());
        mRegion->setRegionTo(QString());
    }
    if (!ui->comboBoxCities->currentText().trimmed().isEmpty() && (ui->comboBoxCities->currentIndex() + 1)) {
        mRegion->setIdCityTo(ui->comboBoxCities->currentData().toString());
        mRegion->setCityTo(ui->comboBoxCities->currentText());
        storeCity(ui->comboBoxCities->currentData().toString());
    } else {
        mRegion->setIdCityTo(QString());
        mRegion->setCityTo(QString());
        storeCity(QString());
    }

    mRegion->setPeriod(ui->spinBoxPeriod->value());
    mRegion->setPrice(ui->doubleSpinBoxPrice->value());
    mRegion->setMaxVolume(ui->doubleSpinBoxVolume->value());
    mRegion->setMaxWeight(ui->doubleSpinBoxWeight->value());
    mRegion->setNote(ui->textEdit->toPlainText().trimmed());
    mRegion->setIsActive(ui->checkBoxIsActive->isChecked());

    accept();
}

void DialogDelivRegion::loadCities(const QString &idCountry, const QString &idRegion)
{
    ui->comboBoxCities->clear();
    ui->comboBoxCities->setEnabled(false);
    mCities->deleteItems();
    mRequestCity.setIdCountry(idCountry);
    if (!idRegion.isEmpty())
        mRequestCity.addStringValue("idRegion", idRegion);
    setCursor(QCursor(Qt::WaitCursor));
    if (ApiAdapter::getApi()->getListCities(mRequestCity, mCities)) {
        int index = -1;
        for (int i = 0; i < mCities->size(); i++) {
            ui->comboBoxCities->addItem(mCities->at(i)->getName(), mCities->at(i)->getId());
            if (mCities->at(i)->getId() == mIdCityDefault)
                index = i;
        }
        ui->comboBoxCities->setCurrentIndex(index);
    }
    ui->comboBoxCities->setEnabled(mCities->size());
    setCursor(QCursor(Qt::ArrowCursor));
}

void DialogDelivRegion::initSignals()
{
    connect(ui->pushButtonCancel, &QAbstractButton::clicked, this, &QDialog::reject);
    connect(ui->pushButtonOK, &QAbstractButton::clicked, this, &DialogDelivRegion::onOk);    
}

void DialogDelivRegion::initVariables()
{
    mCities = new ListDataItems;
}

void DialogDelivRegion::fillControls()
{
    if (!mRegion->idCountryTo().isEmpty())
        mIdCountryDefault = mRegion->idCountryTo();
    if (!mRegion->idRegionTo().isEmpty())
        mIdRegionDefault = mRegion->idRegionTo();
    if (!mRegion->idCityTo().isEmpty())
        mIdCityDefault = mRegion->idCityTo();

    int index = -1;
    for (int i = 0; i < mCountries->size(); i++) {
        ui->comboBoxCountries->addItem(mCountries->at(i)->getName(), mCountries->at(i)->getId());
        if (mCountries->at(i)->getId() == mIdCountryDefault)
            index = i;
    }
    ui->comboBoxCountries->setCurrentIndex(index);
    connect(ui->comboBoxCountries, SIGNAL(currentIndexChanged(int)), SLOT(onChangeCountry()));
    if (index + 1)
        onChangeCountry();

    ui->spinBoxPeriod->setValue(mRegion->period());
    ui->doubleSpinBoxPrice->setValue(mRegion->price());
    ui->doubleSpinBoxVolume->setValue(mRegion->maxVolume());
    ui->doubleSpinBoxWeight->setValue(mRegion->maxWeight());
    ui->textEdit->document()->setPlainText(mRegion->property("note").toString());
    ui->checkBoxIsActive->setChecked(mRegion->getIsActive());
}

void DialogDelivRegion::restoreIdsCountryRegionCity()
{
    QSettings settings;
    mIdCountryDefault = settings.value("idCountry", "20").toString();
    mIdRegionDefault = settings.value("idRegion").toString();
    mIdCityDefault = settings.value("idCity").toString();
}

void DialogDelivRegion::storeCountry(const QString &idCountry)
{
    QSettings *settings = new QSettings();
    mIdCountryDefault = idCountry;
    settings->setValue("idCountry", idCountry);
    settings->sync();
    delete settings;
}

void DialogDelivRegion::storeRegion(const QString &idRegion)
{
    QSettings *settings = new QSettings();
    mIdRegionDefault = idRegion;
    settings->setValue("idRegion", idRegion);
    settings->sync();
    delete settings;
}

void DialogDelivRegion::storeCity(const QString &idCity)
{
    QSettings *settings = new QSettings();
    mIdCityDefault = idCity;
    settings->setValue("idCity", idCity);
    settings->sync();
    delete settings;
}
