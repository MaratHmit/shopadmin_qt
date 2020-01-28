#ifndef DIALOGDYNFIELDITEM_H
#define DIALOGDYNFIELDITEM_H

#include <QDialog>

#include "dataitem.h"

namespace Ui {
class DialogDynFieldItem;
}

class DialogDynFieldItem : public QDialog
{
    Q_OBJECT

public:
    explicit DialogDynFieldItem(QWidget *parent = 0);
    ~DialogDynFieldItem();

    void setDataType(const QString &dataType);
    void setGroups(const ListDataItems *groups);
    void setDynFieldItem(const DataItem *dynFieldItem);
    const DataItem *dynFieldItem() const;

private slots:
    void onChangeType();
    void onSave();

private:
    Ui::DialogDynFieldItem *ui;
    DataItem *mDynFieldItem;

    void showEvent(QShowEvent *e);

    void fillData();
    void fillControls();

    void initVariables();
    void initSignals();

    bool save();
};

#endif // DIALOGDYNFIELDITEM_H
