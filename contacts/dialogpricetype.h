#ifndef DIALOGPRICETYPE_H
#define DIALOGPRICETYPE_H

#include "datacontact.h"

#include <QDialog>

namespace Ui {
class DialogPriceType;
}

class DialogPriceType : public QDialog
{
    Q_OBJECT

public:
    explicit DialogPriceType(QWidget *parent, DataContact *contact);
    ~DialogPriceType();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::DialogPriceType *ui;

    DataContact *contact;
};

#endif // DIALOGPRICETYPE_H
