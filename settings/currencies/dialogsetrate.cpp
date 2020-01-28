#include "dialogsetrate.h"
#include "ui_dialogsetrate.h"
#include "apiadapter.h"

DialogSetRate::DialogSetRate(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogSetRate)
{
    ui->setupUi(this);
    initVariables();
    initSignals();
}

DialogSetRate::~DialogSetRate()
{
    delete ui;
    delete mCurrency;
    delete mBaseCurrency;
}

void DialogSetRate::setCurrency(const DataCurrency *currency, const DataCurrency *baseCurrency)
{
    mCurrency->copy(currency);
    mBaseCurrency->copy(baseCurrency);
}

void DialogSetRate::showEvent(QShowEvent *)
{
    fillControls();
    connectChangeSignals();
}

void DialogSetRate::onOK()
{
    mCurrency->setIsSetAllFields(true);
    if (ApiAdapter::getApi()->saveCurrencyRate(mCurrency))
        accept();
}

void DialogSetRate::onChangeData()
{
    ui->pushButtonOK->setEnabled(true);
}

void DialogSetRate::onSetRateFromCBR()
{
    setCursor(Qt::WaitCursor);
    DataCurrency *currencyCBR = new DataCurrency(mCurrency);
    if (ApiAdapter::getApi()->getInfoCurrencyRate(currencyCBR))
        ui->doubleSpinBox->setValue(currencyCBR->rate());

    delete currencyCBR;
    setCursor(Qt::ArrowCursor);
}

void DialogSetRate::fillControls()
{
    ui->labelName->setText(mCurrency->getName());
    ui->doubleSpinBox->setValue(mCurrency->rate());
    ui->labelNameBaseCurrency->setText(mBaseCurrency->getName());
}

void DialogSetRate::initSignals()
{
    connect(ui->pushButtonCancel, &QPushButton::clicked, this, &DialogSetRate::reject);
    connect(ui->pushButtonOK, &QPushButton::clicked, this, &DialogSetRate::onOK);
    connect(ui->pushButtonSetRateCBR, &QPushButton::clicked, this, &DialogSetRate::onSetRateFromCBR);
}

void DialogSetRate::initVariables()
{
    mCurrency = new DataCurrency();
    mBaseCurrency = new DataCurrency();
}

void DialogSetRate::connectChangeSignals()
{
    connect(mCurrency, &DataCurrency::modified, this, &DialogSetRate::onChangeData);    
    connect(ui->doubleSpinBox, static_cast<void (QDoubleSpinBox::*)(double)> (&QDoubleSpinBox::valueChanged), mCurrency,
            &DataCurrency::setRate);
}
