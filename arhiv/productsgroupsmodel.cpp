#include <QDebug>

#include "productsgroupsmodel.h"

ProductsGroupsModel::ProductsGroupsModel(QObject *parent):
    NameTableModel(parent)
{
    mRootItem = new DataProductsGroup();
    mRootItem->setId("root");
    mSimpleMode = false;
}

ProductsGroupsModel::~ProductsGroupsModel()
{
    delete mRootItem;
}

void ProductsGroupsModel::setSimpleMode(const bool &simpleMode)
{
    mSimpleMode = simpleMode;
}

int ProductsGroupsModel::columnCount(const QModelIndex &) const
{
    if (mSimpleMode)
        return 1;
    else
        return 3;
}

int ProductsGroupsModel::rowCount(const QModelIndex &parent) const
{
    DataProductsGroup *parentItem;

    if (!parent.isValid()) {
        if (listGroups())
            return listGroups()->size();
        else return 0;
    }
    else
        parentItem = static_cast<DataProductsGroup *>(parent.internalPointer());

    if (parentItem)
        return parentItem->children().size();
    else return 0;
}

QVariant ProductsGroupsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    DataProductsGroup *item = static_cast<DataProductsGroup *>(index.internalPointer());

    if (index.column() == 0 && mIsCheckable && Qt::CheckStateRole == role)
        return item->isChecked();

    if (role != Qt::DisplayRole)
        return QVariant();

    switch (index.column()) {
    case HEADER_NAME:
        return item->name();
    case HEADER_CODE:
        return item->code();
    case HEADER_NOTE:
        return item->note();
    default:
        break;
    }

    return QVariant();
}

bool ProductsGroupsModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    Q_UNUSED(value);

    if (!index.isValid())
        return false;

    DataProductsGroup *item = static_cast<DataProductsGroup *>(index.internalPointer());

    if (mIsCheckable && Qt::CheckStateRole == role) {
        item->setIsChecked(!item->isChecked());
        item->setIsCheckedChilds(item->isChecked());
        emit checked(item->id(), item->isChecked());
        return true;
    }
    return false;
}

QVariant ProductsGroupsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    QVariant result;

    if (role != Qt::DisplayRole)
        return result;

    if (orientation == Qt::Horizontal)
        switch (section) {
        case HEADER_NAME:
            return tr("Наименование");
        case HEADER_CODE:
            return tr("Код");
        case HEADER_NOTE:
            return tr("Описание");
        }

    return result;
}

QModelIndex ProductsGroupsModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    DataProductsGroup *parentItem;

    if (!parent.isValid())
        parentItem = mRootItem;
    else
        parentItem = static_cast<DataProductsGroup*>(parent.internalPointer());

    DataProductsGroup *childItem;
    if (row < parentItem->children().size())
        childItem = static_cast<DataProductsGroup*> (parentItem->children().at(row));
    else childItem = 0;
    if (childItem)
        return createIndex(row, column, childItem);
    else return QModelIndex();
}

QModelIndex ProductsGroupsModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    DataProductsGroup *childItem = static_cast<DataProductsGroup *>(index.internalPointer());
    DataProductsGroup *parentItem = static_cast<DataProductsGroup *>(childItem->parent());

    if (parentItem == mRootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

bool ProductsGroupsModel::hasChildren(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return true;

    DataProductsGroup *parentItem = static_cast<DataProductsGroup *>(parent.internalPointer());
    return parentItem->children().size();
}

void ProductsGroupsModel::setupModelData()
{
    for (int i = 0; i < listData()->size(); i++)
        listData()->at(i)->setParent(mRootItem);
}

QStringList ProductsGroupsModel::idsChecked() const
{
    QStringList result;
    QStringList childsRes;

    for (int i = 0; i < listData()->size(); i++) {
        if (listData()->at(i)->isChecked())
            result.append(listData()->at(i)->id());

        childsRes = idsCheckedChilds(static_cast <DataProductsGroup *> (listData()->at(i)));
        if (!childsRes.isEmpty())
            result.append(childsRes);
    }
    return result;
}

ListProductsGroups *ProductsGroupsModel::listGroups() const
{
    return reinterpret_cast<ListProductsGroups *> (listData());
}

QModelIndexList ProductsGroupsModel::indexList() const
{
    return persistentIndexList();
}

QModelIndex ProductsGroupsModel::getModelIndexById(const QString &id)
{
    if (id.isEmpty())
        return QModelIndex();

    for (int i = 0; i < persistentIndexList().size(); i++) {
        DataProductsGroup *item = static_cast <DataProductsGroup *>
                (persistentIndexList().at(i).internalPointer());        
        if (item->id() == id)
            return persistentIndexList().at(i);
    }
    return QModelIndex();
}

DataProductsGroup *ProductsGroupsModel::currentGroup(QModelIndex &index) const
{
    DataProductsGroup *item = static_cast<DataProductsGroup *>(index.internalPointer());
    return item;
}

DataProductsGroup *ProductsGroupsModel::findGroup(DataProductsGroup
                                                  *parent, const QString &id) const
{
    for (int i = 0; i < parent->children().size(); i++) {
        DataProductsGroup *item =
                static_cast <DataProductsGroup *> (parent->children().at(i));
        DataProductsGroup *itemChild;
        if (item->id() == id)
            return item;
        itemChild = findGroup(item, id);
        if (itemChild)
            return itemChild;
    }
    return 0;
}

void ProductsGroupsModel::removeItems(DataProductsGroup *parent, QList<QString> &listIds)
{
    for (int i = 0; i < parent->children().size(); i++) {
        DataProductsGroup *item =
                static_cast <DataProductsGroup *> (parent->children().at(i));
        removeItems(item, listIds);
        for (int j = 0; j < listIds.size(); j++) {
            if (listIds.at(j) == item->id()) {
                item->setParent(0);
                delete item;
            }
        }
    }
}

void ProductsGroupsModel::insertGroup(const DataProductsGroup *group)
{
    DataProductsGroup *pGroup = new DataProductsGroup(group);

    pGroup->setIsChecked(false);
    if (pGroup->idParent().isEmpty()) {
        listGroups()->push_back(pGroup);
        pGroup->setRow(listGroups()->size() - 1);
        pGroup->setParent(mRootItem);
    } else {
        DataProductsGroup *parent = findGroup(mRootItem, pGroup->idParent());
        if (parent) {
            pGroup->setParent(parent);
            pGroup->setRow(parent->children().size() - 1);
        }
    }
}

void ProductsGroupsModel::editGroup(const DataProductsGroup *group)
{
    DataProductsGroup *editGroup = findGroup(mRootItem, group->id());
    if (!editGroup)
        return;

    bool isChangeParent = editGroup->idParent() != group->idParent();
    bool isChecked = editGroup->isChecked();
    if (isChangeParent) {
        DataProductsGroup *parent = findGroup(mRootItem, group->idParent());
        if (parent) {
            editGroup->setParent(parent);
            editGroup->setRow(parent->children().size() - 1);
        }        
        parent = findGroup(mRootItem, editGroup->idParent());
        if (!parent)
            parent = mRootItem;
        if (parent) {
            for (int i = 0; i < parent->children().size(); i++) {
                DataProductsGroup *item =
                        static_cast <DataProductsGroup *> (parent->children().at(i));
                item->setRow(i);
            }
        }
    }
    editGroup->copy(group);
    editGroup->setIsChecked(isChecked);    
}

QStringList ProductsGroupsModel::idsCheckedChilds(DataProductsGroup *data) const
{
    QStringList result;
    QStringList childsRes;

    for (int i = 0; i < data->children().size(); i++) {
        DataProductsGroup *item = static_cast <DataProductsGroup *> (data->children().at(i));
        if (item->isChecked())
            result.append(item->id());

        childsRes = idsCheckedChilds(item);
        if (!childsRes.isEmpty())
            result.append(childsRes);
    }
    return result;
}
