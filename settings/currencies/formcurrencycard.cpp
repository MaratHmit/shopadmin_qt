#include "formcurrencycard.h"
#include "ui_formcurrencycard.h"
#include "apiadapter.h"

#include "QFile"
#include "QRegExpValidator"

FormCurrencyCard::FormCurrencyCard(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FormCurrencyCard)
{
    ui->setupUi(this);
    setValidators();
    initVariables();
    initListCurrencies();
    initSignals();
}

FormCurrencyCard::~FormCurrencyCard()
{
    delete ui;
    delete mCurrency;
}

void FormCurrencyCard::setCurrency(const DataCurrency *currency)
{
    mCurrency->copy(currency);
}

const DataCurrency *FormCurrencyCard::currency() const
{
    return mCurrency;
}

void FormCurrencyCard::showEvent(QShowEvent *)
{
    fillControls();
    connectChangeSignals();
}

void FormCurrencyCard::onChangeBoxCurrencies(const int &index)
{
    QString item = ui->comboBoxCurrencies->itemData(index).toString();
    QStringList list(item.split("|"));

    if (list.size())
        ui->lineEditCode->setText(list.at(0));
    if (list.size() - 1)
        ui->lineEditName->setText(list.at(1));
    if (list.size() - 2)
        ui->lineEditCodeCBR->setText(list.at(2));
    if (list.size() - 3)
        ui->lineEditFront->setText(list.at(3));
    if (list.size() - 4)
        ui->lineEditFlank->setText(list.at(4));    
}

void FormCurrencyCard::onChangeData()
{
    ui->pushButtonOK->setEnabled(!ui->lineEditName->text().trimmed().isEmpty() &&
                                 !ui->lineEditCode->text().trimmed().isEmpty());
}

void FormCurrencyCard::onOK()
{
    ui->pushButtonCancel->setEnabled(false);
    ui->pushButtonOK->setEnabled(false);

    setCursor(QCursor(Qt::WaitCursor));
    if (ApiAdapter::getApi()->saveCurrency(mCurrency))
        accept();
    setCursor(QCursor(Qt::ArrowCursor));

    ui->pushButtonCancel->setEnabled(true);
    ui->pushButtonOK->setEnabled(true);
}

void FormCurrencyCard::initListCurrencies()
{
    QFile file(":/strings/res/strings/list_currencies.txt");
    if (!file.exists())
        return;

    file.open(QIODevice::ReadOnly);
    QString s(file.readAll());
    QStringList list(s.split("\r\n"));
    for (int i = 0; i < list.size(); i++) {
        if (list.at(i).isEmpty())
            continue;
        QStringList item(list.at(i).split("|"));
        if (item.size() > 1)
            ui->comboBoxCurrencies->addItem(item.at(1), list.at(i));
    }
    ui->comboBoxCurrencies->setCurrentIndex(-1);
}

void FormCurrencyCard::initSignals()
{
    connect(ui->pushButtonCancel, &QAbstractButton::clicked, this, &FormCurrencyCard::reject);
    connect(ui->pushButtonOK, &QAbstractButton::clicked, this, &FormCurrencyCard::onOK);
}

void FormCurrencyCard::initVariables()
{
    mCurrency = new DataCurrency();
}

void FormCurrencyCard::connectChangeSignals()
{
    connect(ui->comboBoxCurrencies, static_cast<void (QComboBox::*)(int)> (&QComboBox::currentIndexChanged),
            this, &FormCurrencyCard::onChangeBoxCurrencies);
    connect(mCurrency, &DataCurrency::modified, this, &FormCurrencyCard::onChangeData);
    connect(ui->lineEditCode, &QLineEdit::textChanged, mCurrency, &DataCurrency::setCode);
    connect(ui->lineEditName, &QLineEdit::textChanged, mCurrency, &DataCurrency::setName);
    connect(ui->lineEditCodeCBR, &QLineEdit::textChanged, mCurrency, &DataCurrency::setCbrCode);
    connect(ui->lineEditFlank, &QLineEdit::textChanged, mCurrency, &DataCurrency::setSuffix);
    connect(ui->lineEditFront, &QLineEdit::textChanged, mCurrency, &DataCurrency::setPreifx);
    connect(ui->lineEditMinSum, &QLineEdit::textChanged, mCurrency,
            static_cast<void (DataCurrency::*)(const QString &)> (&DataCurrency::setMinSum));
}

void FormCurrencyCard::fillControls()
{
    if (mCurrency->getId().isEmpty())
        return;

    ui->lineEditCode->setText(mCurrency->getCode());
    ui->lineEditName->setText(mCurrency->getName());
    ui->lineEditFlank->setText(mCurrency->suffix());
    ui->lineEditFront->setText(mCurrency->prefix());
    ui->lineEditMinSum->setText(QString::number(mCurrency->minSum(), 'f', 2));
    ui->lineEditCodeCBR->setText(mCurrency->cbrCode());

    for (int i = 0; i < ui->comboBoxCurrencies->count(); ++i) {
        QStringList item(ui->comboBoxCurrencies->itemData(i).toString().split("|"));
        if (item.size() && item.at(0) == mCurrency->getCode()) {
            ui->comboBoxCurrencies->setCurrentIndex(i);
            break;
        }
    }
}

void FormCurrencyCard::setValidators()
{
    QRegExp regExprLat("\\[A-Z]{3}");
    QRegExpValidator *validLat = new QRegExpValidator(regExprLat, this);
    ui->lineEditCode->setValidator(validLat);

    QRegExp regExprFloat("\\d*\\.{1}\\d{2}$|^\\d*$");
    QRegExpValidator *validCurrency = new QRegExpValidator(regExprFloat, this);
    ui->lineEditMinSum->setValidator(validCurrency);
}
