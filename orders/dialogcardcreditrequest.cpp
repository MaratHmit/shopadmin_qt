#include "dialogcardcreditrequest.h"
#include "ui_dialogcardcreditrequest.h"

#include "apiadapter.h"
#include "seconfig.h"
#include "formproducts.h"

DialogCardCreditRequest::DialogCardCreditRequest(QWidget *parent, const DataOrder *order) :
    QDialog(parent),
    ui(new Ui::DialogCardCreditRequest)
{
    ui->setupUi(this);

    if (order)
        mRequest = new DataOrder(order);
    else mRequest = new DataOrder;

    initVariables();
    fillDefaultControls();
    initSignals();
}

DialogCardCreditRequest::~DialogCardCreditRequest()
{
    delete ui;
    delete mRequest;
}

void DialogCardCreditRequest::showEvent(QShowEvent *)
{
    if (!mRequest->getId().isEmpty())
        getData();
    else ui->pushButtonSendToBank->hide();

     ui->tableViewProducts->resizeColumnsToContents();
}

void DialogCardCreditRequest::initSignals()
{
    connect(ui->toolButtonAddProduct, SIGNAL(clicked(bool)), SLOT(addProduct()));
    connect(ui->toolButtonDelProduct, SIGNAL(clicked(bool)), SLOT(delProduct()));
    connect(ui->pushButtonSendToBank, SIGNAL(clicked(bool)), SLOT(sendRequestInBank()));

    connect(ui->pushButtonOK, &QAbstractButton::clicked, this, &DialogCardCreditRequest::onOk);
}

void DialogCardCreditRequest::initVariables()
{
    mModelProducts = new NameTableModel(this, reinterpret_cast <ListDataItems *> (mRequest->listOrderItems()));
    mModelProducts->setEditableMode(true);

    mModelProducts->clearFields();
    mFieldsProducts = new QVector<Field *>();
    mFieldsProducts->append(new Field("article", "Артикул"));
    mFieldsProducts->append(new Field("name", "Наименование"));
    mFieldsProducts->append(new Field("count", "Кол-во", Qt::AlignRight | Qt::AlignVCenter));
    mFieldsProducts->append(new Field("price", "Цена", Qt::AlignRight | Qt::AlignVCenter));
    mFieldsProducts->append(new Field("discount", "Скидка", Qt::AlignRight | Qt::AlignVCenter));
    mFieldsProducts->append(new Field("cost", "Стоимость", Qt::AlignRight | Qt::AlignVCenter));
    mFieldsProducts->append(new Field("note", "Примечание"));
    mModelProducts->appendFields(mFieldsProducts);

    ui->tableViewProducts->setModel(mModelProducts);
}

void DialogCardCreditRequest::fillDefaultControls()
{

}

void DialogCardCreditRequest::getData()
{
    setCursor(Qt::WaitCursor);
    if (ApiAdapter::getApi()->getInfoCreditRequest(mRequest))
        fillControls();    
    setCursor(Qt::ArrowCursor);
}

void DialogCardCreditRequest::setData()
{
    mRequest->setCustomer(ui->lineEditCustomer->text().trimmed());
    mRequest->setCustomerPhone(ui->lineEditCustomerPhone->text().trimmed());
    mRequest->setProperty("customerINN", ui->lineEditCustomerINN->text().trimmed());
    mRequest->setNote(ui->lineEditNote->text().trimmed());
}

void DialogCardCreditRequest::calcSumTotal()
{
    double sum = 0;
    for (int i = 0; i < mRequest->listOrderItems()->size(); i++)
        sum += mRequest->listOrderItems()->at(i)->cost();
    ui->lineEditSumTotal->setText(SEConfig::priceCurrency(sum, mRequest->currency()));
    ui->lineEditSumTotal->repaint();
}

void DialogCardCreditRequest::fillControls()
{
    mModelProducts->resetModel();
    if (mRequest->getId().isEmpty())
          return;

    ui->lineEditNum->setText(mRequest->getId());
    ui->lineEditDate->setText(mRequest->dateOrder().toString("dd.MM.yyyy"));
    ui->lineEditCustomer->setText(mRequest->customer());
    ui->lineEditCustomerPhone->setText(mRequest->customerPhone());
    ui->lineEditCustomerINN->setText(mRequest->property("customerINN").toString());
    ui->lineEditStatus->setText(mRequest->property("requestStatus").toString());
    ui->lineEditNote->setText(mRequest->property("note").toString());
    ui->pushButtonSendToBank->setEnabled(!mRequest->property("isSent").toBool());
    calcSumTotal();
}

void DialogCardCreditRequest::onOk()
{
    setData();

    if (save())
      accept();
}

bool DialogCardCreditRequest::save()
{
    ui->pushButtonCancel->setEnabled(false);
    ui->pushButtonOK->setEnabled(false);
    mRequest->setIsSetAllFields(true);

    if (ApiAdapter::getApi()->saveCreditRequest(mRequest)) {
        ui->pushButtonCancel->setEnabled(true);
        ui->pushButtonOK->setEnabled(true);
        return true;
    }
    ui->pushButtonCancel->setEnabled(true);
    ui->pushButtonOK->setEnabled(true);
    return false;
}

void DialogCardCreditRequest::sendRequestInBank()
{
    QString params = mRequest->getAsJsonString();
    if (ApiAdapter::getApi()->executeAPI(SEConsts::API_CREDIT_REQUESTS,
                                         SEConsts::APIMETHOD_SEND, params, 0)) {
        QMessageBox::information(this, tr("Отправка заявки в банк"),
                                 tr("Заявка в банк успешно отправлена!"), QMessageBox::Ok);
        ui->pushButtonSendToBank->setEnabled(false);
    }
}

void DialogCardCreditRequest::on_pushButtonCancel_clicked()
{
    reject();
}

void DialogCardCreditRequest::addProduct()
{
    FormProducts form(this, true, false);
    setCursor(QCursor(Qt::WaitCursor));
    if (form.exec() == QDialog::Accepted) {
        for (int i = 0; i < form.getSelectedProducts()->size(); ++i) {
            DataProduct *product = form.getSelectedProducts()->at(i);
            product->setCurrency(QString());
            product->setMeasurement(QString());
            product->setNote(QString());
            mRequest->listOrderItems()->append(new DataOrderItem(product));
        }
        mModelProducts->resetModel();
        ui->tableViewProducts->resizeColumnsToContents();
        calcSumTotal();
    }
    setCursor(QCursor(Qt::ArrowCursor));
}

void DialogCardCreditRequest::delProduct()
{
    if (!ui->tableViewProducts->currentIndex().isValid())
        return;

    QModelIndexList list = ui->tableViewProducts->selectionModel()->selectedIndexes();
    mModelProducts->deleteItems(list);
    mModelProducts->resetModel();

    calcSumTotal();
}
