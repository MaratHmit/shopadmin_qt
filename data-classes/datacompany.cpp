#include "datacompany.h"

DataCompany::DataCompany()
{
    initialization();
}

DataCompany::DataCompany(const DataCompany *d)
{
    initialization();
    copy(d);
}

DataCompany::DataCompany(const QJsonObject &jsonobject)
{
    initialization();
    setFromJSONObject(jsonobject);
}

DataCompany::~DataCompany()
{

}

ListContacts *DataCompany::contacts() const
{
    return mContacts;
}

void DataCompany::copy(const DataCompany *d)
{
    DataContact::copy(d);

    mContacts->deleteItems();
    for (int i = 0; i < d->contacts()->size(); ++i)
        mContacts->append(d->contacts()->at(i), false);
}

void DataCompany::setFromJSONObject(const QJsonObject &jsonobject)
{
    DataContact::setFromJSONObject(jsonobject);

    if (jsonobject["contacts"].isArray()) {
        QJsonArray items = jsonobject["contacts"].toArray();
        foreach (const QJsonValue& value, items)
            mContacts->append(new DataContact(value.toObject()));
        mContacts->setUnModified();
    }
}

QJsonObject DataCompany::getAsJsonObject() const
{
    QJsonObject obj = DataContact::getAsJsonObject();

    if (mContacts->isModified()) {
        mContacts->setIsSetAllFields(true);
        obj.insert("contacts", QJsonValue(mContacts->getAsJsonArray()));
    }
    return obj;
}

void DataCompany::initialization()
{
    DataContact::initialization();
    mContacts = new ListContacts;
}

const QString DataCompany::displayName() const
{
    return getName();
}

void DataCompany::clearData()
{
    DataContact::clearData();
    mContacts->deleteItems();
}

bool DataCompany::isModified() const
{
    return DataContact::isModified() || mContacts->isModified();
}
