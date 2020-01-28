#ifndef FRAMECUSTOMFIELDS_H
#define FRAMECUSTOMFIELDS_H

#include <QWidget>

#include "apiadapter.h"
#include "seconfig.h"
#include "seconsts.h"
#include "nametablemodel.h"

namespace Ui {
class FrameCustomFields;
}

class FrameCustomFields : public QWidget
{
    Q_OBJECT

public:
    explicit FrameCustomFields(QWidget *parent = 0);
    ~FrameCustomFields();

private slots:
    void refreshDataDynFieldsGroups();
    void refreshDataDynFields();

    void onAddEditGroup();
    void onDeleteGroup();
    void onSortGroup();

    void onAddEditItem();
    void onDeleteItem();
    void onSortItem();

    void onChangeDynFieldsGroupsItem();
    void onChangeDynFieldsItem();


private:
    Ui::FrameCustomFields *ui;

    ListDataItems *mListDynFieldGroups;
    ListDataItems *mListDynFields;

    NameTableModel *mModelDynFieldsGroups;
    NameTableModel *mModelDynFields;

    Request mRequest;
    QString dataType;

    int mIdTimer;
    bool mIsRestoreStates;
    bool mIsChangeSort;
    bool mIsChangeSortGroup;
    bool mIsShowed;

    void showEvent(QShowEvent *e);
    void hideEvent(QHideEvent *e);
    void timerEvent(QTimerEvent *e);

    void goToRowById(const QString &id);
    void goToRowByIdGroups(const QString &id);
    void resetRequest();
    void restoreStateTable();
    void setStateTableDefault();
    void saveStateTable();
    void saveSortGroups();
    void saveSortItems();

    void initActions();
    void initVariables();
    void initSignals();
    void initTreeDynFieldsGroups();
    void initTreeDynFields();
};

#endif // FRAMECUSTOMFIELDS_H
