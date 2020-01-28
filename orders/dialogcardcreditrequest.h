#ifndef DIALOGCARDCREDITREQUEST_H
#define DIALOGCARDCREDITREQUEST_H

#include <QDialog>

#include "dataorder.h"
#include "nametablemodel.h"

namespace Ui {
class DialogCardCreditRequest;
}

class DialogCardCreditRequest : public QDialog
{
    Q_OBJECT

public:
    explicit DialogCardCreditRequest(QWidget *parent, const DataOrder *order = 0);
    ~DialogCardCreditRequest();

private slots:
    void on_pushButtonCancel_clicked();

    void addProduct();
    void delProduct();

    void sendRequestInBank();

private:
    Ui::DialogCardCreditRequest *ui;

    void showEvent(QShowEvent *e);

    NameTableModel *mModelProducts;
    QVector<Field *> *mFieldsProducts;

    DataOrder *mRequest;

    void initSignals();
    void initVariables();
    void fillDefaultControls();

    void getData();
    void setData();
    void calcSumTotal();

    void fillControls();

    void onOk();
    bool save();    
};

#endif // DIALOGCARDCREDITREQUEST_H
