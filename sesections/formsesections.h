#ifndef FORMSESECTIONS_H
#define FORMSESECTIONS_H

#include <QWidget>

#include "seconfig.h"
#include "apiadapter.h"
#include "nametablemodel.h"
#include "dialogsesection.h"
#include "dialogsesectionitem.h"
#include "preloader.h"

namespace Ui {
class FormSESections;
}

class FormSESections : public QWidget
{
    Q_OBJECT

public:
    explicit FormSESections(QWidget *parent = 0);
    ~FormSESections();

protected slots:
    void reject();

private slots:
    void onChangeCurrentSection();
    void onChangeCurrentItem();
    void onSort();

    void addEditSectionItem();
    void onDeleteSectionItem();
    void editSection();

private:
    void hideEvent(QHideEvent *e);
    void showEvent(QShowEvent *e);
    void timerEvent(QTimerEvent *e);

    Ui::FormSESections *ui;
    Preloader *preloader;
    ListDataItems *mSections;
    ListDataItems *mSectionsItems;
    NameTableModel *mModelSections;
    NameTableModel *mModelSectionsItems;
    QString mLastIdSection;
    bool mIsRestoreStates;
    int mIdTimer;

    void goToRowById(const QString &id);
    void initVariables();
    void initSignals();    
    void storeStateTables();
    void restoreStateTables();
    void saveSort();

    void refreshSections();
    void refreshSectionsItems();
};

#endif // FORMSESECTIONS_H
