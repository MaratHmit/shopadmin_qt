#ifndef DIALOGDYNFIELDSGROUP_H
#define DIALOGDYNFIELDSGROUP_H

#include <QDialog>

#include "dataitem.h"
#include "apiadapter.h"

namespace Ui {
class DialogDynFieldsGroup;
}

class DialogDynFieldsGroup : public QDialog
{
    Q_OBJECT

public:
    explicit DialogDynFieldsGroup(QWidget *parent = 0);
    ~DialogDynFieldsGroup();

    void setDataType(const QString &dataType);

    void setGroup(const DataItem *group);
    const DataItem *group() const;

private slots:
    void onSave();

private:
    Ui::DialogDynFieldsGroup *ui;
    DataItem *mGroup;

    void showEvent(QShowEvent *e);

    void fillData();
    void fillControls();

    void initVariables();
    void initSignals();

    bool save();
};

#endif // DIALOGDYNFIELDSGROUP_H
