#include "dialogincprices.h"
#include "ui_dialogincprices.h"

#include "apiadapter.h"
#include "seconfig.h"
#include "seconsts.h"

DialogIncPrices::DialogIncPrices(QWidget *parent, DataProductsGroup *item) :
    QDialog(parent),
    ui(new Ui::DialogIncPrices)
{
    ui->setupUi(this);

    mGroup = item;
    ui->checkBoxSetDescendants->setChecked(true);
    connect(this, SIGNAL(accepted()), SLOT(save()));
    connect(ui->checkBoxPrice, SIGNAL(clicked(bool)), SLOT(onChangePrice(bool)));
    connect(ui->checkBoxPriceCorp, SIGNAL(clicked(bool)), SLOT(onChangePrice(bool)));
    connect(ui->checkBoxPriceOpt, SIGNAL(clicked(bool)), SLOT(onChangePrice(bool)));
}

DialogIncPrices::~DialogIncPrices()
{
    delete ui;
}

void DialogIncPrices::showEvent(QShowEvent *)
{
    getData();
}

void DialogIncPrices::getData()
{
    QStringList ids = mGroup->getIds();
    ApiAdapter::getApi()->getInfoProductGroup(mGroup);
    foreach (QString id, ids)
        mGroup->addId(id);

    ui->spinBoxIncPrice->setValue(mGroup->property("incPrice").toInt());
    ui->spinBoxIncPriceOpt->setValue(mGroup->property("incPriceOpt").toInt());
    ui->spinBoxIncPriceCorp->setValue(mGroup->property("incPriceCorp").toInt());

    if (mGroup->property("sourcePrice").toString() == "OFFICE_ZAKAZ")
        ui->comboBoxSourcePrice->setCurrentIndex(1);
}

void DialogIncPrices::save()
{
    mGroup->setProperty("isSetDescendants", ui->checkBoxSetDescendants->isChecked());
    if (!ui->comboBoxSourcePrice->currentIndex())
        mGroup->setProperty("sourcePrice", "FILE");
    else mGroup->setProperty("sourcePrice", "OFFICE_ZAKAZ");
    if (ui->checkBoxPrice->isChecked())
        mGroup->setProperty("incPrice", ui->spinBoxIncPrice->value());
    if (ui->checkBoxPriceCorp->isChecked())
        mGroup->setProperty("incPriceCorp", ui->spinBoxIncPriceCorp->value());
    if (ui->checkBoxPriceOpt->isChecked())
        mGroup->setProperty("incPriceOpt", ui->spinBoxIncPriceOpt->value());
    ApiAdapter::getApi()->saveProductGroup(mGroup);
}

void DialogIncPrices::onChangePrice(bool checked)
{
    if (sender() == ui->checkBoxPrice)
        ui->spinBoxIncPrice->setEnabled(checked);
    if (sender() == ui->checkBoxPriceCorp)
        ui->spinBoxIncPriceCorp->setEnabled(checked);
    if (sender() == ui->checkBoxPriceOpt)
        ui->spinBoxIncPriceOpt->setEnabled(checked);
}
