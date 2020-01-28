#ifndef DATAINTEGRATION_H
#define DATAINTEGRATION_H

#include <dataitem.h>

class DataIntegration : public DataItem
{
public:
    DataIntegration();    
    DataIntegration(const DataIntegration *d);
    DataIntegration(const QJsonObject &jsonobject);
    ~DataIntegration();

    void setFromJSONObject(const QJsonObject &jsonobject);
    QJsonObject getAsJsonObject() const;

    void initialization();
    void clearData();
    void copy(const DataIntegration *d);
    void copy(const DataItem *d);

    bool isModified() const;
    void setUnModified();

    ListDataItems *listParameters() const;

private:
    ListDataItems *mListParameters;

};

typedef SEVector<DataIntegration *> ListIntegrations;

#endif // DATAINTEGRATION_H
