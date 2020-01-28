#include "dataoptionvalue.h"

DataOptionValue::DataOptionValue()
{
    initialization();
}

DataOptionValue::DataOptionValue(const DataOptionValue *d)
{
    initialization();
    copy(d);
}

DataOptionValue::DataOptionValue(const QJsonObject &jsonobject)
{
    initialization();
    setFromJSONObject(jsonobject);
}

DataOptionValue::~DataOptionValue()
{

}

void DataOptionValue::initialization()
{
    DataItem::initialization();

    mPrice = 0;
    mCount = 0;
    mIsDefault = false;
}

void DataOptionValue::copy(const DataOptionValue *d)
{
    DataItem::copy(d);

    mPrice = d->price();
    mCount = d->count();
    mIsDefault = d->isDefault();
}

void DataOptionValue::copy(const DataItem *d)
{
    copy(dynamic_cast <const DataOptionValue *> (d));
}

void DataOptionValue::setFromJSONObject(const QJsonObject &jsonobject)
{
    DataItem::setFromJSONObject(jsonobject);

    if (!jsonobject["price"].isNull())
        mPrice = jsonobject["price"].toDouble();
    if (!jsonobject["isDefault"].isNull())
        mIsDefault = jsonobject["isDefault"].toBool();

    setUnModified();
}

QJsonObject DataOptionValue::getAsJsonObject() const
{
    QJsonObject obj = DataItem::getAsJsonObject();

    if (isSetAllFields || modifiedFields.contains("price"))
        obj.insert("price", QJsonValue(mPrice));
    if (isSetAllFields || modifiedFields.contains("count"))
        obj.insert("count", QJsonValue(mCount));
    if (isSetAllFields || modifiedFields.contains("isDefault"))
        obj.insert("isDefault", QJsonValue(mIsDefault));


    return obj;
}

void DataOptionValue::clearData()
{
    DataItem::clearData();

    mPrice = 0;
    mCount = 0;
    mIsDefault = false;
}

qreal DataOptionValue::price() const
{
    return mPrice;
}

void DataOptionValue::setPrice(const qreal &price)
{
    if (mPrice != price) {
        mPrice = price;
        setModified("price");
    }
}

int DataOptionValue::count() const
{
    return mCount;
}

void DataOptionValue::setCount(const int &count)
{
    if (mCount != count) {
        mCount = count;
        setModified("count");
    }
}

bool DataOptionValue::isDefault() const
{
    return mIsDefault;
}

void DataOptionValue::setDefault(const bool &isDefault)
{
    if (mIsDefault != isDefault) {
        mIsDefault = isDefault;
        setModified("isDefault");
    }
}

bool DataOptionValue::getIsEditableByPropertyName(const QString &propertyName)
{
    if (propertyName == "price")
        return true;
    if (propertyName == "isDefault")
        return true;

    return false;
}

bool DataOptionValue::setEditDataByPropertyName(const QString &propertyName, const QVariant &data)
{
    if (propertyName == "price") {
        setPrice(data.toDouble());
        return true;
    }
    if (propertyName == "isDefault") {
        setDefault(data.toBool());
        return true;
    }

    return false;
}

const QVariant DataOptionValue::getPropertyDisplay(const QString &propertyName) const
{
    if (propertyName == "isDefault") {
        if (mIsDefault)
            return "Да";
        else return "Нет";
    }

    return DataItem::getPropertyDisplay(propertyName);
}
