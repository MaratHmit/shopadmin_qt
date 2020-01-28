#include "dataoption.h"

DataOption::DataOption()
{
    initialization();
}

DataOption::DataOption(const DataOption *d)
{
    initialization();
    copy(d);

}

DataOption::DataOption(const QJsonObject &jsonobject)
{
    initialization();
    setFromJSONObject(jsonobject);
}

DataOption::~DataOption()
{
    delete mOptionValues;
}

void DataOption::initialization()
{
    DataItem::initialization();

    mOptionValues = new ListOptionValues();
}

void DataOption::copy(const DataOption *d)
{
    DataItem::copy(d);
    for (int i = 0; i < d->optionValues()->size(); i++)
        mOptionValues->append(new DataOptionValue(d->optionValues()->at(i)));
}

void DataOption::copy(const DataItem *d)
{
     copy(dynamic_cast <const DataOption *> (d));
}

void DataOption::setFromJSONObject(const QJsonObject &jsonobject)
{
    DataItem::setFromJSONObject(jsonobject);
    QJsonArray items = jsonobject["optionValues"].toArray();
    foreach (const QJsonValue& value, items)
        mOptionValues->append(new DataOptionValue(value.toObject()));

    setUnModified();
}

QJsonObject DataOption::getAsJsonObject() const
{
    QJsonObject obj = DataItem::getAsJsonObject();
    mOptionValues->setIsSetAllFields(true);
    obj.insert("optionValues", QJsonValue(mOptionValues->getAsJsonArray()));

    return obj;
}

void DataOption::clearData()
{
    DataItem::clearData();
    mOptionValues->deleteItems();
}

ListOptionValues *DataOption::optionValues() const
{
    return mOptionValues;
}

bool DataOption::isModified() const
{
    return DataItem::isModified() || mOptionValues->isModified();
}

void DataOption::setUnModified()
{
    DataItem::setUnModified();

    mOptionValues->setUnModified();
}

