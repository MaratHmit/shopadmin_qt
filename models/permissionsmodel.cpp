#include "permissionsmodel.h"
#include "request.h"
#include "seconsts.h"
#include "seconfig.h"
#include "qmath.h"

PermissionsModel::PermissionsModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    items = new ListDataItems;
    apiFetch = new Api;
    apiSave = new Api;

    connect(apiFetch, SIGNAL(finishedRequest(bool,QString)), SLOT(finishedRefreshData(bool,QString)));

}

PermissionsModel::~PermissionsModel()
{    
    delete items;

    apiFetch->deleteLater();
    apiSave->deleteLater();
}

QVariant PermissionsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::TextAlignmentRole)
        return Qt::AlignCenter;

    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal && section) {
        switch (section) {        
        case 1:
            return "Полный доступ";
            break;
        case 2:
            return "Чтение";
            break;
        case 3:
            return "Добавление";
            break;
        case 4:
            return "Редактирование";
            break;
        case 5:
            return "Удаление";
            break;
        default:
            break;
        }
    }

    return QVariant();
}

bool PermissionsModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
    if (value != headerData(section, orientation, role)) {

        emit headerDataChanged(orientation, section, section);
        return true;
    }
    return false;
}


int PermissionsModel::rowCount(const QModelIndex &) const
{
    return items->size();
}

int PermissionsModel::columnCount(const QModelIndex &) const
{
    return 6;
}

QVariant PermissionsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::TextAlignmentRole && index.column())
        return (int) (Qt::AlignHCenter | Qt::AlignVCenter);

    if (role == Qt::DecorationRole && index.column()) {
        int maskRole = items->at(index.row())->property("mask").toInt();
        int maskObject = pow(2, SEConsts::PERMISSION_MASK_SIZE - index.column() + 1);
        if (index.column() == 1) {
            maskObject = pow(2, SEConsts::PERMISSION_MASK_SIZE) - 1;
            return maskRole == maskObject ? QPixmap(":/16/icons/16x16/circle_green.png") :
                                           QPixmap(":/16/icons/16x16/circle_red.png");
        } else
            return maskRole & maskObject ? QPixmap(":/16/icons/16x16/circle_green.png") :
                                           QPixmap(":/16/icons/16x16/circle_red.png");
    }

    if (role == Qt::DisplayRole && !index.column())
        return items->at(index.row())->getName();


    return QVariant();
}

bool PermissionsModel::setData(const QModelIndex &index, const QVariant &, int )
{
    if (!index.column())
        return false;

    int maskObject = (int) pow(2, SEConsts::PERMISSION_MASK_SIZE - index.column() + 1);
    int maskRole = items->at(index.row())->property("mask").toInt();
    if (index.column() == 1) {
        maskObject = pow(2, SEConsts::PERMISSION_MASK_SIZE) - 1;
        maskRole == maskObject ? items->at(index.row())->setProperty("mask", 0) :
                                 items->at(index.row())->setProperty("mask", maskObject);
    } else {
        items->at(index.row())->setProperty("mask", maskRole ^ maskObject);        
    }
    save(*items->at(index.row()));
    emit dataChanged(this->index(index.row(), 1), this->index(index.row(), columnCount() - 1));

    return false;
}

Qt::ItemFlags PermissionsModel::flags(const QModelIndex &) const
{
    Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    flags |= Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;

    return flags;
}

void PermissionsModel::refreshData(const QString &idRole)
{
    request.clear();
    this->idRole = idRole;
    if (!idRole.isEmpty())
        request.setFilter("idRole = " + idRole);
    apiFetch->requestInThread(SEConsts::API_PERMISSION, SEConsts::APIMETHOD_FETCH, request);
}

void PermissionsModel::clicked(const QModelIndex &index)
{
    setData(index, QVariant(), Qt::EditRole);
}

void PermissionsModel::headerClicked(int column)
{
    if (!items->size() || !column)
        return;

    int maskObject = column == 1 ? pow(2, SEConsts::PERMISSION_MASK_SIZE) - 1 :
                                   pow(2, SEConsts::PERMISSION_MASK_SIZE - column + 1);
    int maskRole = column == 1 ? items->at(0)->property("mask").toInt() :
                                 items->at(0)->property("mask").toInt() & maskObject;

    beginResetModel();
    for (int i = 0; i < items->size(); ++i) {
        if (column == 1)
            maskRole ? items->at(i)->setProperty("mask", 0) :
                       items->at(i)->setProperty("mask", maskObject);
        else
            maskRole ? items->at(i)->setProperty("mask", maskObject ^ items->at(i)->property("mask").toInt()) :
                       items->at(i)->setProperty("mask", maskObject | items->at(i)->property("mask").toInt());
        save(*items->at(i));
    }
    endResetModel();
}

void PermissionsModel::finishedRefreshData(const bool status, const QString message)
{
    if (status && apiFetch->getListData(SEConsts::API_PERMISSION, items, request)) {
        beginResetModel();
        endResetModel();
    }
    emit completedRefreshData(status, message);
}

void PermissionsModel::save(DataItem &item)
{    
    item.setIsSetAllFields(true);
    item.setProperty("idRole", idRole);
    apiSave->requestInThread(SEConsts::API_PERMISSION, SEConsts::APIMETHOD_SAVE, item);
}
