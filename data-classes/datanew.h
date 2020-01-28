#ifndef DATANEW_H
#define DATANEW_H

#include "dataimages.h"
#include <QVariant>

class DataNew : public DataImages
{    
    Q_OBJECT
    Q_PROPERTY(QString newsDate READ newsDateStr)

public:
    DataNew();    
    DataNew(const DataNew *d);
    DataNew(const QJsonObject &jsonobject);    

    void copy(const DataNew *d);
    void setFromJSONObject(const QJsonObject &jsonobject);
    QJsonObject getAsJsonObject() const;
    void clearData();    

    void setNewsDate(const QDate &newsDate);
    void setPublicationDate(const QDate &publicationDate);  

    const QDate newsDate() const;
    const QDate publicationDate() const;        
    const QString newsDateStr() const;
    const QString publicationDateStr() const;
    const QString display() const;

private:
    QDate mNewsDate;
    QDate mPublicationDate;   
};

typedef SEVector<DataNew *> ListNews;

#endif // DATANEW_H
