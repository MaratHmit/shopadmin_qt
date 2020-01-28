#ifndef DIALOGADDPARAMPAYSYSTEM_H
#define DIALOGADDPARAMPAYSYSTEM_H

#include "datapayrequisite.h"

#include <QDialog>

namespace Ui {
class DialogAddParamPaySystem;
}

class DialogAddParamPaySystem : public QDialog
{
    Q_OBJECT

public:
    explicit DialogAddParamPaySystem(QWidget *parent, const bool &isSave = true);
    ~DialogAddParamPaySystem();

    QString code() const;
    QString name() const;
    void setIdPayment(const QString &idPayment);
    void setPayReq(const DataPayRequisite *payReq);
    const DataPayRequisite *payReq() const;

private slots:
    void on_buttonBox_accepted();

private:
    void showEvent(QShowEvent *e);

    Ui::DialogAddParamPaySystem *ui;
    DataPayRequisite *mParam;
    bool mIsSave;

    void setData();
    void setValidators();
    void fillControls();
};

#endif // DIALOGADDPARAMPAYSYSTEM_H
