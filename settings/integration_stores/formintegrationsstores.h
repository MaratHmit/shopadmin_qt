#ifndef FORMINTEGRATIONSSTORES_H
#define FORMINTEGRATIONSSTORES_H

#include <QWidget>

#include "dataitem.h"
#include "apiadapter.h"
#include "preloader.h"
#include "checkoauthtoken.h"
#include "seconfig.h"

namespace Ui {
class FormIntegrationsStores;
}

class FormIntegrationsStores : public QWidget
{
    Q_OBJECT

public:
    explicit FormIntegrationsStores(QWidget *parent = 0);
    ~FormIntegrationsStores();

private slots:
    void getToken();
    void checkToken();
    void tokenReceived();
    void stopCheckToken();
    void saveIsActive();

private:
    Ui::FormIntegrationsStores *ui;
    ListDataItems *mIntegrations;
    Preloader *mPreloader;
    bool mIsShowen;

    void showEvent(QShowEvent *);

    void initControls();
    void initVariables();
    void initSignals();
    void refreshData();
};

#endif // FORMINTEGRATIONSSTORES_H
