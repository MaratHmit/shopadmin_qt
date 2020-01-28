#ifndef FORMCONTACTSGROUPS_H
#define FORMCONTACTSGROUPS_H

#include <QWidget>
#include <QTimerEvent>
#include <QTableView>

#include "api.h"
#include "nametablemodel.h"

namespace Ui {
class FormContactsGroups;
}

class FormContactsGroups : public QWidget
{
    Q_OBJECT

public:
    explicit FormContactsGroups(QWidget *parent = 0);
    ~FormContactsGroups();    

    NameTableModel *groupsModel() const;
    QStringList idCheckedGroups() const;

public slots:
    void refreshData(QString id = QString());

private slots:
    void onAddEditGroup();
    void onDeleteGroups();
    void onChangeGroup();
    void onCheckedGroups();
    void onCopyCellText();
    void showContextMenu(QPoint point);
    void onSave(const DataItem *group, const bool &isNew);

signals:
    void checkedGroups();

private:
    Ui::FormContactsGroups *ui;

    bool isShown;
    ListDataItems *groups;
    NameTableModel *modelGroups;

    bool goToRowById(const QString &id);
    void initActions();
    void initTreeGroups();
    void initSignals();
    void initPermission();
    void showEvent(QShowEvent *event);    
};

#endif // FORMCONTACTSGROUPS_H
