#include "dialogsetrates.h"
#include "ui_dialogsetrates.h"
#include "apiadapter.h"
#include "seconfig.h"

DialogSetRates::DialogSetRates(QWidget *parent, const DataProduct *product) :
    QDialog(parent),
    ui(new Ui::DialogSetRates)
{
    ui->setupUi(this);
    mProduct = new DataProduct(product);

    initVariables();
    initSignals();
}

DialogSetRates::~DialogSetRates()
{
    delete ui;
    delete mProduct;
}

void DialogSetRates::showEvent(QShowEvent *)
{
    initSignalsChangedData();
}

void DialogSetRates::onOk()
{
    if (save())
        accept();
}

void DialogSetRates::initVariables()
{

}

void DialogSetRates::initSignals()
{
    connect(ui->pushButtonCancel, SIGNAL(clicked()), SLOT(reject()));
    connect(ui->pushButtonOK, &QAbstractButton::clicked, this, &DialogSetRates::onOk);
}

void DialogSetRates::initSignalsChangedData()
{
    connect(ui->doubleSpinBoxRate, SIGNAL(valueChanged(double)), mProduct, SLOT(setRate(qreal)));
}

bool DialogSetRates::save()
{
    return ApiAdapter::getApi()->saveProduct(mProduct);
}
