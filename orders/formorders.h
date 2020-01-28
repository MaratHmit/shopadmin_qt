#ifndef ORDERS_H
#define ORDERS_H

#include <QDialog>
#include <QtGui>

#include "framepagenavigator.h"
#include "dataorder.h"
#include "dialogcardpreorder.h"
#include "nametablemodel.h"
#include "preloader.h"
#include "request.h"
#include "api.h"

namespace Ui {
class FormOrders;
}

class FormOrders : public QDialog
{
    Q_OBJECT

public:
    explicit FormOrders(QWidget *parent = 0);
    FormOrders(QWidget *parent, const bool &isMultiSelect);
    ~FormOrders();

    void setBrowseMode();
    void setFilterOnlyNotPaidOrders();
    void setMainFilter(const QString &filter);

    const ListOrders *selectedOrders();

public slots:    
    void onClickHeaderItem(int index);
    void refreshData();
    void refreshDataPreorders();
    void refreshDataCreditRequests();

    void goNextPage();
    void goPreviosPage();
    void goLastPage();
    void goFirstPage();

private slots:    
    void onChangeOrder();
    void onChangePreorder();
    void onChangeCreditRequest();
    void onRunSearch();
    void finishedRefreshData(const bool status, const QString message);
    void finishedShowInfoOrder(const bool status, const QString message);

    void onClickButtonSubMenu();

    void storeHeaderSize();

    void addEditPreorder();
    void removePreorder();

    void addEditCreditRequest();
    void removeCreditRequest();
    void sendRequestInBank();

    void printShipmentBlank();

protected slots:
    void reject();

protected:
    void showEvent(QShowEvent *e);
    void timerEvent(QTimerEvent *e);
    void hideEvent(QHideEvent *e);

private:
    Ui::FormOrders *ui;    

    FramePageNavigator *mNavigator;
    FramePageNavigator *mNavigatorPreorders;
    FramePageNavigator *mNavigatorCreditRequests;

    ListOrders *mOrders;
    ListOrders *mSelectedOrders;
    ListOrders *mPreorders;
    ListOrders *mCreditRequests;

    NameTableModel *mModel;
    NameTableModel *mModelPreorders;
    NameTableModel *mModelCreditRequests;
    NameTableModel *mModelProducts;

    Preloader *mPreloader;
    QVector<Field *> *mFields;
    QVector<Field *> *mFieldsPreorders;
    QVector<Field *> *mFieldsCreditRequests;
    QVector<Field *> *mFieldsProducts;

    int mDisplayMode;    
    int mIdTimer;
    bool mIsShowed;    
    bool mIsOnlyNotPaidOrders;    

    bool mIsLoadedLayouts;
    bool mIsLoadedLayoutsPreorders;
    bool mIsLoadedLayoutsCreditRequests;

    Request mRequest;
    Request mRequestPreorders;
    Request mRequestCreditRequests;

    DataOrder *mOrderInfo;
    QString mMainFilter;
    Api *mApi;
    Api *mApiInfo;

    bool isAddPermission;
    bool isEditPermission;
    bool isDeletePermission;

    int mCountOrders;
    int mCountPreorders;
    int mCountCreditRequest;
    int mNumPage;       
    int mNumPagePreorders;
    int mNumPageCreditRequests;

    void addEditOrder();
    void delOrders();
    void cancelOrders();
    void restoreOrders();
    void setOrderStatus();
    void exportData();
    void exportItemData();
    void resetRequest();
    void initActions();
    void initTreeViewOrders();
    void initTreeViewPreorders();
    void initTreeViewCreditRequest();
    void initNavigator();
    void initVariables();
    void initFilters();
    void initSignals();
    void initFields();
    void initPermissions();
    void goToRowById(QString id);
    void checkAccessFunctions();
    void onSaveOrder();
    void setDefaultStateSplitter();
    void showContactCard();
    void showInfoOrder();
    void showContextMenu(QPoint point);
    void printCheckPayment();

};

#endif // ORDERS_H
