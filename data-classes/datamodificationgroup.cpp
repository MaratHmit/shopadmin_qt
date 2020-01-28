#include "datamodificationgroup.h"
#include "seconfig.h"

DataModificationGroup::DataModificationGroup()
{
    initialization();
}

DataModificationGroup::DataModificationGroup(const DataModificationGroup *d)
{
    initialization();
    copy(d);
}

DataModificationGroup::DataModificationGroup(const QJsonObject &jsonobject)
{
    initialization();
    setFromJSONObject(jsonobject);
}

DataModificationGroup::~DataModificationGroup()
{
    mColumns->deleteItems();
    mModifications->deleteItems();
    delete mColumns;
    delete mModifications;
}

void DataModificationGroup::initialization()
{    
    mColumns = new ListFeatures();
    mModifications = new ListModifications();
    clearData();
}

void DataModificationGroup::copy(const DataModificationGroup *d)
{
    DataItem::copy(d);
    mType = d->mType;
    for (int i = 0; i < d->columns()->size(); i++)
        mColumns->append(new DataFeature(d->columns()->at(i)));
    for (int i = 0; i < d->modifications()->size(); i++)
        mModifications->append(new DataModification(d->modifications()->at(i)));
}

void DataModificationGroup::setFromJSONObject(const QJsonObject &jsonobject)
{
    DataItem::setFromJSONObject(jsonobject);
    mType = jsonobject["type"].toString();    
    QJsonArray items = jsonobject["columns"].toArray();
    foreach (const QJsonValue& value, items)
        mColumns->append(new DataFeature(value.toObject()));
    items = jsonobject["items"].toArray();
    foreach (const QJsonValue& value, items)
        mModifications->append(new DataModification(value.toObject()));

    setUnModified();
}

QJsonObject DataModificationGroup::getAsJsonObject() const
{
    QJsonObject obj = DataItem::getAsJsonObject();    
    obj.insert("type", mType);    
    mColumns->setIsSetAllFields(true);
    obj.insert("columns", QJsonValue(mColumns->getAsJsonArray()));
    mModifications->setIsSetAllFields(true);
    obj.insert("items", QJsonValue(mModifications->getAsJsonArray()));

    return obj;
}

void DataModificationGroup::clearData()
{
    DataItem::clearData();
    mType.clear();
    mColumns->deleteItems();    
    mModifications->deleteItems();
}

void DataModificationGroup::setType(const QString &type)
{
    if (mType != type)  {
        mType = type;
        setModified("type");
    }
}

ListFeatures *DataModificationGroup::columns() const
{
    return mColumns;
}

ListModifications *DataModificationGroup::modifications() const
{
    return mModifications;
}

QString DataModificationGroup::type() const
{
    return mType;
}

const QString DataModificationGroup::typeTitle() const
{
    if (!SEConfig::getModificationsGroupsPricesTypes())
        return QString();

    for (int i = 0; i < SEConfig::getModificationsGroupsPricesTypes()->size(); i++)
        if (SEConfig::getModificationsGroupsPricesTypes()->at(i)->getId() == type())
            return SEConfig::getModificationsGroupsPricesTypes()->at(i)->getName();
    return QString();
}

bool DataModificationGroup::isModified() const
{
    return DataItem::isModified() || mColumns->isModified() || mModifications->isModified();
}

void DataModificationGroup::setUnModified()
{
    DataItem::setUnModified();

    mColumns->setUnModified();
    mModifications->setUnModified();
}
