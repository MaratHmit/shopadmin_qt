#ifndef FORMPAYMENTS_H
#define FORMPAYMENTS_H

#include "framepagenavigator.h"
#include "datapayment.h"
#include "preloader.h"
#include "nametablemodel.h"

#include <QWidget>

namespace Ui {
class FormPayments;
}

class FormPayments : public QWidget
{
    Q_OBJECT

public:
    explicit FormPayments(QWidget *parent = 0);
    ~FormPayments();

private:
    Ui::FormPayments *ui;
    FramePageNavigator *mNavigator;
    ListPayments *mPayments;
    NameTableModel *mModel;
    Preloader *mPreloader;
    QVector<Field *> *mFields;
    bool mIsShowed;
    bool mIsLoadedLayouts;
    Request mRequest;
    int mCountPayemnts;
    int mNumPage;

    void showEvent(QShowEvent *event);
    void timerEvent(QTimerEvent *event);
    void hideEvent(QHideEvent *event);

    void addEditPayment();
    void deletePayments();

    void goNextPage();
    void goPreviosPage();
    void goLastPage();
    void goFirstPage();

    void onChangePayment();
    void onRunSearch();
    void onClickHeaderItem(int index);

    void checkEnabled();
    void goToRowById(QString id);

    void initActions();
    void initFields();
    void initTreeViewPayments();
    void initNavigator();    
    void initVariables();
    void initSignals();

    void refreshData();
    void resetRequest();
    void showContextMenu(QPoint point);
};

#endif // FORMPAYMENTS_H
