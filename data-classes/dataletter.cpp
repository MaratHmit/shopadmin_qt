#include "dataletter.h"

DataLetter::DataLetter()
{
    initialization();
}

DataLetter::DataLetter(const DataLetter *d)
{
    initialization();
    copy(d);
}

DataLetter::DataLetter(const QJsonObject &jsonobject)
{
    initialization();
    setFromJSONObject(jsonobject);
}

void DataLetter::setFromJSONObject(const QJsonObject &jsonobject)
{
    DataItem::setFromJSONObject(jsonobject);
    setSubject(jsonobject["subject"].toString());
    setLetter(jsonobject["letter"].toString());
}

QJsonObject DataLetter::getAsJsonObject() const
{
    QJsonObject result = DataItem::getAsJsonObject();
    result.insert("subject", QJsonValue(mSubject));
    result.insert("letter", QJsonValue(mLetter));
    return result;
}

void DataLetter::copy(const DataLetter *d)
{
    DataItem::copy(d);
    setSubject(d->subject());
    setLetter(d->letter());
}

void DataLetter::setSubject(const QString &subject)
{
    if (mSubject != subject) {
        mSubject = subject;
        setModified("subject");
    }
}

void DataLetter::setLetter(const QString &letter)
{
    if (mLetter != letter) {
        mLetter = letter;
        setModified("letter");
    }
}

QString DataLetter::subject() const
{
    return mSubject;
}

QString DataLetter::letter() const
{
    return mLetter;
}
