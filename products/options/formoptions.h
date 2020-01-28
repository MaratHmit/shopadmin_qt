#ifndef FORMOPTIONS_H
#define FORMOPTIONS_H

#include <QWidget>

#include "apiadapter.h"
#include "seconfig.h"
#include "request.h"
#include "field.h"
#include "userpermission.h"
#include "nametablemodel.h"
#include "formoptionvaluecard.h"
#include "dataoptionvalue.h"

namespace Ui {
class FormOptions;
}

class FormOptions : public QDialog
{
    Q_OBJECT

public:
    explicit FormOptions(QWidget *parent, const bool &isSelectMode = false);
    ~FormOptions();

    QString currentId() const;
    void setIdOption(const QString id);
    ListOptionValues *selectedList() const;

public slots:
    void refreshData();

private slots:
    void onAddEditValue();
    void onDeleteValue();
    void onUpDownValue();
    void onSelect();
    void onChange();

private:
    void showEvent(QShowEvent *);
    void hideEvent(QHideEvent *);
    void timerEvent(QTimerEvent *event);
    bool eventFilter(QObject *object, QEvent *event);

    Ui::FormOptions *ui;

    void initSignals();

    int mTypeTimer;
    int mIdTimer;
    bool mSelectMode;
    bool mIsShowed;
    bool mIsRestoreStates;
    Request mRequest;
    QString mIdOption;
    ListOptionValues *mListSelected;

    void saveSort();
    void resetRequest();
    void initTreeView();
    void initButtonsSignals();
    void restoreStateTable();
    void saveStateTable();

    void goToRowById(QString id);

    NameTableModel *mModel;
    ListOptionValues *mOptionValues;
};

#endif // FORMOPTIONS_H
