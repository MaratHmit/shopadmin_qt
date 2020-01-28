#include "integrationsmodel.h"

IntegrationsModel::IntegrationsModel(QObject *parent, ListDataItems *parameters) :
    QAbstractTableModel(parent)
{
    mIsParametersOnOffMode = false;
    mIsFeaturesMode = false;
    mParameters = parameters;
    mRootItem = parameters->root();
}

IntegrationsModel::~IntegrationsModel()
{

}

void IntegrationsModel::setListData(ListDataItems *parameters)
{
    mParameters = parameters;
}

void IntegrationsModel::setIsParametersOnOffMode()
{
    mIsParametersOnOffMode = true;
}

void IntegrationsModel::setIsFeaturesMode()
{
    mIsFeaturesMode = true;
}

int IntegrationsModel::columnCount(const QModelIndex &) const
{
    if (mIsParametersOnOffMode)
        return 3;
    else return 2;
}

int IntegrationsModel::rowCount(const QModelIndex &) const
{
    return mParameters->size();
}

QVariant IntegrationsModel::data(const QModelIndex &index, int role) const
{
    QVariant result;

    if (!index.isValid() || !mParameters)
        return result;

    DataItem *item = static_cast<DataItem *>(index.internalPointer());
    int indexColumn = index.column();
    if (mIsParametersOnOffMode) {
        switch (indexColumn) {
        case 1:
            indexColumn = 2;
            break;
        case 2:
            indexColumn = 1;
            break;
        default:
            break;
        }
    }

    if (role == Qt::BackgroundRole) {
         if (indexColumn != HEADER_VALUE)
            return QColor("#F0F0F0");
    }

    if (role == Qt::TextAlignmentRole) {
        switch (indexColumn) {
        case HEADER_NAME:
             return (int) Qt::AlignLeft | Qt::AlignVCenter;
        case HEADER_VALUE:
            if (item->getValueType() == "S")
                return (int) Qt::AlignLeft | Qt::AlignVCenter;
            if (item->getValueType() == "P")
                return (int) Qt::AlignLeft | Qt::AlignVCenter;
            if (item->getValueType() == "I")
                return (int) Qt::AlignLeft | Qt::AlignVCenter;
            if (item->getValueType() == "D")
                return (int) Qt::AlignLeft | Qt::AlignVCenter;
            if (item->getValueType() == "B")
                return (int) Qt::AlignLeft | Qt::AlignVCenter;
            if (item->getValueType() == "L")
                return (int) Qt::AlignLeft | Qt::AlignVCenter;
            if (item->getValueType() == "CL")
                return (int) Qt::AlignLeft | Qt::AlignVCenter;
        }
    }

    if (role == Qt::DisplayRole) {
        switch (indexColumn) {
        case HEADER_NAME:
            return item->getName();        
        case HEADER_VALUE:
            if (item->getValueType() == "S")
                return item->getValue();
            if (item->getValueType() == "P")
                return "******************";
            if (item->getValueType() == "I")
                return item->getValue().toInt();
            if (item->getValueType() == "D")
                return QString::number(item->getValue().toDouble(), 'f', 2);
            if (item->getValueType() == "L")
                return item->getValue();
            if (item->getValueType() == "B")
                return (bool) item->getValue().toInt();
            if (item->getValueType() == "CL")
                return item->getValue();
        case HEADER_ISACTIVE:
            return item->getIsActive();
        }
    }

    if (role == Qt::EditRole && indexColumn == HEADER_VALUE) {
        if (item->getValueType() == "S")
            return item->getValue();
        if (item->getValueType() == "P")
            return item->getValue();
        if (item->getValueType() == "I")
            return item->getValue().toInt();
        if (item->getValueType() == "D")
            return item->getValue().toDouble();
        if (item->getValueType() == "B")
            return (bool) item->getValue().toInt();        
    }

    return result;
}

QVariant IntegrationsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    QVariant result;

    if (role == Qt::TextAlignmentRole)
        return Qt::AlignLeft;

    if (role != Qt::DisplayRole)
        return result;

    if (mIsParametersOnOffMode) {
        switch (section) {
        case 1:
            section = 2;
            break;
        case 2:
            section = 1;
            break;
        default:
            break;
        }
    }

    if (orientation == Qt::Horizontal) {
        switch (section) {        
        case HEADER_NAME:
            return tr("Наименование параметра");
        case HEADER_VALUE:
            return tr("Значение");        
        case HEADER_ISACTIVE:
            return tr("Включен");
        }
    }

    return result;
}

bool IntegrationsModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return false;

    int indexColumn = index.column();
    if (mIsParametersOnOffMode) {
        switch (indexColumn) {
        case 1:
            indexColumn = 2;
            break;
        case 2:
            indexColumn = 1;
            break;
        default:
            break;
        }
    }

    if (role == Qt::EditRole && indexColumn == HEADER_VALUE) {
        DataItem *item = mParameters->at(index.row());
        QString valueData = value.toString();
        if (item->getValueSize() > 0 && valueData.length() > item->getValueSize())
            valueData = valueData.remove(item->getValueSize(), valueData.length() - item->getValueSize());
        if (item->getValueType() == "B") {
            if (value.toBool())
                valueData = "1";
            else valueData.clear();
        }
        item->setValue(valueData);
        emit modified();
        return true;
    }

    return false;
}

Qt::ItemFlags IntegrationsModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
            return Qt::ItemIsEnabled;

    Qt::ItemFlags flags = QAbstractItemModel::flags(index);

    int indexColumn = index.column();
    if (mIsParametersOnOffMode) {
        switch (indexColumn) {
        case 1:
            indexColumn = 2;
            break;
        case 2:
            indexColumn = 1;
            break;
        default:
            break;
        }
    }

    if (indexColumn == HEADER_VALUE)
        return flags |=  Qt::ItemIsEditable;

    return flags;
}

QModelIndex IntegrationsModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    const DataItem *parentItem;
    if (!parent.isValid())
        parentItem = mRootItem;
    else parentItem = static_cast<DataItem* >(parent.internalPointer());

    DataItem *childItem;
    if (row < parentItem->children().size())
        childItem = static_cast<DataItem* > (parentItem->children().at(row));
    else childItem = 0;
    if (childItem)
        return createIndex(row, column, childItem);
    else return QModelIndex();
}

DataItem *IntegrationsModel::dataItem(const QModelIndex &index) const
{
    DataItem *item = static_cast<DataItem *>(index.internalPointer());
    return item;
}

void IntegrationsModel::resetModel()
{
    beginResetModel();
    endResetModel();
}

void IntegrationsModel::beginReset()
{
    beginResetModel();
}

void IntegrationsModel::endReset()
{
    endResetModel();
}

void IntegrationsModel::repaintModel()
{
    emit layoutChanged();
}

