#ifndef DATALETTER_H
#define DATALETTER_H

#include "dataitem.h"

class DataLetter : public DataItem
{
public:
    DataLetter();
    DataLetter(const DataLetter *d);
    DataLetter(const QJsonObject &jsonobject);

    void setFromJSONObject(const QJsonObject &jsonobject);
    QJsonObject getAsJsonObject() const;
    void copy(const DataLetter *d);

    void setSubject(const QString &subject);
    void setLetter(const QString &letter);

    QString subject() const;
    QString letter() const;

private:
    QString mSubject;
    QString mLetter;

};

typedef SEVector<DataLetter *> ListLetters;

#endif // DATALETTER_H
