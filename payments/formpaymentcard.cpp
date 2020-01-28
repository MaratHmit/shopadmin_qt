#include "formpaymentcard.h"
#include "ui_formpaymentcard.h"
#include "apiadapter.h"
#include "formorders.h"
#include "formcontacts.h"
#include "seconfig.h"

FormPaymentCard::FormPaymentCard(QWidget *parent, const DataPayment *payment) :
    QDialog(parent),
    ui(new Ui::FormPaymentCard)
{
    ui->setupUi(this);
    mPayment = new DataPayment(payment);

    initVariables();
    initSignals();

}

FormPaymentCard::FormPaymentCard(QWidget *parent, const DataOrder *order) :
    QDialog(parent),
    ui(new Ui::FormPaymentCard)
{
    ui->setupUi(this);
    mPayment = new DataPayment();

    initVariables();
    initSignals();
    mOrder->copy(order);
    mIsOrderMode = true;
}

FormPaymentCard::~FormPaymentCard()
{
    delete ui;
    delete mPayment;
    delete mOrder;
    delete mContact;
    delete mPaySystems;
    delete mPreloader;
    delete mListCurrencies;
}

const DataPayment *FormPaymentCard::payment() const
{
    return mPayment;
}

void FormPaymentCard::showEvent(QShowEvent *)
{
    initCurrencies();
    fillDefaultControls();
    fillControls();
}

void FormPaymentCard::initVariables()
{
    mIsOrderMode = false;
    mPreloader = new Preloader(this);
    mOrder = new DataOrder;
    mContact = new DataContact;
    mPaySystems = new ListDataPaySystems;
    mListCurrencies = new ListCurrencies;
    ui->labelAccountSum->hide();
    ui->lineEditAccountSum->hide();    
    ui->labelDocNum->hide();
    ui->lineEditDocNum->hide();        
    ui->labelCurrency->hide();
    ui->comboBoxCurrencies->hide();
}

void FormPaymentCard::initSignals()
{
    connect(ui->pushButtonCancel, &QAbstractButton::clicked, this, &FormPaymentCard::reject);
    connect(ui->pushButtonOK, &QAbstractButton::clicked, this, &FormPaymentCard::onOk);
    connect(ui->comboBoxPaymentType,  static_cast<void (QComboBox::*)(int)> (&QComboBox::currentIndexChanged),
            this, &FormPaymentCard::onChangeTypePayment);
    connect(ui->comboBoxPaymentTarget,  static_cast<void (QComboBox::*)(int)> (&QComboBox::currentIndexChanged),
            this, &FormPaymentCard::onChangePaymentTarget);
    connect(ui->comboBoxCurrencies,  static_cast<void (QComboBox::*)(int)> (&QComboBox::currentIndexChanged),
            this, &FormPaymentCard::onChangeCurrency);
    connect(ui->toolButtonSelectOrder, &QAbstractButton::clicked, this, &FormPaymentCard::selectOrder);
    connect(ui->toolButtonSelectPayer, &QAbstractButton::clicked, this, &FormPaymentCard::selectPayer);
    connect(ui->doubleSpinBoxPaymentSum, static_cast<void (QDoubleSpinBox::*)(double)> (&QDoubleSpinBox::valueChanged),
            mPayment, &DataPayment::setAmount);
    connect(ui->dateEditDocDate, &QDateEdit::dateChanged, mPayment, &DataPayment::setDocDate);
    connect(ui->plainTextEdit, &QPlainTextEdit::textChanged, this, &FormPaymentCard::onChangeNote);    
}

void FormPaymentCard::fillControls()
{
    if (mPayment->getId().isEmpty())
        return;

    setCursor(Qt::WaitCursor);
    mPreloader->startAnimation();
    if (ApiAdapter::getApi()->getInfoPayment(mPayment)) {
        if (!mIsOrderMode && !mPayment->idOrder().isEmpty()) {
            mOrder->setId(mPayment->idOrder());
            ApiAdapter::getApi()->getInfoOrder(mOrder);
        }
        mContact->setId(mPayment->idPayer());
        ApiAdapter::getApi()->getInfoContact(mContact);
    }

    ui->labelDocNum->show();
    ui->lineEditDocNum->show();
    if (!mPayment->idOrder().isEmpty())
        ui->lineEditOrder->setText("Заказ № " + mPayment->idOrder() + " от " +
                                   mOrder->dateOrder().toString("dd.MM.yyyy"));
    ui->lineEditCustomer->setText(mOrder->customer());
    ui->lineEditOrderAmount->setText(mOrder->sumTitle());
    ui->lineEditPaidAmount->setText(mOrder->paidSumTitle());
    ui->lineEditSurcharge->setText(mOrder->surchargeSumTitle());
    ui->lineEditPayer->setText(mContact->displayName());
    ui->lineEditAccountSum->setText(mContact->balanceTitle());
    ui->lineEditDocNum->setText(mPayment->docNum());
    ui->dateEditDocDate->setDate(mPayment->docDate());
    ui->plainTextEdit->setPlainText(mPayment->property("note").toString());
    ui->doubleSpinBoxPaymentSum->setValue(mPayment->amount());

    for (int i = 0; i < ui->comboBoxPaymentType->count(); ++i)
        if (ui->comboBoxPaymentType->itemData(i).toString() == mPayment->idPaymentType()) {
            ui->comboBoxPaymentType->setCurrentIndex(i);
            break;
        }

    ui->comboBoxPaymentTarget->setCurrentIndex(mPayment->paymentTarget());

    mPreloader->stopAnimation();
    setCursor(Qt::ArrowCursor);
}

void FormPaymentCard::fillDefaultControls()
{
    if (mIsOrderMode) {
        ui->widgetPaymentTarget->hide();
        ui->groupBoxOrderInfo->hide();
        mPayment->setIdOrder(mOrder->getId());
        mPayment->setOrderAmount(mOrder->sum());
        if (!mOrder->idCustomer().isEmpty()) {
            mContact->setId(mOrder->idCustomer());
            mPayment->setIdPayer(mContact->getId());
            ApiAdapter::getApi()->getInfoContact(mContact);
        }
        ui->lineEditPayer->setText(mContact->displayName());
        ui->lineEditAccountSum->setText(mContact->balanceTitle());
        ui->lineEditOrderAmount->setText(mOrder->sumTitle());
        ui->lineEditPaidAmount->setText(mOrder->paidSumTitle());
        ui->lineEditSurcharge->setText(mOrder->surchargeSumTitle());
        if (mPayment->getId().isEmpty() &&
                ((mPayment->idPaymentType() == "0" && mOrder->surchargeSum() <= mContact->balance()) ||
                 mPayment->idPaymentType() != "0"))
            ui->doubleSpinBoxPaymentSum->setValue(mOrder->surchargeSum());
        setCurrencyByOrder();
        setMaximumHeight(300);
    }
    ui->doubleSpinBoxPaymentSum->setFocus();
    mDefaultHeight = geometry().height();
    ui->dateEditDocDate->setDate(QDate::currentDate());
    fillPaySystems();
}

void FormPaymentCard::fillPaySystems()
{
    QString currentData;
    if (ui->comboBoxPaymentType->currentIndex() >= 0)
        currentData = ui->comboBoxPaymentType->currentData().toString();
    ui->comboBoxPaymentType->clear();
    if (ui->comboBoxPaymentTarget->currentIndex() == 0)
        ui->comboBoxPaymentType->addItem("С лицевого счета", "0");
    if (!mPaySystems->size())
        ApiAdapter::getApi()->getListPaySystems(mPaySystems);
    for (int i = 0; i < mPaySystems->size(); ++i)
        ui->comboBoxPaymentType->addItem(mPaySystems->at(i)->getName(), mPaySystems->at(i)->getId());
    if (currentData.isEmpty())
        ui->comboBoxPaymentType->setCurrentIndex(0);
    else {
        for (int i = 0; i < ui->comboBoxPaymentType->count(); ++i)
            if (ui->comboBoxPaymentType->itemData(i).toString() == currentData) {
                ui->comboBoxPaymentType->setCurrentIndex(i);
                break;
            }
    }
}

void FormPaymentCard::initCurrencies()
{
    mListCurrencies = new ListCurrencies();
    if (ApiAdapter::getApi()->getListCurrencies(mListCurrencies)) {
        ui->comboBoxCurrencies->clear();
        for (int i = 0; i < mListCurrencies->size(); i++) {
            ui->comboBoxCurrencies->addItem(mListCurrencies->at(i)->getName(),
                                            mListCurrencies->at(i)->getCode());
            if (mListCurrencies->at(i)->getCode() == SEConfig::getMainInfo()->baseCurrency()) {
                ui->comboBoxCurrencies->setCurrentIndex(i);
                mPayment->setCurrency(SEConfig::getMainInfo()->baseCurrency());
                SEConfig::setSufficPrefixSpinBox(ui->doubleSpinBoxPaymentSum, mListCurrencies,
                                                 mPayment->currency());
            }
        }
    }

}

void FormPaymentCard::onOk()
{
    if (mIsOrderMode) {
        mPayment->setName(ui->comboBoxPaymentType->currentText());
        mPayment->setIdPaymentType(ui->comboBoxPaymentType->currentData().toString());
        mPayment->setPayerName(mOrder->customer());
    }

    if (mIsOrderMode || !mPayment->isModified() || save())
        accept();
}

void FormPaymentCard::onChangeCurrency()
{
    mPayment->setCurrency(ui->comboBoxCurrencies->currentData().toString());
    SEConfig::setSufficPrefixSpinBox(ui->doubleSpinBoxPaymentSum, mListCurrencies,
                                     mPayment->currency());
}

void FormPaymentCard::onChangePaymentTarget()
{    
    ui->comboBoxCurrencies->setEnabled(ui->comboBoxPaymentTarget->currentIndex());

    mPayment->setPaymentTarget(ui->comboBoxPaymentTarget->currentIndex());
    if (!ui->comboBoxPaymentTarget->currentIndex()) {
        ui->labelPayeer->setText(tr("Плательщик*"));
        ui->toolButtonSelectPayer->setToolTip(tr("Выбрать плательщика"));
        setCurrencyByOrder();
    } else
    {
        ui->labelPayeer->setText(tr("Получатель*"));
        ui->toolButtonSelectPayer->setToolTip(tr("Выбрать получателя"));
    }

    ui->groupBoxOrderInfo->setVisible(!ui->comboBoxPaymentTarget->currentIndex());
    fillPaySystems();
}

void FormPaymentCard::onChangeTypePayment()
{
    ui->labelAccountSum->setVisible(!ui->comboBoxPaymentType->currentIndex() &&
                                    !ui->comboBoxPaymentTarget->currentIndex());
    ui->lineEditAccountSum->setVisible(!ui->comboBoxPaymentType->currentIndex() &&
                                       !ui->comboBoxPaymentTarget->currentIndex());
    mPayment->setIdPaymentType(ui->comboBoxPaymentType->currentData().toString());
    if (ui->doubleSpinBoxPaymentSum->value() == 0 &&
            ((mPayment->idPaymentType() == "0" && mOrder->surchargeSum() <= mContact->balance()) ||
             mPayment->idPaymentType() != "0"))
        ui->doubleSpinBoxPaymentSum->setValue(mOrder->surchargeSum());
}

void FormPaymentCard::onChangeNote()
{
    mPayment->setNote(ui->plainTextEdit->toPlainText().trimmed());
}

bool FormPaymentCard::save()
{
    if (mOrder->getId().isEmpty() && ui->comboBoxPaymentTarget->currentIndex() == 0) {
        QMessageBox::warning(this, tr("Cохранение платежа"),  tr("Не выбран заказ!"));
        ui->lineEditOrder->setFocus();
        return false;
    }

    if (mPayment->amount() <= 0) {
        QMessageBox::warning(this, tr("Cохранение платежа"),  tr("Не указана сумма оплаты!"));
        ui->doubleSpinBoxPaymentSum->setFocus();
        return false;
    }

    if (mPayment->idPaymentType() == "0" && mPayment->amount() > mContact->balance()) {
        QMessageBox::warning(this, tr("Cохранение платежа"),
                             tr("Сумма платежа превышает сумму на лицевом счете плательщика!"));
        ui->doubleSpinBoxPaymentSum->setFocus();
        ui->doubleSpinBoxPaymentSum->selectAll();
        return false;
    }

    return ApiAdapter::getApi()->savePayment(mPayment);
}

void FormPaymentCard::selectOrder()
{
    setCursor(Qt::WaitCursor);
    FormOrders formOrders(this, false);
    formOrders.setFilterOnlyNotPaidOrders();
    if (formOrders.exec() == QDialog::Accepted) {
        if (formOrders.selectedOrders()->size()) {
            mOrder->copy(formOrders.selectedOrders()->at(0));
            mPayment->setIdOrder(mOrder->getId());
            mPayment->setOrderAmount(mOrder->sum());
            if (ApiAdapter::getApi()->getInfoOrder(mOrder) && mContact->getId().isEmpty()) {
                mContact->setId(mOrder->idCustomer());
                mPayment->setIdPayer(mContact->getId());
                ApiAdapter::getApi()->getInfoContact(mContact);
                ui->lineEditPayer->setText(mContact->displayName());
                ui->lineEditAccountSum->setText(mContact->balanceTitle());
            }
            ui->lineEditOrder->setText("Заказ № " + mOrder->getId() + " от " +
                                       mOrder->dateOrder().toString("dd.MM.yyyy"));
            ui->lineEditCustomer->setText(mOrder->customer());
            ui->lineEditOrderAmount->setText(mOrder->sumTitle());            
            ui->lineEditPaidAmount->setText(mOrder->paidSumTitle());
            ui->lineEditSurcharge->setText(mOrder->surchargeSumTitle());
            if (mPayment->getId().isEmpty() &&
                    ((mPayment->idPaymentType() == "0" && mOrder->surchargeSum() <= mContact->balance()) ||
                     mPayment->idPaymentType() != "0"))
                ui->doubleSpinBoxPaymentSum->setValue(mOrder->surchargeSum());
            setCurrencyByOrder();
        }
    }
    setCursor(Qt::ArrowCursor);
}

void FormPaymentCard::selectPayer()
{
    setCursor(Qt::WaitCursor);
    FormContacts formContacts(this, false);
    if (formContacts.exec() == QDialog::Accepted) {
        if (formContacts.getSelectedContacts()->size()) {
            mContact->copy(formContacts.getSelectedContacts()->at(0));
            mPayment->setIdPayer(mContact->getId());
            ui->lineEditPayer->setText(mContact->displayName());
            ui->lineEditAccountSum->setText(mContact->balanceTitle());
        }
    }
    setCursor(Qt::ArrowCursor);
}

void FormPaymentCard::setCurrencyByOrder()
{
    for (int i = 0; i < ui->comboBoxCurrencies->count(); ++i)
        if (ui->comboBoxCurrencies->itemData(i).toString() == mOrder->currency()) {
            ui->comboBoxCurrencies->setCurrentIndex(i);
            ui->comboBoxCurrencies->setEnabled(0);
            break;
        }
}
