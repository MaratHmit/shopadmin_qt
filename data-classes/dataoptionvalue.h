#ifndef DATAOPTIONVALUE_H
#define DATAOPTIONVALUE_H

#include "dataitem.h"

class DataOptionValue : public DataItem
{
    Q_OBJECT

    Q_PROPERTY(qreal price READ price WRITE setPrice)
    Q_PROPERTY(int count READ count WRITE setCount)
    Q_PROPERTY(bool isDefault READ isDefault WRITE setDefault)

public:
    DataOptionValue();    
    DataOptionValue(const DataOptionValue *d);
    DataOptionValue(const QJsonObject &jsonobject);

    ~DataOptionValue();

    void initialization();
    void copy(const DataOptionValue *d);
    void copy(const DataItem *d);
    void setFromJSONObject(const QJsonObject &jsonobject);
    QJsonObject getAsJsonObject() const;
    void clearData();

    qreal price() const;
    void setPrice(const qreal &price);

    int count() const;
    void setCount(const int &count);

    bool isDefault() const;
    void setDefault(const bool &isDefault);

    bool getIsEditableByPropertyName(const QString &propertyName);
    bool setEditDataByPropertyName(const QString &propertyName, const QVariant &data);
    const QVariant getPropertyDisplay(const QString &propertyName) const;

private:
    bool mIsDefault;
    qreal mPrice;
    int mCount;

};

typedef SEVector<DataOptionValue *> ListOptionValues;

#endif // DATAOPTIONVALUE_H
