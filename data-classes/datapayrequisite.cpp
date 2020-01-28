#include "datapayrequisite.h"

DataPayRequisite::DataPayRequisite():
    DataItem()
{
    initialization();
}

DataPayRequisite::DataPayRequisite(const QJsonObject &jsonobject)
{
    initialization();
    setFromJSONObject(jsonobject);
}

void DataPayRequisite::setFromJSONObject(const QJsonObject &jsonobject)
{
    DataItem::setFromJSONObject(jsonobject);
    mIdPayment = jsonobject["idPayment"].toString();
}

QJsonObject DataPayRequisite::getAsJsonObject() const
{
    QJsonObject obj = DataItem::getAsJsonObject();
    obj.insert("idPayment", QJsonValue(mIdPayment));
    return obj;
}

void DataPayRequisite::setIdPayment(const QString &idPayment)
{
    if (mIdPayment != idPayment) {
        mIdPayment = idPayment;
        setModified("idPayment");
    }
}

QString DataPayRequisite::idPayment() const
{
    return mIdPayment;
}

void DataPayRequisite::clearData()
{
    DataItem::clearData();
    mIdPayment.clear();
}
