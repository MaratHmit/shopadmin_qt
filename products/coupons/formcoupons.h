#ifndef FORMCOUPONS_H
#define FORMCOUPONS_H

#include "nametablemodel.h"
#include "datacoupon.h"
#include "preloader.h"
#include "formorders.h"
#include "request.h"

#include <QWidget>
#include <QThread>

namespace Ui {
class FormCoupons;
}

class FormCoupons : public QWidget
{
    Q_OBJECT

public:
    explicit FormCoupons(QWidget *parent = 0);
    ~FormCoupons();

private:
    Ui::FormCoupons *ui;
    NameTableModel *mModel;
    NameTableModel *mModelHistory;
    ListCoupons *mCoupons;
    Preloader *mPreloader;
    FormOrders *mFrameOrders;
    Request mRequest;
    QThread *mThreadShowHistory;
    bool mIsShowed;

    bool isAddPermission;
    bool isEditPermission;
    bool isDeletePermission;

    void showEvent(QShowEvent *e);
    void timerEvent(QTimerEvent *e);
    void hideEvent(QHideEvent *e);

    void showContextMenu(QPoint point);

    void onAddEditCoupon();
    void onDeleteCoupon();
    void onChangeCurrentItem(const QModelIndex &current, const QModelIndex &previous);
    void checkEnabled();
    void setDefaultStateSplitter();

    void onCopyCellText();
    void onCopyCodeCoupon();

    void goToRowById(const QString &id);
    void initActions();
    void initVariables();
    void initSignals();
    void initPermissions();
    void resetRequest();
    void refreshData();
    void showHistory();
};

#endif // FORMCOUPONS_H
