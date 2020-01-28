#include "dialogaddeditaccount.h"
#include "ui_dialogaddeditaccount.h"
#include "seconfig.h"
#include "apiadapter.h"

DialogAddEditAccount::DialogAddEditAccount(QWidget *parent, const DataPersonalAccount *account) :
    QDialog(parent),
    ui(new Ui::DialogAddEditAccount)
{
    ui->setupUi(this);
    this->account = new DataPersonalAccount(account);

    initVariables();
    initSignals();
}

DialogAddEditAccount::~DialogAddEditAccount()
{
    delete ui;
    delete account;
}

const DataPersonalAccount *DialogAddEditAccount::getAccount() const
{
    return account;
}

void DialogAddEditAccount::changeTypeOperation()
{
    account->setTypeOperation(ui->comboBoxTypes->currentData().toString().toInt());
}

void DialogAddEditAccount::changeComment()
{
    account->setNote(ui->plainTextEditNotes->document()->toPlainText());
}

void DialogAddEditAccount::showEvent(QShowEvent *)
{
    initTypesOperations();
    getData();
    initSignalsChanged();
    ui->doubleSpinBoxInPayee->setFocus();
}

void DialogAddEditAccount::initVariables()
{
    ui->dateEditAccount->setDate(QDate::currentDate());
    SEConfig::setSufficPrefixSpinBox(ui->doubleSpinBoxInPayee, SEConfig::getMainInfo()->listCurrency(),
                                     SEConfig::getMainInfo()->baseCurrency());
    SEConfig::setSufficPrefixSpinBox(ui->doubleSpinBoxOutPayee, SEConfig::getMainInfo()->listCurrency(),
                                     SEConfig::getMainInfo()->baseCurrency());
}

void DialogAddEditAccount::initSignals()
{
    connect(ui->pushButtonCancel, SIGNAL(clicked()), SLOT(reject()));
    connect(ui->pushButtonOK, SIGNAL(clicked()), SLOT(accept()));
}

void DialogAddEditAccount::initSignalsChanged()
{
    connect(ui->dateEditAccount, SIGNAL(dateChanged(QDate)), account, SLOT(setDatePayee(QDate)));
    connect(ui->comboBoxTypes, SIGNAL(currentIndexChanged(int)), SLOT(changeTypeOperation()));
    connect(ui->doubleSpinBoxInPayee, SIGNAL(valueChanged(double)), account, SLOT(setInPayee(qreal)));
    connect(ui->doubleSpinBoxOutPayee, SIGNAL(valueChanged(double)), account, SLOT(setOutPayee(qreal)));
    connect(ui->plainTextEditNotes, SIGNAL(textChanged()), SLOT(changeComment()));
}

void DialogAddEditAccount::initTypesOperations()
{
    ListDataItems types;
    if (ApiAdapter::getApi()->getListTypeOperationsAccounts(&types)) {
        for (int i = 0; i < types.size(); ++i)
            ui->comboBoxTypes->addItem(types.at(i)->getName(), types.at(i)->getId());
        ui->comboBoxTypes->setCurrentIndex(0);
    }
}

void DialogAddEditAccount::getData()
{
    if (account->getId().isEmpty()) {
        account->setTypeOperation(ui->comboBoxTypes->currentData().toString().toInt());
        account->setDatePayee(ui->dateEditAccount->date());
        account->setCurrency(SEConfig::getMainInfo()->baseCurrency());
        return;
    }

    ui->dateEditAccount->setDate(account->datePayee());
    for (int i = 0; i < ui->comboBoxTypes->count(); ++i)
        if (ui->comboBoxTypes->itemData(i).toString().toInt() == account->typeOperation()) {
            ui->comboBoxTypes->setCurrentIndex(i);
            break;
        }
    ui->doubleSpinBoxInPayee->setValue(account->inPayee());
    ui->doubleSpinBoxOutPayee->setValue(account->outPayee());
    ui->plainTextEditNotes->setPlainText(account->property("note").toString());
    if (account->currency().isEmpty())
        account->setCurrency(SEConfig::getMainInfo()->baseCurrency());
}
