#include "datapayment.h"
#include "seconfig.h"

DataPayment::DataPayment()
{
    initialization();
}

DataPayment::DataPayment(const QJsonObject &jsonobject)
{
    initialization();
    setFromJSONObject(jsonobject);
}

DataPayment::DataPayment(const DataPayment *d)
{
    initialization();
    if (d)
        copy(d);
}

DataPayment::~DataPayment()
{

}

void DataPayment::setFromJSONObject(const QJsonObject &jsonobject)
{
    DataItem::setFromJSONObject(jsonobject);

    if (!jsonobject["idOrder"].isNull())
        mIdOrder = jsonobject["idOrder"].toString();
    if (!jsonobject["idPayer"].isNull())
        mIdPayer = jsonobject["idPayer"].toString();
    if (!jsonobject["payerName"].isNull())
        mPayerName = jsonobject["payerName"].toString();
    if (!jsonobject["docDate"].isNull())
        mDocDate = QDate::fromString(jsonobject["docDate"].toString(), "yyyy-MM-dd");
    if (!jsonobject["docYear"].isNull())
        mDocYear = jsonobject["docYear"].toInt();
    if (!jsonobject["docNum"].isNull())
        mDocNum = jsonobject["docNum"].toString();
    if (!jsonobject["idManager"].isNull())
        mIdManager = jsonobject["idManager"].toString();
    if (!jsonobject["idPaymentType"].isNull())
        mIdPaymentType = jsonobject["idPaymentType"].toString();
    if (!jsonobject["amount"].isNull())
        mAmount = jsonobject["amount"].toDouble();
    if (!jsonobject["orderAmount"].isNull())
        mOrderAmount = jsonobject["orderAmount"].toDouble();
    if (!jsonobject["idUserAccountId"].isNull())
        mIdUserAccountIn = jsonobject["idUserAccountId"].toString();
    if (!jsonobject["idUserAccountOut"].isNull())
        mIdUserAccountIn = jsonobject["idUserAccountOut"].toString();
    if (!jsonobject["paymentTarget"].isNull())
        mPaymentTarget = jsonobject["paymentTarget"].toInt();
    if (!jsonobject["currency"].isNull())
        mCurrency = jsonobject["currency"].toString();

    setUnModified();
}

QJsonObject DataPayment::getAsJsonObject() const
{
    QJsonObject obj = DataItem::getAsJsonObject();

    if (!mIdOrder.isNull())
        obj.insert("idOrder", QJsonValue(mIdOrder));
    if (!mIdPayer.isNull())
        obj.insert("idPayer", QJsonValue(mIdPayer));
    if (!mIdManager.isNull())
        obj.insert("idManager", QJsonValue(mIdManager));
    if (!mIdPaymentType.isNull())
        obj.insert("idPaymentType", QJsonValue(mIdPaymentType));
    if (mAmount >= 0)
        obj.insert("amount", QJsonValue(mAmount));
    if (mOrderAmount >= 0)
        obj.insert("orderAmount", QJsonValue(mOrderAmount));
    if (!mIdUserAccountIn.isNull())
        obj.insert("idUserAccountId", QJsonValue(mIdUserAccountIn));
    if (!mIdUserAccountOut.isNull())
        obj.insert("idUserAccountOut", QJsonValue(mIdUserAccountOut));
    obj.insert("docDate", QJsonValue(mDocDate.toString("yyyy-MM-dd")));
    obj.insert("docYear", QJsonValue(mDocYear));
    obj.insert("paymentTarget", QJsonValue(mPaymentTarget));
    obj.insert("currency", QJsonValue(mCurrency));

    return obj;
}

void DataPayment::clearData()
{
    DataItem::clearData();

    mIdOrder.clear();
    mIdPayer.clear();
    mDocNum.clear();
    mPayerName.clear();
    mDocDate = QDate::currentDate();
    mDocYear = QDate::currentDate().year();
    mIdPaymentType.clear();
    mIdManager.clear();
    mAmount = 0.0;
    mOrderAmount = 0.0;
    mIdUserAccountIn.clear();
    mIdUserAccountOut.clear();    
    mPaymentTarget = 0;
    mCurrency.clear();
}

void DataPayment::copy(const DataPayment *d)
{
    DataItem::copy(d);

    setIdOrder(d->idOrder());
    setIdPayer(d->idPayer());
    setDocNum(d->docNum());
    setPayerName(d->payerName());
    setDocDate(d->docDate());
    setDocYear(d->docYear());
    setIdPaymentType(d->idPaymentType());
    setIdManager(d->idManager());
    setAmount(d->amount());
    setOrderAmount(d->orderAmount());
    setIdUserAccountIn(d->idUserAccountIn());
    setIdUserAccountOut(d->idUserAccountOut());    
    setPaymentTarget(d->paymentTarget());
    setCurrency(d->currency());

    setUnModified();
}

void DataPayment::copy(const DataItem *d)
{
    copy(dynamic_cast <const DataPayment *> (d));
}

void DataPayment::initialization()
{
    DataItem::initialization();

    mDocDate = QDate::currentDate();
    mDocYear = QDate::currentDate().year();
    mIdPaymentType = QString();
    mIdManager = QString();
    mAmount = 0.0;
    mOrderAmount = 0.0;
    mPaymentTarget = 0;
}

const QString DataPayment::idOrder() const
{
    return mIdOrder;
}

const QString DataPayment::idPayer() const
{
    return mIdPayer;
}

const QString DataPayment::docNum() const
{
    return mDocNum;
}

const QString DataPayment::payerName() const
{
    return mPayerName;
}

const QDate DataPayment::docDate() const
{
    return mDocDate;
}

int DataPayment::docYear() const
{
    return mDocYear;
}

const QString DataPayment::idPaymentType() const
{
    return mIdPaymentType;
}

const QString DataPayment::idManager() const
{
    return mIdManager;
}

double DataPayment::amount() const
{
    return mAmount;
}

double DataPayment::orderAmount() const
{
    return mOrderAmount;
}

const QString DataPayment::idUserAccountIn() const
{
    return mIdUserAccountIn;
}

const QString DataPayment::idUserAccountOut() const
{
    return mIdUserAccountOut;
}

int DataPayment::paymentTarget() const
{
    return mPaymentTarget;
}

const QString DataPayment::currency() const
{
    return mCurrency;
}

const QString DataPayment::docNumTitle() const
{
    if (mDocNum.isEmpty())
        return "-";
    return "№ " + mDocNum;
}

const QString DataPayment::docDateTitle() const
{
    return mDocDate.toString("dd.MM.yyyy");
}

const QString DataPayment::amountTitle() const
{
    return SEConfig::priceCurrency(mAmount, mCurrency);
}

const QString DataPayment::idOrderTitle() const
{
    if (mIdOrder.isEmpty())
        return "-";
    return "№ " + mIdOrder;
}

const QString DataPayment::paymentTargetTitle() const
{
    if (mPaymentTarget)
        return tr("Пополнение счёта");
    return tr("Оплата заказа");
}

const QString DataPayment::display() const
{
    return docNumTitle() + " от " + docDateTitle();
}

void DataPayment::setIdOrder(const QString &idOrder)
{
    if (mIdOrder != idOrder) {
        mIdOrder = idOrder;
        setModified("idOrder");
    }
}

void DataPayment::setIdPayer(const QString &idPayer)
{
    if (mIdPayer != idPayer) {
        mIdPayer = idPayer;
        setModified("idPayer");
    }
}

void DataPayment::setDocNum(const QString &docNum)
{
    if (mDocNum != docNum) {
        mDocNum = docNum;
        setModified("docNum");
    }
}

void DataPayment::setPayerName(const QString &payerName)
{
    if (mPayerName != payerName) {
        mPayerName = payerName;
        setModified("payerName");
    }
}

void DataPayment::setDocDate(const QDate &docDate)
{
    if (mDocDate != docDate) {
        mDocDate = docDate;
        setModified("docDate");
    }
}

void DataPayment::setDocYear(const int &docYear)
{
    if (mDocYear != docYear) {
        mDocYear = docYear;
        setModified("docYear");
    }
}

void DataPayment::setIdPaymentType(const QString &idPaymentType)
{
    if (mIdPaymentType != idPaymentType) {
        mIdPaymentType = idPaymentType;
        setModified("idPaymentType");
    }
}

void DataPayment::setIdManager(const QString &idManager)
{
    if (mIdManager != idManager) {
        mIdManager = idManager;
        setModified("idManager");
    }
}

void DataPayment::setAmount(const double &amount)
{
    if (mAmount != amount) {
        mAmount = amount;
        setModified("amount");
    }
}

void DataPayment::setOrderAmount(const double &orderAmount)
{
    if (mOrderAmount != orderAmount) {
        mOrderAmount = orderAmount;
        setModified("orderAmount");
    }
}

void DataPayment::setIdUserAccountIn(const QString &idUserAccountIn)
{
    if (mIdUserAccountIn != idUserAccountIn) {
        mIdUserAccountIn = idUserAccountIn;
        setModified("idUserAccountIn");
    }
}

void DataPayment::setIdUserAccountOut(const QString &idUserAccountOut)
{
    if (mIdUserAccountOut != idUserAccountOut) {
        mIdUserAccountOut = idUserAccountOut;
        setModified("idUserAccountOut");
    }
}

void DataPayment::setPaymentTarget(const int paymentTarget)
{
    if (mPaymentTarget != paymentTarget) {
        mPaymentTarget = paymentTarget;
        setModified("paymentTarget");
    }
}

void DataPayment::setCurrency(const QString &currency)
{
    if (mCurrency != currency) {
        mCurrency = currency;
        setModified("currency");
    }
}

