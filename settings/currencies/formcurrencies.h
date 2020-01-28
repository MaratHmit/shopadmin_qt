#ifndef FORMCURRENCIES_H
#define FORMCURRENCIES_H

#include "datamain.h"
#include "nametablemodel.h"
#include "datacurrency.h"
#include "request.h"

#include <QWidget>

namespace Ui {
class FormCurrencies;
}

class FormCurrencies : public QWidget
{
    Q_OBJECT

public:
    explicit FormCurrencies(QWidget *parent, DataMain *mainInfo);
    ~FormCurrencies();

signals:
    void changeIndexCurrencyBox(const int &index);    

private:
    Ui::FormCurrencies *ui;
    NameTableModel *mModel;
    ListCurrencies *mCurrencies;
    Request mRequest;
    DataMain *mMainInfo;

    bool isAddPermission;
    bool isEditPermission;
    bool isDeletePermission;

    void showEvent(QShowEvent *e);
    void timerEvent(QTimerEvent *e);    

    void onAddEditCurrency();
    void onDeleteCurrencies();
    void onSetRateCurency();
    void onChangeCurrentItem();
    void onChangeAutoRate();

    void goToRowById(const QString &id);
    void initSignals();
    void connectChangeSignals();
    void disconnectChaneSignals();
    void initVariables();
    void initBoxCurrencies();
    void initPermissions();
    void refreshData();
};

#endif // FORMCURRENCIES_H
