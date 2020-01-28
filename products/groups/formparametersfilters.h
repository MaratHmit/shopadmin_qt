#ifndef FORMPARAMETERSFILTERS_H
#define FORMPARAMETERSFILTERS_H

#include <QDialog>

#include "dataitem.h"
#include "nametablemodel.h"
#include "api.h"

namespace Ui {
class FormParametersFilters;
}

class FormParametersFilters : public QDialog
{
    Q_OBJECT

public:
    explicit FormParametersFilters(QWidget *parent = 0);
    ~FormParametersFilters();

    void setExistingFilters(ListDataItems *items);
    const ListDataItems *selectedItems();

private:
    Ui::FormParametersFilters *ui;
    ListDataItems *mItems;
    ListDataItems *mSelectedItems;
    ListDataItems *mExistingItems;
    NameTableModel *mModel;

    void showEvent(QShowEvent *event);
    void timerEvent(QTimerEvent *event);

    void initVariables();
    void initSignals();
    void removeTheExisting();
    void refreshData();
};

#endif // FORMPARAMETERSFILTERS_H
