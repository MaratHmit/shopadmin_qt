#include "licensetablemodel.h"

LicenseTableModel::LicenseTableModel(QObject *parent):
    NameTableModel(parent)
{

}

int LicenseTableModel::columnCount(const QModelIndex &) const
{
    return 3;
}

QVariant LicenseTableModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::TextAlignmentRole)
        return (int) (Qt::AlignLeft | Qt::AlignVCenter);        

    if (!getItems() || role != Qt::DisplayRole)
        return QVariant();

    DataLicense *license = reinterpret_cast<DataLicense *> (getItems()->at(index.row()));

    switch (index.column()) {
    case HEADER_SERIAL:
        return license->getSerial();
    case HEADER_NUMORDER:
        return QVariant();
    case HEADER_DATAREG:
        if(license->getDataReg().toString("dd.MM.yyyy") != "30.12.1899")
            return license->getDataReg().toString("dd.MM.yyyy");
        break;    
    default:
        break;
    }

    return QVariant();
}

QVariant LicenseTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole) {
        if (orientation == Qt::Horizontal)
            switch (section) {
            case HEADER_SERIAL:
                return tr("Номер лицензии");
            case HEADER_NUMORDER:
                return tr("Номер заказа");
            case HEADER_DATAREG:
                return tr("Дата регистрации");
            }
    }

    return QVariant();
}

ListLicense *LicenseTableModel::listLicense() const
{
    return reinterpret_cast<ListLicense *> (getItems());
}
