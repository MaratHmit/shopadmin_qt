#include "dialogsetprices.h"
#include "ui_dialogsetprices.h"
#include "datacurrency.h"
#include "apiadapter.h"
#include "seconfig.h"

DialogSetPrices::DialogSetPrices(QWidget *parent, const DataProduct *product) :
    QDialog(parent),
    ui(new Ui::DialogSetPrices)
{
    ui->setupUi(this);
    mProduct = new DataProduct(product);

    initVariables();
    initSignals();
}

DialogSetPrices::~DialogSetPrices()
{
    delete ui;
    delete mProduct;
}

void DialogSetPrices::onChangeCurrency()
{
    mProduct->setCurrency(ui->comboBoxCurrencies->currentData().toString());
}

void DialogSetPrices::showEvent(QShowEvent *)
{
    initSignalsChangedData();
}

void DialogSetPrices::onOk()
{
    if (save())
        accept();
}

void DialogSetPrices::initVariables()
{    
    ListCurrencies *pCurrencies = new ListCurrencies();
    if (ApiAdapter::getApi()->getListCurrencies(pCurrencies)) {
        ui->comboBoxCurrencies->clear();
        for (int i = 0; i < pCurrencies->size(); i++) {
            ui->comboBoxCurrencies->addItem(pCurrencies->at(i)->getName(),
                                            pCurrencies->at(i)->getCode());
            if (pCurrencies->at(i)->getCode() == SEConfig::getMainInfo()->baseCurrency())
                ui->comboBoxCurrencies->setCurrentIndex(i);
        }
    }

    ui->doubleSpinBoxPrice->setValue(mProduct->price());
    ui->doubleSpinPurchasePirce->setValue(mProduct->pricePurchase());
    ui->doubleSpinMiniWholesalePrice->setValue(mProduct->priceMiniWholesale());
    ui->doubleSpinWholesalePrice->setValue(mProduct->priceWholesale());
    for (int i = 0; i < ui->comboBoxCurrencies->count(); ++i)
        if (ui->comboBoxCurrencies->itemData(i).toString() == mProduct->getCurrency()) {
            ui->comboBoxCurrencies->setCurrentIndex(i);
            break;
        }
}

void DialogSetPrices::initSignals()
{
    connect(ui->pushButtonCancel, SIGNAL(clicked()), SLOT(reject()));
    connect(ui->pushButtonOK, &QAbstractButton::clicked, this, &DialogSetPrices::onOk);
}

void DialogSetPrices::initSignalsChangedData()
{
    connect(ui->comboBoxCurrencies, SIGNAL(currentIndexChanged(int)), SLOT(onChangeCurrency()));
    connect(ui->doubleSpinBoxPrice, SIGNAL(valueChanged(double)), mProduct, SLOT(setPrice(qreal)));
    connect(ui->doubleSpinMiniWholesalePrice, SIGNAL(valueChanged(double)), mProduct, SLOT(setPriceMiniWholesale(qreal)));
    connect(ui->doubleSpinWholesalePrice, SIGNAL(valueChanged(double)), mProduct, SLOT(setPriceWholesale(qreal)));
    connect(ui->doubleSpinPurchasePirce, SIGNAL(valueChanged(double)), mProduct, SLOT(setPricePurchase(qreal)));
}

bool DialogSetPrices::save()
{
    return ApiAdapter::getApi()->saveProduct(mProduct);
}
