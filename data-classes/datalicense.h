#ifndef DATALICENSE_H
#define DATALICENSE_H

#include "dataitem.h"

class DataLicense : public DataItem
{
public:
    DataLicense();

    void setSerial(const QString &serial);
    void setDataReg(const QDate &datareg);
    void setRegkey(const QString &regkey);

    QString getSerial()const;
    QDate getDataReg()const;
    QString getRegkey()const;

    void setFromJSONObject(const QJsonObject &jsonobject);
    QJsonObject getAsJsonObject()const;

private:
    QString mSerial;
    QDate mDataReg;
    QString mRegkey;

};
typedef SEVector<DataLicense *> ListLicense;

#endif // DATALICENSE_H
