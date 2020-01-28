#ifndef DIALOGYACATEGORIES_H
#define DIALOGYACATEGORIES_H

#include <QDialog>

#include "dataitem.h"
#include "datamain.h"
#include "seconfig.h"
#include "nametablemodel.h"

namespace Ui {
class DialogYACategories;
}

class DialogYACategories : public QDialog
{
    Q_OBJECT

public:
    explicit DialogYACategories(QWidget *parent = 0);
    ~DialogYACategories();    

    const DataItem *selectedItem() const;

public slots:
    void accept();

private:
    Ui::DialogYACategories *ui;

    ListDataItems *mItems;
    NameTableModel *mModel;

    void showEvent(QShowEvent *e);

    void initVariables();
    void initSignals();
    void refreshData();

    void setChildrens(DataItem *data, ListDataItems *list);

};

#endif // DIALOGYACATEGORIES_H
