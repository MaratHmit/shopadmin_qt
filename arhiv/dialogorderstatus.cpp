#include "dialogorderstatus.h"
#include "ui_dialogorderstatus.h"
#include "seconfig.h"

DialogOrderStatus::DialogOrderStatus(QWidget *parent, DataOrder *order) :
    QDialog(parent),
    ui(new Ui::DialogOrderStatus)
{
    ui->setupUi(this);
    mOrder = order;

    initVariables();
    initSignals();
}

DialogOrderStatus::~DialogOrderStatus()
{
    delete ui;
}

void DialogOrderStatus::onChangeStatus()
{
    if (ui->comboBoxOrderStatus->currentData().toString() == "Y") {

    }
}

void DialogOrderStatus::initVariables()
{
    int index = -1;
    for (int i = 0; i < SEConfig::dataMain()->listCurrency()->size(); ++i) {
        ui->comboBoxCurrencies->addItem(SEConfig::dataMain()->listCurrency()->at(i)->name(),
                                        SEConfig::dataMain()->listCurrency()->at(i)->code());
        if (mOrder->transactCurrency().isEmpty() &&
                SEConfig::dataMain()->listCurrency()->at(i)->code() == SEConfig::dataMain()->baseCurrency())
            index = i;
        if (!mOrder->transactCurrency().isEmpty() &&
                SEConfig::dataMain()->listCurrency()->at(i)->code() == mOrder->transactCurrency())
            index = i;
    }
    ui->comboBoxCurrencies->setCurrentIndex(index);

    index = -1;
    for (int i = 0; i < SEConfig::ordersStatuses()->size(); ++i) {
        ui->comboBoxOrderStatus->addItem(SEConfig::ordersStatuses()->at(i)->name(),
                                         SEConfig::ordersStatuses()->at(i)->id());
        if (SEConfig::ordersStatuses()->at(i)->id() == mOrder->statusOrder())
            index = i;
    }
    ui->comboBoxOrderStatus->setCurrentIndex(index);

    ui->dateEditPaymentDate->hide();
    ui->labelPaymentDate->hide();
    ui->labelPaymentType->hide();
    ui->comboBoxPaymentType->hide();

    ui->lineEditDocNum->setText(mOrder->transactId());
    ui->plainTextEditDocName->setPlainText(mOrder->payeeDoc());
    ui->lineEditOrderSum->setText(mOrder->sumTitle());
    ui->doubleSpinBoxOrderPay->setValue(mOrder->transactSum());
    ui->dateEditPaymentDate->setDate(mOrder->paymentDate());

    onChangeStatus();

    ui->doubleSpinBoxOrderPay->setFocus();
}

void DialogOrderStatus::initSignals()
{
    connect(ui->pushButtonCancel, &QAbstractButton::clicked, this, &DialogOrderStatus::reject);
    connect(ui->pushButtonOK, &QAbstractButton::clicked, this, &DialogOrderStatus::accept);
}
