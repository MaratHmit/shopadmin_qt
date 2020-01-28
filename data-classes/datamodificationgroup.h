#ifndef DATAMODIFICATIONGROUP_H
#define DATAMODIFICATIONGROUP_H

#include "datamodification.h"

class DataModificationGroup : public DataItem
{
    Q_OBJECT
    Q_PROPERTY(QString type READ typeTitle)

public:
    DataModificationGroup();
    DataModificationGroup(const DataModificationGroup *d);
    DataModificationGroup(const QJsonObject &jsonobject);
    ~DataModificationGroup();

    void initialization();

    void copy(const DataModificationGroup *d);
    void setFromJSONObject(const QJsonObject &jsonobject);
    QJsonObject getAsJsonObject() const;
    void clearData();
    void setType(const QString &type);

    ListFeatures *columns() const;
    ListModifications *modifications() const;
    QString type() const;
    const QString typeTitle() const;
    bool isModified() const;
    void setUnModified();

private:
    QString mType;
    ListFeatures *mColumns;
    ListModifications *mModifications;
};

typedef SEVector<DataModificationGroup *> ListModificationsGroups;

#endif // DATAMODIFICATIONGROUP_H
