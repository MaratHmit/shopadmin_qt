#ifndef FORMPUBLICATIONS_H
#define FORMPUBLICATIONS_H

#include <QDialog>

#include "datanew.h"
#include "nametablemodel.h"
#include "preloader.h"
#include "framepagenavigator.h"
#include "field.h"
#include "formpublicationcard.h"

namespace Ui {
class FormPublications;
}

class FormPublications : public QDialog
{
    Q_OBJECT

public:
    explicit FormPublications(QWidget *parent = 0);

    void setSelectMode(const bool &isMultiSelect);
    ListNews *selectedNews() const;

    ~FormPublications();

private slots:
    void showContextMenu(QPoint point);
    void onClickHeaderItem(int index);
    void onChange();
    void onAddEditNews();
    void onDeleteNews();

    void goNextPage();
    void goPreviosPage();
    void goLastPage();
    void goFirstPage();
    void onRunSearch();    

private:
    void showEvent(QShowEvent *e);
    void timerEvent(QTimerEvent *e);
    void accept();

    Ui::FormPublications *ui;
    FramePageNavigator *mNavigator;
    ListNews *mNews;
    ListNews *mSelectedNews;
    NameTableModel *mModel;
    Preloader *mPreloader;
    Request mRequest;        
    QVector<Field *> mFields;
    bool mIsShowed;
    bool mIsRestoreStates;
    bool mIsSelectMode;
    int mCountNews;
    int mNumPage;

    bool isAddPermission;
    bool isEditPermission;
    bool isDeletePermission;

    void goToRowById(const QString id);
    void resetRequest();
    void checkEnabled();
    void initFields();
    void initVariables();
    void initActions();
    void initSignals();
    void initNavigator();
    void initPermissions();
    void saveSateControls();
    void restoreStateControls();
    void refreshData();
    void showCurrentDescription();
    QString setLocalURLImage(const QString &text);
};

#endif // FORMPUBLICATIONS_H
