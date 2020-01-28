#include "datalettergroup.h"

DataLetterGroup::DataLetterGroup()
{
    initialization();
}

DataLetterGroup::DataLetterGroup(const DataLetterGroup *d)
{
    initialization();
    copy(d);
}

DataLetterGroup::DataLetterGroup(const QJsonObject &jsonobject)
{
    initialization();
    setFromJSONObject(jsonobject);
}

DataLetterGroup::~DataLetterGroup()
{
    delete mTemplates;
}

void DataLetterGroup::setFromJSONObject(const QJsonObject &jsonobject)
{
    DataItem::setFromJSONObject(jsonobject);

    QJsonArray idgroups = jsonobject["templates"].toArray();
       foreach (const QJsonValue& value, idgroups)
           mTemplates->append(new DataItem(value.toObject()));
}

QJsonObject DataLetterGroup::getAsJsonObject() const
{
    QJsonObject result = DataItem::getAsJsonObject();
    if (mTemplates->isModified())
        result.insert("templates", QJsonValue(mTemplates->getAsJsonArray()));
    return result;
}

void DataLetterGroup::copy(const DataLetterGroup *d)
{
    DataItem::copy(d);
    mTemplates->deleteItems();
    for (int i = 0; i < d->templates()->size(); i++)
        mTemplates->append(new DataItem(d->templates()->at(i)));
}

void DataLetterGroup::initialization()
{
    mTemplates = new ListDataItems();
}

ListDataItems *DataLetterGroup::templates() const
{
    return mTemplates;
}
