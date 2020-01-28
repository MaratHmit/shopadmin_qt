#include "datadeliveryconditions.h"


DataDeliveryConditions::DataDeliveryConditions()
{
    initialization();
}

DataDeliveryConditions::DataDeliveryConditions(const DataDeliveryConditions *d)
{
    initialization();
    copy(d);
}

DataDeliveryConditions::DataDeliveryConditions(const QJsonObject &jsonobject)
{
    initialization();
    setFromJSONObject(jsonobject);
}

void DataDeliveryConditions::copy(const DataDeliveryConditions *d)
{
    DataItem::copy(d);
    setTypeParam(d->typeParam());
    setPrice(d->price());
    setMinValue(d->minValue());
    setMaxValue(d->maxValue());
    setPriority(d->priority());
    setOperation(d->operation());
    setTypePrice(d->typePrice());
}

void DataDeliveryConditions::setFromJSONObject(const QJsonObject &jsonobject)
{
    DataItem::setFromJSONObject(jsonobject);
    setTypeParam(jsonobject["typeParam"].toString());
    setPrice(jsonobject["price"].toDouble());
    setMinValue(jsonobject["minValue"].toDouble());
    setMaxValue(jsonobject["maxValue"].toDouble());
    setPriority(jsonobject["priority"].toInt());
    setOperation(jsonobject["operation"].toString());
    setTypePrice(jsonobject["typePrice"].toString());
}

QJsonObject DataDeliveryConditions::getAsJsonObject() const
{
    QJsonObject result = DataItem::getAsJsonObject();
    result.insert("typeParam", QJsonValue(mTypeParam));
    result.insert("price", QJsonValue(mPrice));
    result.insert("minValue", QJsonValue(mMinValue));
    result.insert("maxValue", QJsonValue(mMaxValue));
    result.insert("priority", QJsonValue(mPriority));
    result.insert("operation", QJsonValue(mOperation));
    result.insert("typePrice", QJsonValue(mTypePrice));
    return result;
}

void DataDeliveryConditions::clearData()
{
    DataItem::clearData();
    mTypeParam = "sum";
    mPrice = 0;
    mMinValue = 0;
    mMaxValue = 0;
    mPriority = 0;
    mOperation = "=";
    mTypePrice = "a";
}

void DataDeliveryConditions::initialization()
{
    mTypeParam = "sum";
    mPrice = 0;
    mMinValue = 0;
    mMaxValue = 0;
    mPriority = 0;
    mOperation = "=";
    mTypePrice = "a";
}

void DataDeliveryConditions::setTypeParam(const QString &typeParam)
{
    if (mTypeParam != typeParam) {
        mTypeParam = typeParam;
        setModified("typeParam");
    }
}

void DataDeliveryConditions::setPrice(const qreal &price)
{
    if (mPrice != price) {
        mPrice = price;
        setModified("price");
    }
}

void DataDeliveryConditions::setMinValue(const qreal &minValue)
{
    if (mMinValue != minValue) {
        mMinValue = minValue;
        setModified("minValue");
    }
}

void DataDeliveryConditions::setMaxValue(const qreal &maxValue)
{
    if (mMaxValue != maxValue) {
        mMaxValue = maxValue;
        setModified("maxValue");
    }
}

void DataDeliveryConditions::setPriority(const int &priority)
{
    if (mPriority != priority) {
        mPriority = priority;
        setModified("priority");
    }
}

void DataDeliveryConditions::setOperation(const QString &operation)
{
    if (operation != mOperation) {
        mOperation = operation;
        setModified("operation");
    }
}

void DataDeliveryConditions::setTypePrice(const QString &typePrice)
{
    if (typePrice != mTypePrice) {
        mTypePrice = typePrice;
        setModified("typePrice");
    }
}

QString DataDeliveryConditions::typeParam() const
{
    return mTypeParam;
}

qreal DataDeliveryConditions::price() const
{
    return mPrice;
}

qreal DataDeliveryConditions::minValue() const
{
    return mMinValue;
}

qreal DataDeliveryConditions::maxValue() const
{
    return mMaxValue;
}

int DataDeliveryConditions::priority() const
{
    return mPriority;
}

QString DataDeliveryConditions::operation() const
{
    return mOperation;
}

QString DataDeliveryConditions::typePrice() const
{
    return mTypePrice;
}

const QString DataDeliveryConditions::titleMinValue() const
{
    return QString::number(mMinValue, 'f', 2);
}

const QString DataDeliveryConditions::titleMaxValue() const
{
    return QString::number(mMaxValue, 'f', 2);
}

const QString DataDeliveryConditions::titleTypeParam() const
{
    if (mTypeParam == "sum")
        return "сумма";
    if (mTypeParam == "weight")
        return "вес";
    if (mTypeParam == "volume")
        return "объем";
    return QString();
}

const QString DataDeliveryConditions::titleDescriptionPrice() const
{
    int pr = (int) mPrice;
    if (pr > 100)
        pr = 100;
    if (pr < 0)
        pr = 0;

    if (mOperation == "=") {
        if (mTypePrice == "a")
            return QString::number(mPrice, 'f', 2);
        if (mTypePrice == "s")
            return QString::number(pr) + "% от суммы заказа";
        if (mTypePrice == "d")
            return QString::number(pr) + "% от базовой цены доставки";
    }
    if (mOperation == "+") {
        if (mTypePrice == "a")
            return "+ " + QString::number(mPrice, 'f', 2) + " к базовой сумме доставки";
        if (mTypePrice == "s")
            return "+ " + QString::number(pr) + "% суммы заказа к базовой цене доставки";
        if (mTypePrice == "d") {
            pr = pr + 100;
            if (pr > 100)
                pr = 100;
            return QString::number(pr) + "% базовой цены доставки";
        }
    }
    if (mOperation == "-") {
        if (mTypePrice == "a")
            return "- " + QString::number(mPrice, 'f', 2) + " от базовой цены доставки";
        if (mTypePrice == "s")
            return "- " + QString::number(pr) + "% суммы заказа от базовой цены доставки";
        if (mTypePrice == "d") {
            pr = 100 - pr;
            if (pr < 0)
                pr = 0;
            if (pr)
                return QString::number(pr) + "% базовой цены доставки";
            else return "0";
        }
    }
    return QString();
}
