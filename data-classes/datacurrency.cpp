#include "datacurrency.h"
#include "seconfig.h"

#include <QColor>

DataCurrency::DataCurrency()
{
    initialization();
}

DataCurrency::DataCurrency(const DataCurrency *d)
{
    initialization();
    copy(d);
}

DataCurrency::DataCurrency(const QJsonObject &jsonobject)
{
    initialization();
    setFromJSONObject(jsonobject);
}

void DataCurrency::copy(const DataCurrency *d)
{
    DataItem::copy(d);

    setSuffix(d->suffix());
    setPreifx(d->prefix());
    setMinSum(d->minSum());
    setCbrCode(d->cbrCode());
    setRate(d->rate());
}

void DataCurrency::copy(const DataItem *d)
{
    copy(dynamic_cast <const DataCurrency *> (d));
}

void DataCurrency::clearData()
{
    DataItem::clearData();

    mMinSum = 0;
    mRate = 0;
    mRateDate = QDate::currentDate();
    mRateDisplay = "-";
}

void DataCurrency::initialization()
{
    DataItem::initialization();

    mMinSum = 0;
    mRate = 0;
    mRateDate = QDate::currentDate();
    mRateDisplay = "-";
}

void DataCurrency::setPreifx(const QString &prefix)
{
    if (mPrefix != prefix) {
        mPrefix = prefix;
        setModified("prefix");
    }
}

void DataCurrency::setSuffix(const QString &suffix)
{
    if (mSuffix != suffix) {
        mSuffix = suffix;
        setModified("suffix");
    }

}

void DataCurrency::setCbrCode(const QString &cbrCode)
{
    if (mCbrCode != cbrCode) {
        mCbrCode = cbrCode;
        setModified("cbrCode");
    }
}

void DataCurrency::setMinSum(const QString &minSum)
{
    setMinSum(minSum.toFloat());
}

void DataCurrency::setMinSum(const double &minSum)
{
    if (mMinSum != minSum) {
        mMinSum = minSum;
        setModified("minSum");
    }
}

void DataCurrency::setRate(const double &rate)
{
    if (mRate != rate) {
        mRate = rate;
        setModified("rate");
    }
}

void DataCurrency::setRateDate(const QDate &rateDate)
{
    if (mRateDate != rateDate) {
        mRateDate = rateDate;
        setModified("rateDate");
    }
}

QString DataCurrency::prefix() const
{
    return mPrefix;
}

QString DataCurrency::suffix() const
{
    return mSuffix;
}

QString DataCurrency::cbrCode() const
{
    return mCbrCode;
}

double DataCurrency::minSum() const
{
    return mMinSum;
}

double DataCurrency::rate() const
{
    return mRate;
}

QString DataCurrency::strRate() const
{
    return QString::number(mRate, 'f', 4);
}

QDate DataCurrency::rateDate() const
{
    return mRateDate;
}

QString DataCurrency::rateDisplay() const
{
    return mRateDisplay;
}

bool DataCurrency::isCanAutoRate() const
{
    return mIsCanAutoRate;
}

const QColor DataCurrency::getColorByPropertyName(const QString &propertyName) const
{
    if (getCode() == SEConfig::getMainInfo()->baseCurrency())
        return QColor("#FFF8DC");

    if (propertyName == "rate" && mRate <= 0)
        return QColor("#FFDAB9");

    return QColor();
}

void DataCurrency::setFromJSONObject(const QJsonObject &jsonobject)
{
    DataItem::setFromJSONObject(jsonobject);

    mPrefix = jsonobject["prefix"].toString();
    mSuffix = jsonobject["suffix"].toString();
    mCbrCode = jsonobject["cbrCode"].toString();
    mMinSum = jsonobject["minSum"].toDouble();
    mRate = jsonobject["rate"].toDouble();
    if (jsonobject["rateDate"].isString() && !jsonobject["rateDate"].toString().isEmpty())
        setRateDate(QDate::fromString(jsonobject["rateDate"].toString(), "yyyy-MM-dd"));
    mRateDisplay = jsonobject["rateDisplay"].toString();
    mIsCanAutoRate = jsonobject["isCanAutoRate"].toBool();
}

QJsonObject DataCurrency::getAsJsonObject() const
{
    QJsonObject obj = DataItem::getAsJsonObject();

    obj.insert("prefix", QJsonValue(mPrefix));
    obj.insert("suffix", QJsonValue(mSuffix));
    obj.insert("cbrCode", QJsonValue(mCbrCode));
    obj.insert("minSum", QJsonValue(mMinSum));
    obj.insert("rate", QJsonValue(mRate));
    obj.insert("rateDate", QJsonValue(mRateDate.toString("yyyy-MM-dd")));
    obj.insert("baseCurrency", QJsonValue(SEConfig::getMainInfo()->baseCurrency()));
    return obj;
}
