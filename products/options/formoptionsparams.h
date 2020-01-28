#ifndef FORMOPTIONSPARAMS_H
#define FORMOPTIONSPARAMS_H

#include <QDialog>

#include "apiadapter.h"
#include "seconfig.h"
#include "request.h"
#include "field.h"
#include "userpermission.h"
#include "nametablemodel.h"
#include "dataoption.h"

namespace Ui {
class FormOptionsParams;
}

class FormOptionsParams : public QDialog
{
    Q_OBJECT

public:
    explicit FormOptionsParams(QWidget *parent = 0);
    FormOptionsParams(QWidget *parent, const bool &selectMode);

    ~FormOptionsParams();

    QString currentId() const;
    const ListOptions *getListOptions();
    QStringList getIdsOptionsChecked();    
    void setExludingOptions(const ListOptions *options);

    ListOptions *selectedList() const;

signals:
    void checkedOption();

public slots:
    void refreshData();

private slots:
    void onAddEditOption();
    void onUpDownOption();
    void onChange();
    void onSelect();
    void onChecked();

    void on_toolButtonDelOption_clicked();

private:
    void showEvent(QShowEvent *e);
    void timerEvent(QTimerEvent *event);
    bool eventFilter(QObject *object, QEvent *event);

    Ui::FormOptionsParams *ui;

    int mTypeTimer;
    int mIdTimer;
    bool mSelectMode;
    bool mIsShowed;
    bool mIsInitSignals;
    ListOptions *mListSelected;
    ListOptions *mExludingOptions;

    void saveSort();

    void initTreeView();
    void initButtonsSignals();
    void initialization();

    void goToRowById(QString id);

    NameTableModel *mModel;
    ListOptions *mOptions;
};

#endif // FORMOPTIONSPARAMS_H
