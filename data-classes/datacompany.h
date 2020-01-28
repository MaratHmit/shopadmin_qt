#ifndef DATACOMPANY_H
#define DATACOMPANY_H

#include "datacontact.h"


class DataCompany : public DataContact
{
   Q_OBJECT

public:
    DataCompany();
    DataCompany(const DataCompany *d);
    DataCompany(const QJsonObject &jsonobject);
    ~DataCompany();

    ListContacts *contacts() const;
    void copy(const DataCompany *d);

    void setFromJSONObject(const QJsonObject &jsonobject);
    QJsonObject getAsJsonObject() const;
    void initialization();

    const QString displayName() const;

    void clearData();
    bool isModified() const;

private:
    ListContacts *mContacts;

};

typedef SEVector<DataCompany *> ListCompanies;

#endif // DATACOMPANY_H
