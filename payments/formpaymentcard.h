#ifndef FORMPAYMENTCARD_H
#define FORMPAYMENTCARD_H

#include "datapayment.h"
#include "preloader.h"
#include "dataorder.h"
#include "datapaysystem.h"
#include "datacontact.h"
#include "datacurrency.h"

#include <QDialog>

namespace Ui {
class FormPaymentCard;
}

class FormPaymentCard : public QDialog
{
    Q_OBJECT

public:
    explicit FormPaymentCard(QWidget *parent, const DataPayment *payment = 0);
    FormPaymentCard(QWidget *parent, const DataOrder *order);
    ~FormPaymentCard();
    const DataPayment *payment() const;

private:
    Ui::FormPaymentCard *ui;
    DataPayment *mPayment;
    DataOrder *mOrder;
    DataContact *mContact;
    Preloader *mPreloader;
    ListDataPaySystems *mPaySystems;
    ListCurrencies *mListCurrencies;
    int mDefaultHeight;
    bool mIsOrderMode;

    void showEvent(QShowEvent *event);

    void initVariables();
    void initSignals();

    void fillControls();
    void fillDefaultControls();
    void fillPaySystems();
    void initCurrencies();

    void onOk();
    void onChangeCurrency();
    void onChangePaymentTarget();
    void onChangeTypePayment();
    void onChangeNote();

    bool save();

    void selectOrder();
    void selectPayer();

    void setCurrencyByOrder();

};

#endif // FORMPAYMENTCARD_H
