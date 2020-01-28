#ifndef CONTACTSGROUPSTABLEMODEL_H
#define CONTACTSGROUPSTABLEMODEL_H

#include <nametablemodel.h>
#include <datacontact.h>

class ContactsGroupsTableModel : public NameTableModel
{
    Q_OBJECT
public:
    explicit ContactsGroupsTableModel(QObject *parent = 0);    

    ListDataItems *listGroups() const;    

    QString idGroupsSelected(const QModelIndexList list) const;        

    DataItem *group(const int &row) const;
    void updateItem(const QString &id, const QString &title);

    void copyData(ListDataItems *list);
    void setCheckedsGroups(const QVector<QString>* groups);
    void getCheckedsGroups(QVector<QString>* groups);
    void addData(DataItem *group);

public slots:

};

#endif // CONTACTSGROUPSTABLEMODEL_H
