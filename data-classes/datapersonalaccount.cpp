#include "datapersonalaccount.h"
#include "seconfig.h"

DataPersonalAccount::DataPersonalAccount()
{
    initialization();
}

DataPersonalAccount::DataPersonalAccount(const DataPersonalAccount *d)
{
    initialization();
    if (d)
        copy(d);
}

DataPersonalAccount::DataPersonalAccount(const QJsonObject &jsonobject)
{
    initialization();
    setFromJSONObject(jsonobject);
}

void DataPersonalAccount::copy(const DataItem *d)
{
    copy(dynamic_cast <const DataPersonalAccount *> (d));
}

void DataPersonalAccount::copy(const DataPersonalAccount *d)
{
    DataItem::copy(d);

    setOrderId(d->orderId());
    setAccount(d->account());
    setDatePayee(d->datePayee());
    setInPayee(d->inPayee());
    setOutPayee(d->outPayee());
    setCurrency(d->currency());
    setTypeOperation(d->typeOperation());
    mBalance = d->balance();
}

void DataPersonalAccount::setFromJSONObject(const QJsonObject &jsonobject)
{
    DataItem::setFromJSONObject(jsonobject);

    setOrderId(jsonobject["orderId"].toString());
    setAccount(jsonobject["account"].toString());
    setDatePayee(QDate::fromString(jsonobject["datePayee"].toString(), "yyyy-MM-dd"));
    setInPayee(jsonobject["inPayee"].toDouble());
    setOutPayee(jsonobject["outPayee"].toDouble());
    mBalance = jsonobject["balance"].toDouble();
    setCurrency(jsonobject["currency"].toString());
    setTypeOperation(jsonobject["typeOperation"].toInt());
}

QJsonObject DataPersonalAccount::getAsJsonObject() const
{
    QJsonObject result = DataItem::getAsJsonObject();

    if (!mOrderId.isNull())
        result.insert("orderId", QJsonValue(mOrderId));
    if (!mAccount.isNull())
        result.insert("orderId", QJsonValue(mAccount));
    if (!mDatePayee.isNull())
        result.insert("datePayee", QJsonValue(mDatePayee.toString("yyyy-MM-dd")));
    result.insert("inPayee", QJsonValue(mInPayee));
    result.insert("outPayee", QJsonValue(mOutPayee));
    if (!mCurrency.isNull())
        result.insert("currency", QJsonValue(mCurrency));
    result.insert("typeOperation", QJsonValue(mTypeOperation));
    return result;
}

void DataPersonalAccount::setOrderId(const QString &orderId)
{
    if (mOrderId != orderId) {
        mOrderId = orderId;
        setModified("orderId");
    }
}

void DataPersonalAccount::setAccount(const QString &account)
{
    if (mAccount != account) {
        mAccount = account;
        setModified("account");
    }
}

void DataPersonalAccount::setDatePayee(const QDate &datePayee)
{
    if (mDatePayee != datePayee) {
        mDatePayee = datePayee;
        setModified("datePayee");
    }
}

void DataPersonalAccount::setInPayee(const qreal &inPayee)
{
    if (mInPayee != inPayee) {
        mInPayee = inPayee;
        setModified("inPayee");
    }
}

void DataPersonalAccount::setOutPayee(const qreal &outPayee)
{
    if (mOutPayee != outPayee) {
        mOutPayee = outPayee;
        setModified("outPayee");
    }
}

void DataPersonalAccount::setCurrency(const QString &currency)
{
    if (mCurrency != currency) {
        mCurrency = currency;
        setModified("currency");
    }
}

void DataPersonalAccount::setTypeOperation(const int &typeOperation)
{
    if (mTypeOperation != typeOperation) {
        mTypeOperation = typeOperation;
        setModified("typeOperation");
    }
}

void DataPersonalAccount::setBalance(const qreal &balance)
{
    mBalance = balance;
}

QString DataPersonalAccount::orderId() const
{
    return mOrderId;
}

QString DataPersonalAccount::account() const
{
    return mAccount;
}

QDate DataPersonalAccount::datePayee() const
{
    return mDatePayee;
}

qreal DataPersonalAccount::inPayee() const
{
    return mInPayee;
}

qreal DataPersonalAccount::outPayee() const
{
    return mOutPayee;
}

qreal DataPersonalAccount::balance() const
{
    return mBalance;
}

QString DataPersonalAccount::currency() const
{
    return mCurrency;
}

int DataPersonalAccount::typeOperation() const
{
    return mTypeOperation;
}

QString DataPersonalAccount::inPayeeTitle() const
{
    return SEConfig::priceCurrency(inPayee(), mCurrency);
}

QString DataPersonalAccount::outPayeeTitle() const
{
    return SEConfig::priceCurrency(outPayee(), mCurrency);
}

QString DataPersonalAccount::balanceTitle() const
{
    return SEConfig::priceCurrency(balance(), mCurrency);
}

void DataPersonalAccount::initialization()
{
    mInPayee = 0.0;
    mOutPayee = 0.0;
    mCurrency = 0.0;
    mTypeOperation = 0;
}
