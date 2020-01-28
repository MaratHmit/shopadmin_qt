#include "dataspecification.h"

DataSpecification::DataSpecification(QWidget *parent):
    Data(parent)
{

}

void DataSpecification::setFromJSONObject(const QJsonObject &jsonobject)
{
    Data::setFromJSONObject(jsonobject);

    mIdFeature = jsonobject["idFeature"].toInt();
    mType = jsonobject["type"].toString();
    mMeasure = jsonobject["measure"].toString();
    mValueIdList = jsonobject["valueIdList"].toInt();
    mValueList = jsonobject["valueList"].toString();
    mValueNumber = jsonobject["valueNumber"].toDouble();
    mValueBool = jsonobject["valueBool"].toBool();
    mValueString = jsonobject["valueString"].toString();
    mSortIndexGroup = jsonobject["sortIndexGroup"].toInt();
    mColor = jsonobject["color"].toString();
}

void DataSpecification::setIdFeature(const int &idFeature)
{
    if (mIdFeature != idFeature) {
        mIdFeature = idFeature;
        isModified = true;
    }
}

void DataSpecification::setType(const QString &type)
{
    if (mType != type) {
        mType = type;
        isModified = true;
    }
}

void DataSpecification::setMeasure(const QString &measure)
{
    if (mMeasure != measure) {
        mMeasure = measure;
        isModified = true;
    }
}

void DataSpecification::setValueIdList(const int &valueIdList)
{
    if (mValueIdList != valueIdList) {
        mValueIdList = valueIdList;
        isModified = true;
    }
}

void DataSpecification::setValueList(const QString &valueList)
{
    if (mValueList != valueList) {
        mValueList = valueList;
        isModified = true;
    }
}

void DataSpecification::setValueNumber(const qreal &valueNumber)
{
    if (mValueNumber != valueNumber) {
        mValueNumber = valueNumber;
        isModified = true;
    }
}

void DataSpecification::setValueBool(const bool &valueBool)
{
    if (mValueBool != valueBool) {
        mValueBool = valueBool;
        isModified = true;
    }
}

void DataSpecification::setValueString(const QString &valueString)
{
    if (mValueString != valueString) {
        mValueString = valueString;
        isModified = true;
    }
}

void DataSpecification::setColor(const QString &color)
{
    if (mColor != color) {
        mColor = color;
        isModified = true;
    }
}

void DataSpecification::setSortIndexGroup(const int &sortIndexGroup)
{
    if (mSortIndexGroup != sortIndexGroup) {
        mSortIndexGroup = sortIndexGroup;
        isModified = true;
    }
}

int DataSpecification::idFeature() const
{
    return mIdFeature;
}

QString DataSpecification::type() const
{
    return mType;
}

QString DataSpecification::measure() const
{
    return mMeasure;
}

int DataSpecification::valueIdList() const
{
    return mValueIdList;
}

QString DataSpecification::valueList() const
{
    return mValueList;
}

qreal DataSpecification::valueNumber() const
{
    return mValueNumber;
}

bool DataSpecification::valueBool() const
{
    return mValueBool;
}

QString DataSpecification::valueString() const
{
    return mValueString;
}

int DataSpecification::sortIndexGroup() const
{
    return mSortIndexGroup;
}

QString DataSpecification::color() const
{
    return mColor;
}
