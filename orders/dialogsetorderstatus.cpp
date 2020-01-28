#include "dialogsetorderstatus.h"
#include "ui_dialogsetorderstatus.h"
#include "apiadapter.h"
#include "seconsts.h"
#include "seconfig.h"

DialogSetOrderStatus::DialogSetOrderStatus(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogSetOrderStatus)
{
    ui->setupUi(this);
    initVariables();
}

DialogSetOrderStatus::~DialogSetOrderStatus()
{
    delete ui;
    delete mStatuses;
}

void DialogSetOrderStatus::setTypeStatus(DialogSetOrderStatus::TYPE_STATUS typeStatus)
{
    mTypeStatus = typeStatus;
    if (mTypeStatus == DELIVERY_STATUS)
        setWindowTitle(tr("Статус доставки"));
}

void DialogSetOrderStatus::setOrder(DataOrder *order)
{
    mOrder = order;
}

void DialogSetOrderStatus::showEvent(QShowEvent *)
{
    getData();
    initSignals();
}

void DialogSetOrderStatus::initVariables()
{
    mTypeStatus = ORDER_STATUS;
    mStatuses = new ListDataItems;
}

void DialogSetOrderStatus::initSignals()
{
    connect(ui->pushButtonCancel, SIGNAL(clicked()), SLOT(reject()));
    connect(ui->pushButtonOK, &QAbstractButton::clicked, this, &DialogSetOrderStatus::save);
}

void DialogSetOrderStatus::save()
{
    if (ui->comboBoxStatuses->currentIndex() < 0)
        reject();

    if (!mTypeStatus)
        mOrder->setStatusOrder(ui->comboBoxStatuses->currentData().toString());
    else mOrder->setStatusDelivery(ui->comboBoxStatuses->currentData().toString());

    if (ApiAdapter::getApi()->saveOrder(mOrder))
        accept();
}

void DialogSetOrderStatus::getData()
{
    setCursor(Qt::WaitCursor);
    if (!mTypeStatus)
        ApiAdapter::getApi()->getListOrdersStatuses(mStatuses);
    else ApiAdapter::getApi()->getListDeliveriesStatuses(mStatuses);
    setCursor(Qt::ArrowCursor);

    for (int i = 0; i < mStatuses->size(); ++i)
        ui->comboBoxStatuses->addItem(mStatuses->at(i)->getName(), mStatuses->at(i)->getId());
}
