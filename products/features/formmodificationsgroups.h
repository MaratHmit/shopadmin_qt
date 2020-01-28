#ifndef FORMMODIFICATIONSGROUPS_H
#define FORMMODIFICATIONSGROUPS_H

#include <QDialog>

#include "nametablemodel.h"
#include "formmodificationgroupcard.h"
#include "formfeatures.h"
#include "datamodificationgroup.h"

namespace Ui {
class FormModificationsGroups;
}

class FormModificationsGroups : public QDialog
{
    Q_OBJECT

public:
    FormModificationsGroups(QWidget *parent = 0);
    FormModificationsGroups(QWidget *parent, const bool &isDialog);
    ~FormModificationsGroups();

    void setExludingGroups(const ListModificationsGroups *groups);

    const ListModificationsGroups *selectedGroups();

public slots:
    void refreshData();

protected:
    void showEvent(QShowEvent *e);
    void hideEvent(QHideEvent *e);
    void timerEvent(QTimerEvent *event);        

private slots:
    void onAddEditGroup();
    void onDeleteGroup();
    void onUpDownGroup();
    void onChangeGroups();    
    void onLoadFromCsv();
    void saveCurrentGroup();

private:
    Ui::FormModificationsGroups *ui;
    int mIdTimer;
    int mTypeTimer;
    QString mTypePriceGroupDef;
    QString mNameColumnDef;
    bool mIsLoadedLayouts;
    bool mIsSelectMode;
    NameTableModel *mModel;
    ListDataItems *mListPricesTypes;    
    ListModificationsGroups *mGroups;    
    ListModificationsGroups *mExludingGroups;
    ListModificationsGroups *mSelectedGroups;

    bool isAddPermission;
    bool isEditPermission;
    bool isDeletePermission;

    void initialization();
    void initSignals();
    void initActions();    
    void initPermissions();
    void goToRowById(QString id);
    void initTreeViewGroups();
    void saveSortGroups();
    void refreshValues();
    void importFromCsv(const QString &fileName);
};

#endif // FORMMODIFICATIONSGROUPS_H
