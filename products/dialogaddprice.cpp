#include "dialogaddprice.h"
#include "ui_dialogaddprice.h"
#include "apiadapter.h"

DialogAddPrice::DialogAddPrice(QWidget *parent, DataProduct *product) :
    QDialog(parent),
    ui(new Ui::DialogAddPrice)
{
    ui->setupUi(this);
    mProduct = product;
    connect(ui->pushButtonCancel, &QAbstractButton::clicked, this, &QDialog::reject);
    connect(ui->pushButtonOK, &QAbstractButton::clicked, this, &DialogAddPrice::save);
    connect(ui->comboBoxType, &QComboBox::currentTextChanged, this, &DialogAddPrice::onChangeType);
}

DialogAddPrice::~DialogAddPrice()
{
    delete ui;
}

void DialogAddPrice::onChangeType()
{
    if (ui->comboBoxType->currentIndex() == 0)
        ui->doubleSpinBoxValue->setSuffix("%");
    else ui->doubleSpinBoxValue->setSuffix(QString());
}

void DialogAddPrice::save()
{
    if (ui->comboBoxType->currentIndex() == 0)
        mProduct->setValue("p");
    else mProduct->setValue("a");
    mProduct->setPrice(ui->doubleSpinBoxValue->value());

    if (ApiAdapter::getApi()->addProductPrice(mProduct))
        accept();
}
