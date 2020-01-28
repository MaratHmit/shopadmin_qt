#ifndef DATATYPEDELIVERY_H
#define DATATYPEDELIVERY_H

#include "dataitem.h"

class DataTypeDelivery : public DataItem
{
public:
    DataTypeDelivery();
    DataTypeDelivery(const QJsonObject &jsonobject);
    ~DataTypeDelivery();

    bool isTreeMode() const;
    bool isNeedRegion() const;
    bool isNeedConditions() const;

    void setFromJSONObject(const QJsonObject &jsonobject);
    void initialization();

private:
   bool mIsTreeMode;
   bool mIsNeedRegion;
   bool mIsNeedConditions;

};

typedef SEVector<DataTypeDelivery *> ListTypesDeliveries;

#endif // DATATYPEDELIVERY_H
