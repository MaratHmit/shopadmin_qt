#ifndef FORMFEATURESLIST_H
#define FORMFEATURESLIST_H

#include <QDialog>

#include "datafeature.h"
#include "nametablemodel.h"
#include "requestfeature.h"

namespace Ui {
class FormFeaturesList;
}

class FormFeaturesList : public QDialog
{
    Q_OBJECT

public:    
    FormFeaturesList(QWidget *parent = 0);
    FormFeaturesList(QWidget *parent, DataFeature *feature);
    ~FormFeaturesList();

    void setFeature(DataFeature *feature);
    void setSelectMode();
    void setSelectModificationMode();
    void setSelectedValues(const QStringList &listId);
    const ListFeatures *selectedValues();
    DataFeature *selectedFeatureValue() const;
    void refreshData();
    void clData();
    void setShadowMode();    
    void selectAll();
    void setIsNotDeleteSelectedList();

private slots:
    void onAddEdit();
    void onDelete();
    void onChange();
    void onUpDown();
    void onSelect();
    void onSearch();

private:    
    void showEvent(QShowEvent *e);
    void hideEvent(QHideEvent *e);
    void timerEvent(QTimerEvent *event);

    bool eventFilter(QObject *object, QEvent *event);

    Ui::FormFeaturesList *ui;
    DataFeature *mFeature;
    NameTableModel *mModel;
    ListFeatures *mValues;
    QStringList mListIdSelected;
    ListFeatures *mSelectedValues;

    int mIdTimer;
    int mTypeTimer;
    bool isModifiedSort;
    bool mIsNotDeleteSelectedList;
    int mIsSelectMode;
    RequestFeature mRequest;

    bool isAddPermission;
    bool isEditPermission;
    bool isDeletePermission;

    QString currentId();

    void initialization();
    void saveSort();    
    void initTable();
    void initSignals();
    void initPermissions();
    void goToRowById(const QString &id);
    void selectSelectedItems();
};

#endif // FORMFEATURESLIST_H
