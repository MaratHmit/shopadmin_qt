#include "contactsgroupstablemodel.h"

ContactsGroupsTableModel::ContactsGroupsTableModel(QObject *parent) :
    NameTableModel(parent)
{

}

ListDataItems *ContactsGroupsTableModel::listGroups() const
{
    return listData();
}

QString ContactsGroupsTableModel::idGroupsSelected(const QModelIndexList list) const
{
    QString result;

    for (int i = 0; i < list.size(); i++) {
        if (!result.isEmpty())
            result += ",";
        result += idData(list.at(i).row());
    }
    return result;
}

DataItem *ContactsGroupsTableModel::group(const int &row) const
{    
    if (row < 0)
        return 0;
    else return listData()->at(row);
}

void ContactsGroupsTableModel::updateItem(const QString &id, const QString &name)
{
    for (int i = 0; i < listGroups()->size(); i++) {
        DataItem *gr = listGroups()->at(i);
        if (gr->id() == id) {
            gr->setName(name);
            return;
        }
    }
}

void ContactsGroupsTableModel::copyData(ListDataItems *list)
{
    listGroups()->clear();
    for (int i = 0; i < list->size(); i++)
        listGroups()->push_back(list->at(i));
}

void ContactsGroupsTableModel::setCheckedsGroups(const QVector<QString> *groups)
{
    for (int i = 0; i < listGroups()->size(); i++) {
        DataItem *gr = listGroups()->at(i);
        gr->setIsChecked(groups->contains(gr->id()));
    }
}

void ContactsGroupsTableModel::getCheckedsGroups(QVector<QString> *groups)
{
    groups->clear();
    for (int i = 0; i < listGroups()->size(); i++) {
        DataItem *gr = listGroups()->at(i);
        if (gr->isChecked())
            groups->append(gr->id());
    }
}

void ContactsGroupsTableModel::addData(DataItem *group)
{
    listGroups()->push_back(group);
}


