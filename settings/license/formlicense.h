#ifndef FORMLICENSE_H
#define FORMLICENSE_H

#include <QWidget>
#include <licensetablemodel.h>

namespace Ui {
class FormLicense;
}

class FormLicense : public QWidget
{
    Q_OBJECT

public:
    explicit FormLicense(QWidget *parent = 0);
    ~FormLicense();

private slots:
    void onChangeDelivery();
    void onAddEditClicked();
    void onDeleteClicked();    
    void refreshData();

private:
    Ui::FormLicense *ui;
    LicenseTableModel *mModel;
    bool mIsShowed;
    bool mIsLoadedLayouts;

    void showEvent(QShowEvent *event);

    void initSignals();    
    void goToRowById(QString id);

};

#endif // FORMLICENSE_H
