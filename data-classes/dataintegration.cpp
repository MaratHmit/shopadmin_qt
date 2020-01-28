#include "dataintegration.h"

DataIntegration::DataIntegration()
{
    initialization();
}

DataIntegration::DataIntegration(const DataIntegration *d)
{
    initialization();
    copy(d);
}

DataIntegration::DataIntegration(const QJsonObject &jsonobject)
{
    initialization();
    setFromJSONObject(jsonobject);
}

DataIntegration::~DataIntegration()
{
    delete mListParameters;
}

void DataIntegration::setFromJSONObject(const QJsonObject &jsonobject)
{
    DataItem::setFromJSONObject(jsonobject);

    mListParameters->deleteItems();
    QJsonArray items = jsonobject["parameters"].toArray();
    foreach (const QJsonValue& value, items)
        mListParameters->append(new DataItem(value.toObject()));
}

QJsonObject DataIntegration::getAsJsonObject() const
{
     QJsonObject obj = DataItem::getAsJsonObject();

     if (mListParameters->isModified()) {
         mListParameters->setIsSetAllFields(true);
         obj.insert("parameters", QJsonValue(mListParameters->getAsJsonArray()));
     }

     return obj;
}

void DataIntegration::initialization()
{
    DataItem::initialization();

    mListParameters = new ListDataItems;
}

void DataIntegration::clearData()
{
    DataItem::clearData();

    mListParameters->deleteItems();
    setUnModified();
}

void DataIntegration::copy(const DataIntegration *d)
{
    DataItem::copy(d);

    mListParameters->deleteItems();
    for (int i = 0; i < d->mListParameters->size(); i++)
        mListParameters->append(new DataItem(d->mListParameters->at(i)));

    setUnModified();
}

void DataIntegration::copy(const DataItem *d)
{
    copy(dynamic_cast <const DataIntegration *> (d));
}

bool DataIntegration::isModified() const
{
    return DataItem::isModified() || mListParameters->isModified();
}

void DataIntegration::setUnModified()
{
    DataItem::setUnModified();

    mListParameters->setUnModified();
}

ListDataItems *DataIntegration::listParameters() const
{
    return mListParameters;
}

