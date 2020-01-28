#include "dialogpricetype.h"
#include "ui_dialogpricetype.h"
#include "apiadapter.h"

DialogPriceType::DialogPriceType(QWidget *parent, DataContact *contact) :
    QDialog(parent),
    ui(new Ui::DialogPriceType)
{
    ui->setupUi(this);
    this->contact = contact;
    if (contact->priceType() < ui->comboBoxPriceType->count())
        ui->comboBoxPriceType->setCurrentIndex(contact->priceType());
}

DialogPriceType::~DialogPriceType()
{
    delete ui;
}

void DialogPriceType::on_buttonBox_accepted()
{
    contact->setPriceType(ui->comboBoxPriceType->currentIndex());
    ApiAdapter::getApi()->saveContact(contact);
}
