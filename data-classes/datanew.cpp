#include "datanew.h"

DataNew::DataNew()
{
    initialization();
}

DataNew::DataNew(const DataNew *d)
{
    initialization();
    copy(d);
}

DataNew::DataNew(const QJsonObject &jsonobject)
{
    initialization();
    setFromJSONObject(jsonobject);
}

void DataNew::copy(const DataNew *d)
{
    DataItem::copy(d);
    setNewsDate(d->newsDate());
    setPublicationDate(d->publicationDate());
}

void DataNew::setFromJSONObject(const QJsonObject &jsonobject)
{
    DataImages::setFromJSONObject(jsonobject);
    if (jsonobject["newsDate"].isString() && !jsonobject["newsDate"].toString().isEmpty())
        setNewsDate(QDate::fromString(jsonobject["newsDate"].toString(), "yyyy-MM-dd"));
    if (jsonobject["publicationDate"].isString() && !jsonobject["publicationDate"].toString().isEmpty())
        setPublicationDate(QDate::fromString(jsonobject["publicationDate"].toString(), "yyyy-MM-dd"));    
}

QJsonObject DataNew::getAsJsonObject() const
{
    QJsonObject result = DataImages::getAsJsonObject();

    if (mNewsDate != QDate() && mNewsDate.isValid())
        result.insert("newsDate", QJsonValue(mNewsDate.toString("yyyy-MM-dd")));
    if (mPublicationDate != QDate() && mPublicationDate.isValid())
        result.insert("publicationDate", QJsonValue(mPublicationDate.toString("yyyy-MM-dd")));    

    return result;
}

void DataNew::clearData()
{    
    mNewsDate = QDate();
    mPublicationDate = QDate();   
    DataImages::clearData();
}

void DataNew::setNewsDate(const QDate &newDate)
{
    if (newDate.isValid() && mNewsDate != newDate) {
        mNewsDate = newDate;
        setModified("newDate");
    }
}

void DataNew::setPublicationDate(const QDate &publicationDate)
{
    if (publicationDate.isValid() && mPublicationDate != publicationDate) {
        mPublicationDate = publicationDate;
        setModified("publicationDate");
    }
}

const QDate DataNew::newsDate() const
{
    return mNewsDate;
}

const QDate DataNew::publicationDate() const
{
    return mPublicationDate;
}

const QString DataNew::newsDateStr() const
{
    if (mNewsDate.isValid())
        return mNewsDate.toString("dd.MM.yyyy");
    return QString();
}

const QString DataNew::publicationDateStr() const
{
    if (mPublicationDate.isValid())
        return mPublicationDate.toString("dd.MM.yyyy");
    return QString();
}

const QString DataNew::display() const
{
    QString result;
    result += "Дата публикации: " + publicationDateStr() + "\n";
    result += getName();
    return result;
}

