#ifndef DIALOGREGISTRATION_H
#define DIALOGREGISTRATION_H

#include <QDialog>

#include "dataitem.h"
#include "preloader.h"

namespace Ui {
class DialogRegistration;
}

class DialogRegistration : public QDialog
{
    Q_OBJECT

public:
    explicit DialogRegistration(QWidget *parent = 0);
    ~DialogRegistration();    

    const QString nameProject() const;
    const QString serialNumber() const;
    const QString serialKey() const;

private:
    Ui::DialogRegistration *ui;
    Preloader *preloader;

    void initVariables();
    void initValidators();
    void initSignals();
    void registration();
    void onSetProxySettings();
};

#endif // DIALOGREGISTRATION_H
