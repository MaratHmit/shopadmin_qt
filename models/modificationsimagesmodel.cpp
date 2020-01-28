#include "modificationsimagesmodel.h"
#include "seconfig.h"
#include "seconsts.h"
#include "filewebloader.h"

#include <QThread>
#include <QDir>

ModificationsImagesModel::ModificationsImagesModel(QObject *parent) :
    NameTableModel(parent)
{
    rootItem = nullptr;
    initVariables();
}

int ModificationsImagesModel::rowCount(const QModelIndex &) const
{
    return 1;
}

int ModificationsImagesModel::columnCount(const QModelIndex &) const
{
    if (!items)
        return 0;

    return items->size();
}

QVariant ModificationsImagesModel::headerData(
        int , Qt::Orientation , int role) const
{
    if (role == Qt::TextAlignmentRole)
        return Qt::AlignLeft;

    if (role != Qt::DisplayRole)
        return QVariant();

    return QVariant();
}

QVariant ModificationsImagesModel::data(const QModelIndex &index, int role) const
{

    QVariant result;

    if (!getItems() || !index.isValid())
        return result;

    if (role == Qt::TextAlignmentRole) {
        if (index.column() == 1)
            return int(Qt::AlignHCenter | Qt::AlignVCenter);
    }

    DataItem *image = nullptr;
    for (int i = 0; i < getItems()->size(); i++)
        if (getItems()->at(i)->row() == index.column()) {
            image = getItems()->at(i);
            break;
        }

    if (!image)
        return result;

    if (role == Qt::DecorationRole)
        return getImage(*image, index);

    return QVariant();
}

DataItem *ModificationsImagesModel::dataItem(const QModelIndex &index) const
{
    DataItem *image = 0;
    for (int i = 0; i < getItems()->size(); i++)
        if (getItems()->at(i)->row() == index.column()) {
            image = getItems()->at(i);
            break;
        }
    return image;
}
