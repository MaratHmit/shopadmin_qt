#ifndef DATAPAYREQUISITE_H
#define DATAPAYREQUISITE_H

#include "dataitem.h"

class DataPayRequisite : public DataItem
{
public:
    DataPayRequisite();
    DataPayRequisite(const QJsonObject &jsonobject);


    void setFromJSONObject(const QJsonObject &jsonobject);
    QJsonObject getAsJsonObject() const;

    void setIdPayment(const QString &idPayment);
    QString idPayment() const;

    void clearData();

private:
    QString mIdPayment;
};

#endif // DATAPAYREQUISITE_H
