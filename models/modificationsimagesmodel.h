#ifndef MODIFICATIONSIMAGESMODEL_H
#define MODIFICATIONSIMAGESMODEL_H

#include "nametablemodel.h"

class ModificationsImagesModel : public NameTableModel
{
    Q_OBJECT
public:
    ModificationsImagesModel(QObject *parent = 0);

    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    QVariant data(const QModelIndex &index, int role) const;

    DataItem *dataItem(const QModelIndex &index) const;

signals:

public slots:

};

#endif // MODIFICATIONSIMAGESMODEL_H
