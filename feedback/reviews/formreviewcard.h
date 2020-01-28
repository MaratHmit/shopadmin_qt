#ifndef FORMREVIEWCARD_H
#define FORMREVIEWCARD_H

#include <QDialog>

#include "datareview.h"

namespace Ui {
class FormReviewCard;
}

class FormReviewCard : public QDialog
{
    Q_OBJECT

public:
    explicit FormReviewCard(QWidget *parent = 0);
    ~FormReviewCard();

    void setReview(const DataReview *review);

signals:
    void afterShow();

private slots:
    void onSave();
    void onChangeMark(int );
    void onChangeUseTime(int );
    void onChangePlainText();
    void onAfterShow();
    void selectProduct();
    void selectUser();

private:
    Ui::FormReviewCard *ui;
    DataReview *mReview;

    void showEvent(QShowEvent* );

    void initVariables();
    void initSignals();
    void initSignalsChangedData();
    void initPermissions();

    bool save();


};

#endif // FROMREVIEWCART_H
