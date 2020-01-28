#ifndef FORMNEWSGROUPS_H
#define FORMNEWSGROUPS_H

#include <QWidget>
#include <QDialog>
#include <QTreeWidgetItem>

#include "nametablemodel.h"
#include "preloader.h"

namespace Ui {
class FormNewsGroups;
}

class FormNewsGroups : public QDialog
{
    Q_OBJECT

public:
    explicit FormNewsGroups(QWidget *parent = 0);
    ~FormNewsGroups();

    QStringList getIdsGroupsChecked();
    void setSelectMode(const bool &isMultiSelect);
    void setSimpleMode(const bool &isGroupMode);
    QList<DataItem *> listSelected();

public slots:
    void refreshData();

signals:
    void onItemTreeChanged();

protected:
    void showEvent(QShowEvent *e);
    void hideEvent(QHideEvent *e);    

private slots:
    void insertRecord(const DataItem *group);
    void editRecord(const DataItem *group);
    void showContextMenu(QPoint point);

    void onEditGroup();
    void onDeleteGroups();
    void on_treeViewGroupsNews_doubleClicked(const QModelIndex &index);
    void onSelectGroups();
    void onItemChecked();
    void onChangeGroup();
    void onAddGroup();
    void onAddSubGroup();

private:
    Ui::FormNewsGroups *ui;    
    bool mSelectMode;
    bool mIsShowed;
    QList<DataItem *> mListSelected;
    ListDataItems *mGroups;
    NameTableModel *mModel;
    Preloader *mPreloader;

    bool isAddPermission;
    bool isEditPermission;
    bool isDeletePermission;

    void setContextMenu();
    void initActions();
    void initSignals();
    void initWidgetGroups();        
    void initPermissions();
    void recordAddEdit(const DataItem *group, const bool &isNew);

    QTreeWidgetItem *getTreeItemByIdInChilds(const int &id,
                                             const QTreeWidgetItem &itemParent);
};

#endif // FORMPRODUCTSGROUPS_H
