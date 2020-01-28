#ifndef FORMPRODUCTSGROUPS_H
#define FORMPRODUCTSGROUPS_H

#include <QWidget>
#include <QDialog>
#include <QTreeWidgetItem>

#include "api.h"
#include "formproductgroupcard.h"
#include "nametablemodel.h"
#include "field.h"
#include "formsetvisibleheadercolumns.h"
#include "dialogincprices.h"

namespace Ui {
class FormProductsGroups;
}

class FormProductsGroups : public QDialog
{
    Q_OBJECT

public:
    explicit FormProductsGroups(QWidget *parent = 0);
    ~FormProductsGroups();

    QStringList getIdsGroupsChecked();
    void setSelectMode(const bool &isMultiSelect);
    void setSimpleMode(const bool &isGroupMode);
    void setExludingGroups(const ListProductsGroups *groups);
    const DataProductsGroup *checkedGroup() const;
    void setVisibleCheckeds(const bool &isVisible);

    QList<DataProductsGroup *> listSelected();

public slots:
    void refreshData(const QString &idParent = QString(), const QModelIndex &index = QModelIndex());

signals:
    void onItemTreeChanged();

protected:
    void showEvent(QShowEvent *);
    void hideEvent(QHideEvent *);
    void timerEvent(QTimerEvent *);  
    bool eventFilter(QObject *, QEvent *);

private slots:    
    void showContextMenu(QPoint point);
    void finishedRefreshData(const bool status, const QString message);

    void onEditGroup();
    void onDeleteGroups();
    void on_treeViewGroups_doubleClicked(const QModelIndex &index);
    void onSelectGroups();
    void onItemChecked();
    void onChangeGroup();
    void onAddGroupProduct();
    void onAddSubGroupProduct();    
    void onSortGroups();
    void onExpanded(const QModelIndex &index);
    void onMoved(const QModelIndex &parent, int first, int last);
    void onSetIncPrices();
    void onChangeParentGroup();
    void onClearParentGroup();

    void importData();
    void exportData();

private:
    Ui::FormProductsGroups *ui;    
    int mIdTimer;
    bool mSelectMode;
    bool mIsShowed;
    bool mIsRestoreStates;    
    QList<DataProductsGroup *> mListSelected;
    ListProductsGroups *mGroups;
    ListProductsGroups *mExludingGroups;
    NameTableModel *mModel;
    QVector<Field *> mFields;
    Request mRequest;
    FormSetVisibleHeaderColumns *mFormSetColumns;
    Api *mApi;
    QStringList idsCheckedsStore;

    void selectAll();

    void resetRequest(const QString &idParent = QString());
    void setContextMenu();
    void initActions();
    void initSignals();
    void initFields();
    void initWidgetGroups();            
    void initPermissions();
    void saveSortGroups();
    void expandCollapseTree();
    void checkedUncheckedTree();
    void setGroups(const ListProductsGroups &groups, const QModelIndex &index);
    void setVisibleColumns();
    void storeCheckedItems();
    void restoreCheckedItems();

    void setCodeURL();

    void goToRowById(const QString &id);

    QTreeWidgetItem *getTreeItemByIdInChilds(const int &id,
                                             const QTreeWidgetItem &itemParent);
};

#endif // FORMPRODUCTSGROUPS_H
