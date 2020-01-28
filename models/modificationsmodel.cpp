#include "modificationsmodel.h"

#include <QDebug>

ModificationsModel::ModificationsModel(
        QObject *parent, DataModificationGroup *group) :
    QAbstractTableModel(parent)
{
    mGroup = group;
}

int ModificationsModel::rowCount(const QModelIndex &) const
{
    return mGroup->modifications()->size();
}

int ModificationsModel::columnCount(const QModelIndex &) const
{
    return mGroup->columns()->size() + 7;
}

QVariant ModificationsModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::TextAlignmentRole) {
        if (index.column() < mGroup->columns()->size() + 1)
            return int(Qt::AlignLeft | Qt::AlignVCenter);
        else return int(Qt::AlignRight | Qt::AlignVCenter);
    }

    if (role == Qt::DisplayRole) {
        if (index.column() < mGroup->columns()->size()) {
            int col = 0;
            for (int i = 0; i < mGroup->modifications()->at(
                     index.row())->values()->size(); i++) {
                if (mGroup->modifications()->at(
                            index.row())->values()->at(i)->idFeature() ==
                        mGroup->columns()->at(index.column())->getId()) {
                    col = i;
                    break;
                }
            }
            return mGroup->modifications()->at(
                        index.row())->values()->at(col)->getName();
        }
        else {
            switch (index.column() - mGroup->columns()->size())
            {
            case 0: return mGroup->modifications()->at(
                            index.row())->article();
            case 1: if (mGroup->modifications()->at(
                            index.row())->count() < 0)
                    return "∞";
                else
                    return mGroup->modifications()->at(
                                index.row())->count();
            case 2: return mGroup->modifications()->at(
                            index.row())->price();
            case 3: return mGroup->modifications()->at(
                            index.row())->pricePurchase();
            case 4: return mGroup->modifications()->at(
                            index.row())->priceSmallOpt();
            case 5: return mGroup->modifications()->at(
                            index.row())->priceOpt();
            case 6: return mGroup->modifications()->at(
                            index.row())->property("description");
            default: break;
            }
        }
    }

    if (role == Qt::EditRole) {
        if (index.column() >= mGroup->columns()->size())
            switch (index.column() - mGroup->columns()->size())
            {
            case 0: return mGroup->modifications()->at(
                            index.row())->article();
            case 1: if (mGroup->modifications()->at(
                            index.row())->count() < 0)
                    return -1;
                else
                    return mGroup->modifications()->at(
                                index.row())->count();
            case 2: return mGroup->modifications()->at(
                            index.row())->price();
            case 3: return mGroup->modifications()->at(
                            index.row())->pricePurchase();
            case 4: return mGroup->modifications()->at(
                            index.row())->priceSmallOpt();
            case 5: return mGroup->modifications()->at(
                            index.row())->priceOpt();
            case 6: return mGroup->modifications()->at(
                            index.row())->property("description");
            default: break;
            }
    }

    return QVariant();
}

QVariant ModificationsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::TextAlignmentRole)
        return Qt::AlignLeft;

    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        if (section < mGroup->columns()->size())
            return mGroup->columns()->at(section)->getName();
        else {
            switch (section - mGroup->columns()->size())
            {
            case 0: return "Артикул";
            case 1: return "Кол-во";
            case 2: return "Розн. цена";
            case 3: return "Закуп. цена";
            case 4: return "М.опт. цена";
            case 5: return "Опт. цена";
            case 6: return "Описание";
            default: break;
            }
        }
    }

    return QVariant();
}

bool ModificationsModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid() && role == Qt::EditRole) {
        if (index.column() >= mGroup->columns()->size())
            switch (index.column() - mGroup->columns()->size())
            {
            case 0: mGroup->modifications()->at(index.row())->setArticle(value.toString());
                break;
            case 1: {
                if (value.toString().isEmpty())
                    mGroup->modifications()->at(
                                index.row())->setCount(-1);
                else mGroup->modifications()->at(
                            index.row())->setCount(value.toReal());
            }
                break;
            case 2: mGroup->modifications()->at(
                            index.row())->setPrice(value.toReal());
                    break;
            case 3: mGroup->modifications()->at(
                            index.row())->setPricePurscase(value.toReal());
                    break;
            case 4: mGroup->modifications()->at(
                            index.row())->setPriceSmallOpt(value.toReal());
                    break;
            case 5: mGroup->modifications()->at(
                            index.row())->setPriceOpt(value.toReal());
                    break;
            case 6: mGroup->modifications()->at(index.row())->setDescription(value.toString());
            default: break;
            }

        emit dataChanged(index, index);
        return true;
    }
    return false;
}

Qt::ItemFlags ModificationsModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flags = QAbstractTableModel::flags(index);

    if (index.isValid() && index.column() >=  mGroup->columns()->size())
        flags |=  Qt::ItemIsEditable;
    return flags;
}

void ModificationsModel::beginReset()
{
    beginResetModel();
}

void ModificationsModel::endReset()
{
    endResetModel();
}
