#include "dialogcardpreorder.h"
#include "ui_dialogcardpreorder.h"

#include "formproducts.h"

DialogCardPreorder::DialogCardPreorder(QWidget *parent, const DataOrder *order) :
    QDialog(parent),
    ui(new Ui::DialogCardPreorder)
{
    ui->setupUi(this);

    if (order)
        mOrder = new DataOrder(order);
    else mOrder = new DataOrder;

    if (!mOrder->getId().isEmpty()) {
        ui->lineEditCustomer->setText(mOrder->customer());
        ui->lineEditCustomerEmail->setText(mOrder->customerEmail());
        ui->lineEditCustomerPhone->setText(mOrder->customerPhone());
        ui->lineEditProduct->setText(mOrder->productName());
        ui->doubleSpinBox->setValue(mOrder->property("count").toDouble());
    }

    if (!mOrder->property("isSent").toBool()) {
        ui->lineEditStatus->setText("Не отправлено");
        QPalette palette;
        palette.setBrush(QPalette::Base, Qt::red);
        ui->lineEditStatus->setPalette(palette);
    }
}

DialogCardPreorder::~DialogCardPreorder()
{
    delete ui;
    delete mOrder;
}

void DialogCardPreorder::on_pushButtonCancel_clicked()
{
    reject();
}

void DialogCardPreorder::on_toolButtonCustomer_clicked()
{
    FormProducts form(this, false, false);
    setCursor(QCursor(Qt::WaitCursor));
    if (form.exec() == QDialog::Accepted) {
        for (int i = 0; i < form.getSelectedProducts()->size(); ++i) {
            DataProduct *product = form.getSelectedProducts()->at(i);
            mOrder->setIdProduct(product->getId());
            ui->lineEditProduct->setText(product->getName());
            break;
        }
    }
    setCursor(QCursor(Qt::ArrowCursor));
}

bool DialogCardPreorder::save()
{
    ui->pushButtonCancel->setEnabled(false);
    ui->pushButtonOK->setEnabled(false);
    mOrder->setIsSetAllFields(true);

    if (ApiAdapter::getApi()->savePreorder(mOrder)) {
        ui->pushButtonCancel->setEnabled(true);
        ui->pushButtonOK->setEnabled(true);
        return true;
    }
    ui->pushButtonCancel->setEnabled(true);
    ui->pushButtonOK->setEnabled(true);
    return false;
}

void DialogCardPreorder::calcSumTotal()
{

}

void DialogCardPreorder::getData()
{

}

void DialogCardPreorder::on_pushButtonOK_clicked()
{
    setData();
    if (save())
        accept();
}

void DialogCardPreorder::setData()
{
    mOrder->setCustomer(ui->lineEditCustomer->text().trimmed());
    mOrder->setCustomerEmail(ui->lineEditCustomerEmail->text().trimmed());
    mOrder->setCustomerPhone(ui->lineEditCustomerPhone->text().trimmed());
    mOrder->setProperty("count", ui->doubleSpinBox->value());
}

void DialogCardPreorder::on_toolButtonCustomerInfo_clicked()
{
    if (mOrder->idProduct().isEmpty())
        return;

    FormProductCard card;
    card.setId(mOrder->idProduct());
    card.exec();
}
