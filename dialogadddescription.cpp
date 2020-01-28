#include "dialogadddescription.h"
#include "ui_dialogadddescription.h"
#include "apiadapter.h"
#include "seconfig.h"


DialogAddDescription::DialogAddDescription(QWidget *parent, const DataProduct *product) :
    QDialog(parent),
    ui(new Ui::DialogAddDescription)
{
    ui->setupUi(this);
    mProduct = new DataProduct(product);

    initVariables();
    initSignals();
}

DialogAddDescription::~DialogAddDescription()
{
    delete ui;
}

void DialogAddDescription::showEvent(QShowEvent *)
{

}

void DialogAddDescription::onOk()
{
    if (save())
        accept();
}

void DialogAddDescription::initVariables()
{
    ui->widgetFullDescription->setSection(SEConsts::Section_Products);
}

void DialogAddDescription::initSignals()
{
    connect(ui->pushButtonCancel, SIGNAL(clicked()), SLOT(reject()));
    connect(ui->pushButtonOK, &QAbstractButton::clicked, this, &DialogAddDescription::onOk);
}

void DialogAddDescription::initSignalsChangedData()
{

}

bool DialogAddDescription::save()
{
    ui->widgetFullDescription->refreshSourceText();
    mProduct->setFullDescription(ui->widgetFullDescription->textHTML());
    mProduct->setProperty("method", "addDescription");
    return ApiAdapter::getApi()->saveProduct(mProduct);
}
