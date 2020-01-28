#ifndef FORMDISCOUNTS_H
#define FORMDISCOUNTS_H

#include "nametablemodel.h"
#include "datadiscount.h"
#include "preloader.h"

#include <QDialog>
#include <QTimer>

namespace Ui {
class FormDiscounts;
}

class FormDiscounts : public QDialog
{
    Q_OBJECT

public:
    explicit FormDiscounts(QWidget *parent = 0);    
    ~FormDiscounts();

    void setSelectMode(const bool &isMultiSelect);
    const ListDiscounts *selectedDiscounts();

private slots:
    void onSort();
    void saveSort();

private:
    Ui::FormDiscounts *ui;
    NameTableModel *mModel;
    ListDiscounts *mDiscounts;
    ListDiscounts *mSelectedDiscounts;
    Preloader *mPreloader;        
    QTimer *timerSort;
    bool mIsSelectMode;
    bool mIsShowed;

    bool isAddPermission;
    bool isEditPermission;
    bool isDeletePermission;

    void showEvent(QShowEvent *e);
    void timerEvent(QTimerEvent *e);

    void showContextMenu(QPoint point);
    void onAddEditDiscount();
    void onDeleteDiscount();
    void onChangeCurrentItem();

    void goToRowById(const QString &id);
    void initActions();
    void initVariables();
    void initSignals();    
    void initPermissions();
    void refreshData();
};

#endif // FORMDISCOUNTS_H
