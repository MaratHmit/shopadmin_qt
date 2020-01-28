#ifndef DIALOGPRODUCTMODIFICATIONS_H
#define DIALOGPRODUCTMODIFICATIONS_H

#include "dataproduct.h"
#include "nametablemodel.h"

#include <QDialog>

namespace Ui {
class DialogProductModifications;
}

class DialogProductModifications : public QDialog
{
    Q_OBJECT

public:    
    DialogProductModifications(QWidget *parent, DataProduct *product);
    ~DialogProductModifications();

    const ListProducts *selectedModifications();

private:
    Ui::DialogProductModifications *ui;
    DataProduct *mProduct;
    ListProducts *mModifications;
    ListProducts *mSelectedModifications;
    NameTableModel *mModel;

    void showEvent(QShowEvent *e);

    void showContextMenu(QPoint point);

    void initVariables();
    void initActions();
    void initFields();
    void initSignals();

    void fillModifications();

    void refreshData();
};

#endif // DIALOGPRODUCTMODIFICATIONS_H
