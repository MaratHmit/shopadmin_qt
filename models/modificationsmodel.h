#ifndef MODIFICATIONSMODEL_H
#define MODIFICATIONSMODEL_H

#include <QAbstractTableModel>

#include "datamodificationgroup.h"

class ModificationsModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit ModificationsModel(QObject *parent, DataModificationGroup *group);

    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    Qt::ItemFlags flags(const QModelIndex &index) const;

    void beginReset();
    void endReset();

private:
    DataModificationGroup *mGroup;

};

#endif // MODIFICATIONSMODEL_H
