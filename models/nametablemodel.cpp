#include "nametablemodel.h"

#include <QDebug>
#include <QThread>
#include <QDir>
#include <QFile>
#include <QMimeData>

#include "apiadapter.h"
#include "seconfig.h"
#include "filewebloader.h"

NameTableModel::NameTableModel(QObject *parent) :
    QAbstractTableModel(parent)
{
    rootItem = nullptr;
    initVariables();
}

NameTableModel::NameTableModel(QObject *parent, ListDataItems *items, const bool &isSelectMode) :
    QAbstractTableModel(parent), rootItem(items->root())
{
    initVariables();
    this->items = items;
    this->isSelectMode = isSelectMode;
}

NameTableModel::~NameTableModel()
{    
    while (!fields.isEmpty())
        delete fields.takeLast();
}

void NameTableModel::initVariables()
{    
    items = nullptr;
    isCheckable = false;
    isSelectMode = false;
    isEditableMode = false;    
    isHandleEmtyIdItem = false;
    sizeImage.setWidth(SEConsts::IMAGE_SIZE_PREVIEW);
    sizeImage.setHeight(SEConsts::IMAGE_SIZE_PREVIEW);
    fields.append(new Field("name", "Наименование"));
}

void NameTableModel::downloadImage(const QString &urlImage, const QModelIndex &modelIndex) const
{
    QThread* thread = new QThread;
    FileWebLoader *loader = new FileWebLoader(urlImage, modelIndex);
    loader->moveToThread(thread);
    connect(thread, &QThread::started, loader, &FileWebLoader::process);
    connect(loader, &FileWebLoader::finishedDownloadImage, thread, &QThread::quit);
    connect(loader, &FileWebLoader::finishedDownloadImage, this, &NameTableModel::finishDownloadImage);
    connect(loader, &FileWebLoader::finishedDownloadImage, loader, &QObject::deleteLater);
    connect(thread, &QThread::finished, thread, &QObject::deleteLater);
    thread->start();
}

QStringList NameTableModel::idsCheckedChilds(DataItem *data) const
{
    QStringList result;
    QStringList childsRes;

    for (int i = 0; i < data->children().size(); i++) {
        DataItem *item = static_cast <DataItem *> (data->children().at(i));
        if (item->getIsChecked()) {
            if (!result.contains(item->getId()))
                result.append(item->getId());
            for (int j = 0; j < item->getIdsChilds().size(); ++j) {
                if (!result.contains(item->getIdsChilds().at(j)))
                    result.append(item->getIdsChilds().at(j));
            }
        } else result.removeOne(item->getId());

        childsRes = idsCheckedChilds(item);
        if (!childsRes.isEmpty())
            result.append(childsRes);
    }
    return result;
}

const DataItem *NameTableModel::findItem(const DataItem *parent, const QString &id) const
{
    for (int i = 0; i < parent->children().size(); i++) {
        const DataItem *item = static_cast <DataItem *> (parent->children().at(i));
        if (item->getId() == id)
            return item;

        const DataItem *itemChild = findItem(item, id);
        if (itemChild)
            return itemChild;
    }
    return 0;
}

void NameTableModel::unselectedChilds(QModelIndexList *list)
{
    for (int i = list->size() - 1; i >= 0; --i)
        if (list->at(i).parent().isValid() && existParentInSelectedList(list->at(i).parent(), *list))
            list->removeAt(i);
}

bool NameTableModel::existParentInSelectedList(const QModelIndex &parent, const QModelIndexList &list)
{
    if (!parent.isValid())
        return false;

    return list.contains(parent) || existParentInSelectedList(parent.parent(), list);
}

int NameTableModel::rowCount(const QModelIndex &parent) const
{       
    if (!items)
        return 0;

    const DataItem *parentItem;

    if (!parent.isValid())
        parentItem = rootItem;
    else parentItem = static_cast<DataItem *>(parent.internalPointer());

    if (parentItem)
        return parentItem->children().size();
    else return 0;
}

int NameTableModel::rowCount() const
{
    if (items)
        return items->size();
    return 0;
}

int NameTableModel::columnCount(const QModelIndex &) const
{  
    return fields.size();
}

int NameTableModel::columnCount() const
{
    return fields.size();
}

bool NameTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return false;

    if (items && isCheckable && Qt::CheckStateRole == role) {
        DataItem *item = dataItem(index);
        if (item) {
            item->setIsChecked(!item->getIsChecked());
            item->setIsCheckedChilds(item->getIsChecked());
            emit checked(item->getId(), item->getIsChecked());
        }
        return true;
    }

    if (items && role == Qt::DecorationRole) {
        emit dataChanged(index, index);
        return true;
    }

    if (isEditableMode && index.isValid() && role == Qt::EditRole) {
        DataItem *item = static_cast<DataItem *>(index.internalPointer());
        bool result = item->setEditDataByPropertyName(fields.at(index.column())->name(), value);
        if (result)
            emit dataChanged(index, index);
        return result;
    }

    return false;
}

Qt::ItemFlags NameTableModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
            return 0;

    Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    flags |= Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;

    if (index.isValid() && index.column() >= 0 && !isSelectMode && isEditableMode) {
        DataItem *item = static_cast<DataItem *>(index.internalPointer());
        if (item->getIsEditableByPropertyName(fields.at(index.column())->name()))
            return flags |=  Qt::ItemIsEditable;
    }
    if (index.isValid() && isHandleEmtyIdItem) {
         DataItem *item = static_cast<DataItem *>(index.internalPointer());
         if (item->getId().isEmpty())
             flags = Qt::ItemIsEnabled;
    }

    return isCheckable && !isSelectMode ?
                flags | Qt::ItemIsUserCheckable : flags;
}

QVariant NameTableModel::data(const QModelIndex &index, int role) const
{
    QVariant result;

    if (!index.isValid())
        return result;

    if (!items)
        return result;

    DataItem *item = static_cast<DataItem *>(index.internalPointer());

    if (role == Qt::UserRole)
        return item->getId();

    if (isCheckable && Qt::CheckStateRole == role && index.column() == 0)
        return item->getIsChecked();

    if (role == Qt::TextAlignmentRole && index.column() < fields.size())
        return fields.at(index.column())->align();

    if (role == Qt::SizeHintRole && (fields.at(index.column())->name() == "image"))
        result = QSize(SEConsts::IMAGE_SIZE_PREVIEW + 2, SEConsts::IMAGE_SIZE_PREVIEW + 2);

    if (role == Qt::DecorationRole && (fields.at(index.column())->name() == "image"))
        result = getImage(*item, index);

    if (role == Qt::BackgroundRole) {
         QColor color = item->getColorByPropertyName(fields.at(index.column())->name());
       if (color.isValid())
         return color;
    }

    if (role == Qt::DisplayRole && index.column() < fields.size() &&
            fields.at(index.column())->role() == role &&
            fields.at(index.column())->name() != "image")
        return item->getPropertyDisplay(fields.at(index.column())->name());
    if (role == Qt::DecorationRole && index.column() < fields.size() &&
            fields.at(index.column())->name() != "image") {
        if (fields.at(index.column())->role() == role)
            return item->getPropertyDisplay(fields.at(index.column())->name());
        else return item->getPropertyDecoration(fields.at(index.column())->name());
    }

    if (role == Qt::EditRole && index.column() < fields.size() &&
            fields.at(index.column())->name() != "image")
        return item->getPropertyEdit(fields.at(index.column())->name());

    if ((role == Qt::FontRole) && isHandleEmtyIdItem && item->getId().isEmpty()) {
        return QVariant::fromValue(QFont("Segoe UI", 9, QFont::Bold));

    }

    return result;
}

QVariant NameTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::TextAlignmentRole && section < fields.size())
        return fields.at(section)->align();

    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal && section < fields.size())
        return fields.at(section)->title();

    return QVariant();
}

bool NameTableModel::hasChildren(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return true;

    DataItem *parentItem = static_cast<DataItem* > (parent.internalPointer());    
    return parentItem->children().size();
}

Qt::DropActions NameTableModel::supportedDropActions() const
{
    return Qt::MoveAction;
}

QMimeData *NameTableModel::mimeData(const QModelIndexList &indexes) const
{
    QMimeData *mimeData = new QMimeData();
    QByteArray encodedData;

    QDataStream stream(&encodedData, QIODevice::WriteOnly);

    foreach (const QModelIndex &index, indexes) {
        if (index.isValid() && index.column() == 0) {
            QString text = dataItem(index)->getId();
            stream << text;
        }
    }

    mimeData->setData("application/vnd.text.list", encodedData);
    return mimeData;
}

QStringList NameTableModel::mimeTypes() const
{
    QStringList types;
    types << "application/vnd.text.list";
    return types;
}

bool NameTableModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int , int column,
                                  const QModelIndex &parent)
{
    if (action == Qt::IgnoreAction)
        return true;
    if (!data->hasFormat("application/vnd.text.list"))
        return false;
    if (column > 0)
        return false;

    const DataItem *parentItem;
    if (!parent.isValid())
        parentItem = rootItem;
    else parentItem = static_cast<DataItem* >(parent.internalPointer());

    QByteArray encodedData = data->data("application/vnd.text.list");
    QDataStream stream(&encodedData, QIODevice::ReadOnly);
    QStringList newItems;
    int rows = 0;
    int beginRow = parentItem->children().size();
    while (!stream.atEnd()) {
        QString text;
        stream >> text;
        newItems << text;
        ++rows;
    }
    if (parent.isValid())
        beginInsertRows(parent, beginRow, beginRow + rows - 1);
    else beginInsertRows(QModelIndex(), beginRow, beginRow + rows - 1);
    foreach (const QString &id, newItems) {
        for (int i = 0; i < items->size(); ++i) {
            if (items->at(i)->getId() == id) {
                if (parent.isValid()) {
                    items->at(i)->setIdParent(parentItem->getId());
                    items->at(i)->setParent(static_cast<DataItem* >(parent.internalPointer()));
                } else items->setRootParent(items->at(i));
                break;
            }
        }
        beginRow++;
    }
    endInsertRows();    
    return true;
}

bool NameTableModel::removeRows(int row, int count, const QModelIndex &parent)
{
    beginRemoveRows(parent, row, row + count - 1) ;
    endRemoveRows();
    return true;
}

QModelIndex NameTableModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    const DataItem *parentItem;
    if (!parent.isValid())
        parentItem = rootItem;
    else parentItem = static_cast<DataItem* >(parent.internalPointer());

    DataItem *childItem;
    if (row < parentItem->children().size())
        childItem = static_cast<DataItem* > (parentItem->children().at(row));
    else childItem = 0;
    if (childItem)
        return createIndex(row, column, childItem);
    else return QModelIndex();
}

QModelIndex NameTableModel::parent(const QModelIndex &index) const
{
    if (!index.isValid() || !items)
        return QModelIndex();

    DataItem *childItem = static_cast<DataItem *>(index.internalPointer());
    DataItem *parentItem = 0;
    if (childItem)
        parentItem = static_cast<DataItem *>(childItem->parent());

    if (!parentItem || parentItem == rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

QString NameTableModel::idData(const QModelIndex &index) const
{
    DataItem *item = dataItem(index);
    if (item)
        return item->getId();
    return QString();
}

QString NameTableModel::nameData(const QModelIndex &index) const
{
    DataItem *item = dataItem(index);
    if (item)
        return item->getName();
    return QString();
}

DataItem *NameTableModel::dataItem(const QModelIndex &index) const
{
    DataItem *item = static_cast<DataItem *>(index.internalPointer());
    return item;
}

QModelIndex NameTableModel::getIndexByName(const QString &name) const
{
    if (!items)
        return QModelIndex();

    for (int i = 0; i < items->size(); i++) {
        DataItem *item = items->at(i);
        if (item->getName().compare(name, Qt::CaseInsensitive) == 0) {
            return index(i, 0);
        }
    }
    return QModelIndex();
}

QModelIndexList NameTableModel::indexList() const
{     
    return persistentIndexList();
}

const QModelIndex NameTableModel::getModelIndexById(const QString &id)
{
    if (id.isEmpty())
        return QModelIndex();

    for (int i = 0; i < items->size(); i++) {
        DataItem *item = items->at(i);
        if (item->getId() == id) {
            if (item->parent() == rootItem)
                return index(item->row(), 0);
            return index(item->row(), 0, getModelIndexById(dynamic_cast <DataItem *> (item->parent())->getId()));
        }
    }

    return QModelIndex();
}

void NameTableModel::addItem(DataItem *item, const bool &isLast)
{
    QModelIndex parentIndex = getModelIndexById(item->getIdParent());
    DataItem *itemParent = dataItem(parentIndex);
    if (isLast) {
        if (itemParent)
            beginInsertRows(parentIndex, itemParent->children().size(), itemParent->children().size());
        else beginInsertRows(parentIndex, rootItem->children().size(), rootItem->children().size());
        items->append(item);
    } else {
        if (itemParent)
            beginInsertRows(parentIndex, 0, 0);
        else beginInsertRows(parentIndex, 0, 0);
        items->prepend(item);
    }

    for (int i = 0; i < items->size(); ++i)
        if (!item->getIdParent().isEmpty() && items->at(i)->getId() == item->getIdParent()) {
            item->setParent(items->at(i));
            break;
        }
    endInsertRows();
}

void NameTableModel::moveItem(const QModelIndex &sourceIndex, DataItem *distItem)
{
    QModelIndex parent = getModelIndexById(distItem->getIdParent());
    DataItem *itemParent = dataItem(parent);
    if (itemParent) {
        beginMoveRows(sourceIndex.parent(), sourceIndex.row(), sourceIndex.row(), parent, itemParent->children().size());
        distItem->setParent(itemParent);
        moveRow(sourceIndex.parent(), sourceIndex.row(), parent, itemParent->children().size());
    }
    else {
        beginMoveRows(sourceIndex.parent(), sourceIndex.row(), sourceIndex.row(), parent, rootItem->children().size());
        items->setRootParent(distItem);
        moveRow(sourceIndex.parent(), sourceIndex.row(), parent, rootItem->children().size());
    }
    endMoveRows();
}

void NameTableModel::deleteItem(DataItem *item)
{
    QModelIndex parent = getModelIndexById(item->getIdParent());
    beginRemoveRows(parent, item->row(), item->row());
    items->deleteItem(item);
    endRemoveRows();
}

void NameTableModel::setImageSize(const QSize &sizeImage)
{
    if (!sizeImage.isNull() && sizeImage.width() > 16)
        this->sizeImage = sizeImage;
}

void NameTableModel::setEditableMode(const bool &isOn)
{
    isEditableMode = isOn;
}

void NameTableModel::setTitleHeader(const int &index, const QString &title)
{
    if (fields.size() > index) {
        fields.at(index)->setTitle(title);
        resetModel();
    }
}

void NameTableModel::setIsHandleEmtyIditem(const bool isHandle)
{
    isHandleEmtyIdItem = isHandle;
}

QStringList NameTableModel::idsChecked() const
{
    QStringList result;
    QStringList childsRes;

    for (int i = 0; i < getItems()->size(); i++) {
        if (getItems()->at(i)->getIsChecked()) {
            if (!result.contains(getItems()->at(i)->getId()))
                result.append(getItems()->at(i)->getId());
            for (int j = 0; j < getItems()->at(i)->getIdsChilds().size(); ++j) {
                if (!result.contains(getItems()->at(i)->getIdsChilds().at(j)))
                    result.append(getItems()->at(i)->getIdsChilds().at(j));
            }
        }

        childsRes = idsCheckedChilds(static_cast <DataItem *> (getItems()->at(i)));
        if (!childsRes.isEmpty())
            result.append(childsRes);
    }
    result.removeDuplicates();
    return result;
}

Request NameTableModel::getSortParamsForApi()
{
    Request result;
    if (!items)
        return result;

    for (int i = 0; i < items->size(); i++)
        result.addSortIndex(items->at(i)->getId(), items->at(i)->row());

    return result;
}

void NameTableModel::setListData(ListDataItems *items)
{
    beginResetModel(); 
    this->items = items;
    rootItem = items->root();
    endResetModel();
}

ListDataItems *NameTableModel::getItems() const
{
    return items;
}

void NameTableModel::appendField(Field *field)
{
    fields.append(field);
}

void NameTableModel::prependField(Field *field)
{
    fields.prepend(field);
}

void NameTableModel::appendFields(const QVector<Field *> *fields)
{
    clearFields();
    for (int i = 0; i < fields->size(); i++)
        appendField(fields->at(i));
}

void NameTableModel::clearFields()
{
    while (!fields.isEmpty())
        delete fields.takeLast();
}

void NameTableModel::setCheckedMode(const bool &isCheckable)
{
    this->isCheckable = isCheckable;
}

void NameTableModel::beginReset()
{
    beginResetModel();
}

void NameTableModel::endReset()
{
    endResetModel();
}

void NameTableModel::unBindItems()
{    
    items = nullptr;
}

void NameTableModel::deleteItems(QModelIndexList &list)
{    
    unselectedChilds(&list);
    for (int i = list.size() - 1; i >= 0; --i) {
        QModelIndex index = list.at(i);
        DataItem *item = static_cast<DataItem *>(index.internalPointer());
        beginRemoveRows(index.parent(), index.row(), index.row());
        items->deleteItem(item);
        endRemoveRows();
    }
    items->setModified();
}

const QModelIndexList NameTableModel::getPersistentIndexList() const
{
    return persistentIndexList();
}

const QVector<Field *> NameTableModel::getFields() const
{
    return fields;
}

void NameTableModel::resetModel()
{
    beginReset();    
    endReset();
}

void NameTableModel::finishDownloadImage(const QString &fileImage, const QModelIndex &modelIndex)
{
    if (fileImage.isEmpty())
        return;

    setData(modelIndex, fileImage, Qt::DecorationRole);
}

const QPixmap NameTableModel::getImage(const DataItem &item, const QModelIndex &index) const
{
    if (!item.getPixmap()->isNull())
        return *item.getPixmap();    
    else {
        if (item.property("imageUrlPreview").isNull())
            return QPixmap();

        QString fileName(QMD5(item.property("imageUrlPreview").toString()));
        QString filePath = SEConfig::getFolderCache() + QDir::separator() + fileName;
        QFile file(filePath);
        if (file.exists()) {
            QPixmap ico(filePath);
            if (!ico.isNull()) {
                *item.getPixmap() = ico.scaled(sizeImage, Qt::KeepAspectRatio);
                return *item.getPixmap();
            }
        } else {
            downloadImage(item.property("imageUrlPreview").toString(), index);
            return QPixmap(":/128/icons/128x128/no-image.png").scaled(sizeImage);
        }
    }
    return QPixmap();
}

