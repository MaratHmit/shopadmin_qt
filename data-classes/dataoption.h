#ifndef DATAOPTION_H
#define DATAOPTION_H

#include "dataoptionvalue.h"

class DataOption : public DataItem
{
    Q_OBJECT

public:
    DataOption();
    DataOption(const DataOption *d);
    DataOption(const QJsonObject &jsonobject);

    ~DataOption();

    void initialization();
    void copy(const DataOption *d);
    void copy(const DataItem *d);
    void setFromJSONObject(const QJsonObject &jsonobject);
    QJsonObject getAsJsonObject() const;
    void clearData();

    ListOptionValues *optionValues() const;
    bool isModified() const;
    void setUnModified();

private:
    ListOptionValues *mOptionValues;

};

typedef SEVector<DataOption *> ListOptions;

#endif // DATAOPTION_H
