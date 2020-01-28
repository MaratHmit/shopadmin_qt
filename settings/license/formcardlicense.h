#ifndef FORMCARDLICENSE_H
#define FORMCARDLICENSE_H

#include <QDialog>
#include "datalicense.h"
namespace Ui {
class FormCardLicense;
}

class FormCardLicense : public QDialog
{
    Q_OBJECT

public:
    explicit FormCardLicense(QWidget *parent = 0);
    ~FormCardLicense();
   bool saveContact();
   void setIdLicense(const QString &idcontact);
   QString idLicense() const;

private slots:
   void on_pushButtonCancel_clicked();
   void on_pushButtonOk_clicked();

private:
    Ui::FormCardLicense *ui;
    DataLicense *mLicense;

    void showEvent(QShowEvent *event);
    void getParam();
    void fillControls();    
};

#endif // FORMCARDLICENSE_H
