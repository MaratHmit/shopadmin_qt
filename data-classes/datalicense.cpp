#include "datalicense.h"

DataLicense::DataLicense():
    DataItem()
{
}

void DataLicense::setSerial(const QString &serial)
{
    if (mSerial != serial) {
        mSerial = serial;
        setModified("serial");
    }
}

void DataLicense::setDataReg(const QDate &datareg)
{
    if (mDataReg != datareg) {
        mDataReg = datareg;
        setModified("datareg");
    }
}

void DataLicense::setRegkey(const QString &regkey)
{
    if (mRegkey != regkey)  {
        mRegkey = regkey;
        setModified("regkey");
    }
}

QString DataLicense::getSerial() const
{
    return mSerial;
}

QDate DataLicense::getDataReg() const
{
    return mDataReg;
}

QString DataLicense::getRegkey() const
{
    return mRegkey;
}

void DataLicense::setFromJSONObject(const QJsonObject &jsonobject)
{
    DataItem::setFromJSONObject(jsonobject);

    mSerial = jsonobject["serial"].toString();
    mDataReg = QDate::fromString(jsonobject["dateReg"].toString(), "yyyy-MM-dd");
    mRegkey = jsonobject["regKey"].toString();
}

QJsonObject DataLicense::getAsJsonObject() const
{
    QJsonObject obj = DataItem::getAsJsonObject();

//    result += "&serial=" + mSerial +
//              "&dateReg=" + mDataReg.toString("yyyy-MM-dd") +
//              "&regKey=" + mRegkey;

    return obj;

}
