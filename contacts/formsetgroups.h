#ifndef FORMSETGROUPS_H
#define FORMSETGROUPS_H

#include <QDialog>

#include "datacontact.h"
#include "nametablemodel.h"

namespace Ui {
class FormSetGroups;
}

class FormSetGroups : public QDialog
{
    Q_OBJECT

public:
    explicit FormSetGroups(QWidget *parent, ListContacts *contacts, int typeContacts);
    ~FormSetGroups();

private:
    void showEvent(QShowEvent *e);

    Ui::FormSetGroups *ui;
    ListContacts *contacts;
    ListDataItems *groups;
    NameTableModel *modelGroups;
    int typeContacts;

    void initListView();
    void initSignals();
    void loadGroups();
    void fillGroupsChecked(QVector<QString> *groupsChecked);
    void onOK();
};

#endif // FORMSETGROUPS_H
