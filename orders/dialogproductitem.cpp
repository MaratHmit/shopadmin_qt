#include "dialogproductitem.h"
#include "ui_dialogproductitem.h"
#include "formproducts.h"
#include "seconfig.h"
#include "userpermission.h"

DialogProductItem::DialogProductItem(QWidget *parent, const DataOrderItem *orderItem, const bool &isServiceMode) :
    QDialog(parent),
    ui(new Ui::DialogProductItem)
{
    ui->setupUi(this);
    if (orderItem) {
        mOrderItem = new DataOrderItem(orderItem);
        mOrderItem->convertInBaseCurrency();
    } else mOrderItem = new DataOrderItem();
    mIsServiceMode = isServiceMode;

    initSignals();
    if (orderItem)
        fillControls();
    initPermissions();
}

DialogProductItem::~DialogProductItem()
{
    delete ui;
    delete mOrderItem;
}

const DataOrderItem *DialogProductItem::product() const
{
    return mOrderItem;
}

void DialogProductItem::calcSumTotal()
{
    ui->lineEditSumTotal->setText(mOrderItem->costTitle());
}

void DialogProductItem::checkEnabled()
{
    ui->pushButtonOK->setEnabled(!ui->lineEditName->text().trimmed().isEmpty() &&
                                 ui->doubleSpinBoxCount->value() > 0);
}

void DialogProductItem::fillControls(){    

    ui->lineEditName->setText(mOrderItem->getName());
    ui->doubleSpinBoxPrice->setValue(mOrderItem->price());
    ui->doubleSpinBoxCount->setValue(mOrderItem->count());
    ui->doubleSpinBoxDiscount->setValue(mOrderItem->discountPercent());
    ui->plainTextEditNote->setPlainText(mOrderItem->property("note").toString());
    ui->lineEditSumTotal->setText(mOrderItem->costTitle());
    ui->lineEditName->setCursorPosition(0);
}

void DialogProductItem::initSignals()
{
    connect(ui->pushButtonCancel, &QAbstractButton::clicked, this, &DialogProductItem::reject);
    connect(ui->pushButtonOK, &QAbstractButton::clicked, this, &DialogProductItem::accept);
    connect(ui->lineEditName, &QLineEdit::textChanged, this, &DialogProductItem::checkEnabled);
    connect(ui->lineEditName, &QLineEdit::textEdited, mOrderItem, &DataOrderItem::setName);
    connect(ui->doubleSpinBoxCount, static_cast<void (QDoubleSpinBox::*)(double)> (&QDoubleSpinBox::valueChanged),
            this, &DialogProductItem::checkEnabled);
    connect(ui->doubleSpinBoxCount, static_cast<void (QDoubleSpinBox::*)(double)> (&QDoubleSpinBox::valueChanged),
            mOrderItem, &DataOrderItem::setCount);
    connect(ui->doubleSpinBoxCount, static_cast<void (QDoubleSpinBox::*)(double)> (&QDoubleSpinBox::valueChanged),
            this, &DialogProductItem::calcSumTotal);
    connect(ui->doubleSpinBoxPrice, static_cast<void (QDoubleSpinBox::*)(double)> (&QDoubleSpinBox::valueChanged),
            mOrderItem, &DataOrderItem::setPrice);
    connect(ui->doubleSpinBoxPrice, static_cast<void (QDoubleSpinBox::*)(double)> (&QDoubleSpinBox::valueChanged),
            this, &DialogProductItem::calcSumTotal);
    connect(ui->doubleSpinBoxDiscount, static_cast<void (QDoubleSpinBox::*)(double)> (&QDoubleSpinBox::valueChanged),
            mOrderItem, &DataOrderItem::setDiscountPercent);
    connect(ui->doubleSpinBoxDiscount, static_cast<void (QDoubleSpinBox::*)(double)> (&QDoubleSpinBox::valueChanged),
            this, &DialogProductItem::calcSumTotal);    
    connect(ui->plainTextEditNote, &QPlainTextEdit::textChanged, this, &DialogProductItem::setNote);    
}

void DialogProductItem::selectProduct()
{
    FormProducts form(this, false);
    setCursor(QCursor(Qt::WaitCursor));
    if (form.exec() == QDialog::Accepted) {
        const DataProduct *product = form.getSelectedProducts()->at(0);
        mOrderItem->copy(product);
        mOrderItem->convertInBaseCurrency();
        fillControls();
    }
    setCursor(QCursor(Qt::ArrowCursor));
}

void DialogProductItem::setNote()
{
    mOrderItem->setNote(ui->plainTextEditNote->toPlainText().trimmed());
}

void DialogProductItem::initPermissions()
{

}
