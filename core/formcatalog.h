#ifndef FORMCATALOG_H
#define FORMCATALOG_H

#include <QDialog>
#include <QTreeView>
#include <QToolButton>
#include <QPushButton>
#include <QAction>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLine>
#include <QMenu>
#include <QTimer>

#include "seconsts.h"
#include "seconfig.h"
#include "dataitem.h"
#include "nametablemodel.h"
#include "field.h"
#include "request.h"
#include "apiadapter.h"
#include "preloader.h"
#include "framepagenavigator.h"
#include "framesearchtable.h"
#include "dialogcard.h"

class FormCatalog : public QDialog
{
    Q_OBJECT

public:
    FormCatalog(QWidget *parent, const QString &apiObject, const QString &permissionObject = QString());
    FormCatalog(QWidget *parent, ListDataItems *items);
    ~FormCatalog();    

    void addField(Field *field);    
    void setSelectMode(const bool &isMultiSelect = false);
    void setTitleButtonAdd(const QString &title, const QString &toolTip);
    void setVisiblePaginator(const bool &visible);
    void setVisibleSearch(const bool &visible);
    void setRootIsDecorated(const bool &isDecorated);
    void setSortingEnabled(const bool &isEnabled);
    void setIsCheckedMode(const bool &isCheckedMode);
    void setDialogCard(DialogCard *dialogCard);
    void setRefreshModeAfterChange();

    NameTableModel *getModelItems() const;

signals:
    void checked(const QStringList &ids);
    void modified();

public slots:
    void refreshData();
    void refreshCurrentInfo();

    void createItem();
    void editItem();
    void deleteItem();

protected:    
    void initActions();
    void initButtons();
    void initControls();
    void initVariables();
    void initSignals();
    void initTreeView();
    virtual void resetRequest();

private slots:
    void finishedRefreshList(const bool status, const QString message);
    void finishedRefreshCurrentInfo(const bool status, const QString message);

    void currentItemChaned();
    void runSearch();
    void onCheckedItems();

    void goNextPage();
    void goPreviosPage();
    void goLastPage();
    void goFirstPage();

    void showContextMenu(QPoint point);

private:
    void showEvent(QShowEvent *);
    void hideEvent(QHideEvent *);    

    QTreeView *treeViewItems;
    NameTableModel *modelItems;
    ListDataItems *items;
    Api *apiFetch;
    Api *apiInfo;
    Preloader *preloader;
    Request request;
    QString apiObject;
    QString permissionObject;
    int countItems;
    int numPage;
    int idTimerRefreshCurrentInfo;
    FramePageNavigator *paginator;
    FrameSearchTable *widgetSearch;
    QTimer *timerRefreshCurrentInfo;
    QTimer *timerSearch;
    DialogCard *dialogCard;
    bool mIsRefreshAfterChange;

    bool isAddPermission;
    bool isEditPermission;
    bool isDeletePermission;

    void initPermissions();

    QAction *actionAdd;
    QAction *actionEdit;
    QAction *actionDelete;
    QAction *actionRefresh;

    QHBoxLayout layoutButtons;

    bool isMultiSelect;
    bool isSelectMode;
    bool isRefreshOnShow;
    bool isShown;
    bool isRestoreStates;
    bool isRefreshedCurrentInfo;

    void restoreStateTree();
    void setStateTree();
    void setStateTreeDefault();
    void saveStateTree();


};

#endif // FORMCATALOG_H
