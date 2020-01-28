#include "datafeature.h"
#include "seconfig.h"

#include <QRgb>
#include <QDebug>

DataFeature::DataFeature():
    DataItem()
{    
    initialization();
}

DataFeature::DataFeature(const DataFeature *d):
    DataItem()
{    
    initialization();
    copy(d);
}

DataFeature::DataFeature(const QJsonObject &jsonobject)
{    
    initialization();
    setFromJSONObject(jsonobject);
}

void DataFeature::copy(const DataFeature *d)
{
    DataItem::copy(d);
    mIdFeature = d->idFeature();  
    mMeasure = d->measure();
    mHEXColor = d->hexColor();
    mIsSEO = d->isSEO();   
    mSortIndexGroup = d->sortIndexGroup();
    mNamesTypesGoods = d->namesTypesGoods();
}

void DataFeature::setMeasure(const QString &measurement)
{
    if (mMeasure != measurement) {
        mMeasure = measurement;
        setModified("measurement");
    }
}

void DataFeature::setIsSEO(const bool &isSEO)
{
    if (mIsSEO != isSEO) {
        mIsSEO = isSEO;
        setIsChecked(isSEO);
        setModified("isSEO");
    }
}

void DataFeature::setHEXColor(const QString &hexColor)
{
    mHEXColor = hexColor;
}

void DataFeature::setIdFeature(const QString &idFeature)
{
    if (mIdFeature != idFeature) {
        mIdFeature = idFeature;
        setModified("idFeature");
    }
}

void DataFeature::setSortIndexGroup(const int &sortIndexGroup)
{
    if (mSortIndexGroup != sortIndexGroup) {
        mSortIndexGroup = sortIndexGroup;
        setModified("sortIndexGroup");
    }
}

void DataFeature::setFromJSONObject(const QJsonObject &jsonobject)
{
    DataItem::setFromJSONObject(jsonobject);

    mMeasure = jsonobject["measure"].toString();
    mIsSEO = jsonobject["isSEO"].toBool();
    mHEXColor = jsonobject["color"].toString();
    mIdFeature = jsonobject["idFeature"].toString();
    mSortIndexGroup = jsonobject["sortIndexGroup"].toInt();
    mNamesTypesGoods = jsonobject["namesTypesGoods"].toString();

    setIsChecked(mIsSEO);
}

QJsonObject DataFeature::getAsJsonObject() const
{
    QJsonObject obj = DataItem::getAsJsonObject();    

    obj.insert("idFeature", QJsonValue(mIdFeature));
    obj.insert("measure", QJsonValue(mMeasure));
    obj.insert("color", QJsonValue(mHEXColor));
    obj.insert("isSEO", QJsonValue(mIsSEO));
    obj.insert("sortIndexGroup", QJsonValue(mSortIndexGroup));
    return obj;
}

QString DataFeature::measure() const
{
    return mMeasure;
}

bool DataFeature::isSEO() const
{
    return mIsSEO;
}

QString DataFeature::hexColor() const
{
    return mHEXColor;
}

const QColor DataFeature::color() const
{    
    if (mHEXColor.length() == 6)
        return QColor("#" + mHEXColor);
    else return QColor();
}

QString DataFeature::idFeature() const
{
    return mIdFeature;
}

int DataFeature::sortIndexGroup() const
{
    return mSortIndexGroup;
}

bool DataFeature::isColorType() const
{
    return getValueType() == "CL";
}

bool DataFeature::isListType() const
{
    return isColorType() || getValueType() == "L";
}

QString DataFeature::namesTypesGoods() const
{
    return mNamesTypesGoods;
}

const QPixmap DataFeature::icoType() const
{
    QString s;
    if (getValueType() == "CL")
        s = ":/16/icons/16x16/color_swatch.png";
    else if (getValueType() == "L")
        s = ":/16/icons/16x16/viewlist.png";
    else s = ":/16/icons/16x16/empty.png";

    QPixmap ico(s);
    return ico;
}

const QColor DataFeature::getColorByPropertyName(const QString &propertyName) const
{
    if (propertyName == "color")
        return color();

    return QColor();
}

const QVariant DataFeature::getPropertyDecoration(const QString &name) const
{
    if (name == "name" && !mHEXColor.isEmpty())
        return getColorByPropertyName("color");            

    return QVariant();
}

const QVariant DataFeature::getPropertyDisplay(const QString &propertyName) const
{
    if (propertyName == "isYAMarket" && property("isYAMarket").toBool())
        return mIcoIsYAMraket;

    return DataItem::getPropertyDisplay(propertyName);
}

const QString DataFeature::typeTitle() const
{    
    for (int i = 0; i < SEConfig::getFeaturesTypes()->size(); i++) {
        if (SEConfig::getFeaturesTypes()->at(i)->getValueType() == getValueType())
            return SEConfig::getFeaturesTypes()->at(i)->getName();
    }
    return QString();
}

void DataFeature::clearData()
{
    DataItem::clearData();

    mIdFeature.clear();
    mMeasure.clear();
    mHEXColor.clear();
    mIsSEO = false;   
    mSortIndexGroup = 0;
}

void DataFeature::initialization()
{
    mSortIndexGroup = 0;
    mIsSEO = false;   
    mIcoIsYAMraket.load(":/16/icons/16x16/check_black.png");
}
