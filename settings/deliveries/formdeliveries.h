#ifndef FORMDELIVERIES_H
#define FORMDELIVERIES_H

#include "nametablemodel.h"
#include "datadelivery.h"

#include <QDialog>

namespace Ui {
class FormDeliveries;
}

class FormDeliveries : public QDialog
{
    Q_OBJECT

public:
    explicit FormDeliveries(QWidget *parent, const bool &isSelectMode = false);
    const DataDelivery *selectedDelivery() const;

    ~FormDeliveries();    

private slots:
    void refreshData();
    void currentRowChange();
    void addEditDelivery();
    void deleteDelivery();
    void showContextMenu(QPoint point);
    void sortDelivery();

private:
    Ui::FormDeliveries *ui;
    NameTableModel *mModel;
    bool mIsShowed;
    bool mIsLoadedLayouts;
    bool mIsSelectMode;    
    ListDeliveries *mListDelivs;    
    int mIdTimer;

    bool isAddPermission;
    bool isEditPermission;
    bool isDeletePermission;

    void showEvent(QShowEvent *event);
    void timerEvent(QTimerEvent *event);
    void setSelectMode(const bool& isSelectMode);    
    void saveSort();

    void goToRowById(const QString &id);
    void initVariables();
    void initFields();
    void initSignals();    
    void initActions();    
    void initPermissions();
    QString currentIdDelivery() const;

};

#endif // FORMDELIVERIES_H
