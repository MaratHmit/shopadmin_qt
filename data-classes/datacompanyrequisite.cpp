#include "datacompanyrequisite.h"

DataCompanyRequisite::DataCompanyRequisite()
{
    initialization();
}

DataCompanyRequisite::DataCompanyRequisite(const QJsonObject &jsonobject)
{
    initialization();
    setFromJSONObject(jsonobject);
}

DataCompanyRequisite::~DataCompanyRequisite()
{

}

const QColor DataCompanyRequisite::getColorByPropertyName(const QString &propertyName) const
{
    if (propertyName != "value")
        return QColor("#FAEBD7");

    return QColor();
}

QVariant DataCompanyRequisite::getPropertyEdit(const QString &propertyName) const
{
    if (propertyName == "value")
        return getValue();

    return QVariant();
}

bool DataCompanyRequisite::getIsEditableByPropertyName(const QString &propertyName)
{
    if (propertyName == "value")
        return 1;

    return 0;
}

bool DataCompanyRequisite::setEditDataByPropertyName(const QString &propertyName, const QVariant &data)
{
    if (propertyName == "value") {
        setValue(data.toString());
        return true;
    }

    return false;
}

