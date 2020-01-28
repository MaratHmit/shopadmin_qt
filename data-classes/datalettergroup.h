#ifndef DATALETTERGROUP_H
#define DATALETTERGROUP_H

#include "dataitem.h"

class DataLetterGroup : public DataItem
{
public:
    DataLetterGroup();
    DataLetterGroup(const DataLetterGroup *d);
    DataLetterGroup(const QJsonObject &jsonobject);
    ~DataLetterGroup();

    void setFromJSONObject(const QJsonObject &jsonobject);
    QJsonObject getAsJsonObject() const;
    void copy(const DataLetterGroup *d);
    void initialization();

    ListDataItems *templates() const;

private:
    ListDataItems *mTemplates;
};

typedef SEVector<DataLetterGroup *> ListLettersGroups;

#endif // DATALETTERGROUP_H
