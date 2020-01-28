#ifndef LICENSETABLEMODEL_H
#define LICENSETABLEMODEL_H

#include "nametablemodel.h"
#include "datalicense.h"

class LicenseTableModel : public NameTableModel
{
public:
   explicit LicenseTableModel(QObject *parent = 0);

   int columnCount(const QModelIndex &parent) const;
   QVariant data(const QModelIndex &index, int role) const;
   QVariant headerData(int section, Qt::Orientation orientation, int role) const;

   ListLicense *listLicense() const;

private:
    enum{
        HEADER_SERIAL = 0,
        HEADER_NUMORDER = 1,
        HEADER_DATAREG = 2
    };
};

#endif // LICENSETABLEMODEL_H
