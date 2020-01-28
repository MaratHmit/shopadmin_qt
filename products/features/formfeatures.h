#ifndef FORMFEATURES_H
#define FORMFEATURES_H

#include <QDialog>
#include <QKeyEvent>

#include "nametablemodel.h"
#include "datafeature.h"
#include "requestfeature.h"
#include "framepagenavigator.h"

namespace Ui {
class FormFeatures;
}

class FormFeatures : public QDialog
{
    Q_OBJECT

public:
    FormFeatures(QWidget *parent = 0);
    FormFeatures(QWidget *parent, const bool &isDialog,
                 const bool &isModificationsMode = false);
    ~FormFeatures();

    void setSelectMode(const ListFeatures *deletedList = 0);
    void setIsNotDeleteSelectedList();
    ListFeatures *selectedList() const;

protected:
    void showEvent(QShowEvent *e);    
    void hideEvent(QShowEvent *e);
    void timerEvent(QTimerEvent *event);        

public slots:
    void refreshData();
    void refreshGroups();
    void refreshFeatures();

    void goNextPage();
    void goPreviosPage();
    void goLastPage();
    void goFirstPage();

private slots:    
    void onAddEditFeature();    
    void onDeleteFeature();
    void onChangeFeatures();
    void onSortFeatures();
    void onCheckedSEO();    
    void onSetSEOMode(const bool &isActive);
    void onSelectFeatures();
    void onSetGroup();
    void onUnbindGroup();
    void onRunSearch();

    void showContextMenu(QPoint point);

private:
    Ui::FormFeatures *ui;
    int mTypeTimer;
    int mIdTimer;
    int mNumPage;
    int mCountFeatures;
    bool mIsShowed;    
    bool mIsSelectMode;
    bool mIsModificationsMode;
    bool mIsNotDeleteSelectedList;
    ListDataItems *mListTypesFeatures;
    ListFeatures *mListSelected;    
    ListFeatures *mFeatures;
    RequestFeature mRequest;        

    bool isAddPermission;
    bool isEditPermission;
    bool isDeletePermission;

    const ListFeatures *mDeletedList;

    void initialization();
    void initNavigator();

    bool eventFilter(QObject *object, QEvent *event);

    void resetRequest();

    QString currentIdFeature();
    DataFeature *currentFeature();
    void saveSortGroups();
    void saveSEOCheckeds();
    void refreshListFeature();

    NameTableModel *mFeaturesModel;

    void initSignals();
    void initTreeFeatures();
    void initActions();
    void initPermissions();
    void saveState();
    void loadState();
    void searchData();

    void goToRowByIdFeature(QString id);

};

#endif // FORMFEATURES_H
