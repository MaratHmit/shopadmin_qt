#ifndef DATAIMAGES_H
#define DATAIMAGES_H

#include "dataitem.h"

class DataImages : public DataItem
{
    Q_OBJECT

public:
    DataImages();    
    DataImages(const DataImages *d);
    DataImages(const QJsonObject &jsonobject);

    ListDataItems *listImages() const;

    void clearData();
    void copy(const DataImages *d);
    void copy(const DataItem *d);

    void setFromJSONObject(const QJsonObject &jsonobject);
    QJsonObject getAsJsonObject() const;

    virtual bool isModified() const;
    void setUnModified();

    ListDataItems *listCustomFields() const;

protected:
    virtual void initialization();

private:
    ListDataItems *mListImages;
    ListDataItems *mListCustomFields;

};

#endif // DATAIMAGES_H
