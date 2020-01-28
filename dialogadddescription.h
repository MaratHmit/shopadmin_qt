#ifndef DIALOGADDDESCRIPTION_H
#define DIALOGADDDESCRIPTION_H

#include <QDialog>
#include "dataproduct.h"

namespace Ui {
class DialogAddDescription;
}

class DialogAddDescription : public QDialog
{
    Q_OBJECT

public:
    explicit DialogAddDescription(QWidget *parent, const DataProduct *product);
    ~DialogAddDescription();

private:
    Ui::DialogAddDescription *ui;
    DataProduct *mProduct;

    void showEvent(QShowEvent *);

    void onOk();
    void initVariables();
    void initSignals();
    void initSignalsChangedData();

    bool save();
};

#endif // DIALOGADDDESCRIPTION_H
