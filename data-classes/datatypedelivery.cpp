#include "datatypedelivery.h"

DataTypeDelivery::DataTypeDelivery()
{
    initialization();
}

DataTypeDelivery::DataTypeDelivery(const QJsonObject &jsonobject)
{
    initialization();
    setFromJSONObject(jsonobject);
}

DataTypeDelivery::~DataTypeDelivery()
{

}

bool DataTypeDelivery::isTreeMode() const
{
    return mIsTreeMode;
}

bool DataTypeDelivery::isNeedRegion() const
{
    return mIsNeedRegion;
}

bool DataTypeDelivery::isNeedConditions() const
{
    return mIsNeedConditions;
}

void DataTypeDelivery::setFromJSONObject(const QJsonObject &jsonobject)
{
    DataItem::setFromJSONObject(jsonobject);

    if (!jsonobject["isTreeMode"].isNull())
        mIsTreeMode = jsonobject["isTreeMode"].toBool();
    if (!jsonobject["isNeedRegion"].isNull())
        mIsNeedRegion = jsonobject["isNeedRegion"].toBool();
    if (!jsonobject["isNeedConditions"].isNull())
        mIsNeedConditions = jsonobject["isNeedConditions"].toBool();
}

void DataTypeDelivery::initialization()
{
    DataItem::initialization();
    mIsTreeMode = false;
    mIsNeedRegion = false;
    mIsNeedConditions = false;
}

