#ifndef DATACOMPANYREQUISITE_H
#define DATACOMPANYREQUISITE_H

#include "dataitem.h"

class DataCompanyRequisite : public DataItem
{
public:
    DataCompanyRequisite();        
    DataCompanyRequisite(const QJsonObject &jsonobject);

    ~DataCompanyRequisite();

    const QColor getColorByPropertyName(const QString &propertyName) const;
    QVariant getPropertyEdit(const QString &propertyName) const;
    bool getIsEditableByPropertyName(const QString &propertyName);
    bool setEditDataByPropertyName(const QString &propertyName, const QVariant &data);
};

#endif // DATACOMPANYREQUISITE_H

typedef SEVector<DataCompanyRequisite *> ListCompanyRequisites;
