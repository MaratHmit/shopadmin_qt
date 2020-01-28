#include "dataimages.h"

DataImages::DataImages()
{
    initialization();
}

DataImages::DataImages(const DataImages *d)
{
    initialization();
    copy(d);
}

DataImages::DataImages(const QJsonObject &jsonobject)
{
    initialization();
    setFromJSONObject(jsonobject);
}

ListDataItems *DataImages::listImages() const
{
    return mListImages;
}

void DataImages::clearData()
{
     DataItem::clearData();
     mListImages->deleteItems();
     mListCustomFields->deleteItems();
}

void DataImages::copy(const DataImages *d)
{
     DataItem::copy(d);
     mListImages->deleteItems();
     for (int i = 0; i < d->listImages()->size(); i++)
         mListImages->append(new DataItem(d->listImages()->at(i)));
     mListCustomFields->deleteItems();
     for (int i = 0; i < d->listCustomFields()->size(); i++)
         mListCustomFields->append(new DataItem(d->listCustomFields()->at(i)));
     mListCustomFields->setUnModified();


     setUnModified();
}

void DataImages::copy(const DataItem *d)
{
    copy(dynamic_cast <const DataImages *> (d));
}

ListDataItems *DataImages::listCustomFields() const
{
    return mListCustomFields;
}

void DataImages::setFromJSONObject(const QJsonObject &jsonobject)
{
    DataItem::setFromJSONObject(jsonobject);

    if (!jsonobject["images"].isNull()) {
        mListImages->deleteItems();
        QJsonArray items = jsonobject["images"].toArray();
        foreach (const QJsonValue& value, items)
            mListImages->append(new DataItem(value.toObject()));
    }
    if (!jsonobject["dynFields"].isNull()) {
        mListCustomFields->deleteItems();
        QJsonArray items = jsonobject["dynFields"].toArray();
        foreach (const QJsonValue& value, items)
            mListCustomFields->append(new DataItem(value.toObject()));
        mListCustomFields->setUnModified();
    }
}

QJsonObject DataImages::getAsJsonObject() const
{
    QJsonObject obj = DataItem::getAsJsonObject();

    if (isSetAllFields || mListImages->isModified()) {
        mListImages->setIsSetAllFields(true);
        obj.insert("images", QJsonValue(mListImages->getAsJsonArray()));
    }
    if (isSetAllFields || mListCustomFields->isModified()) {
        mListCustomFields->setIsSetAllFields(true);
        obj.insert("dynFields", QJsonValue(mListCustomFields->getAsJsonArray()));
    }

    return obj;
}

bool DataImages::isModified() const
{
    return DataItem::isModified() || mListImages->isModified() || mListCustomFields->isModified();
}

void DataImages::setUnModified()
{
    DataItem::setUnModified();
    mListImages->setUnModified();
}

void DataImages::initialization()
{
    DataItem::initialization();
    mListImages = new ListDataItems();
    mListCustomFields = new ListDataItems();
}


