#include "formintegrationsstores.h"
#include "ui_formintegrationsstores.h"

#include <QDesktopServices>
#include <QThread>

FormIntegrationsStores::FormIntegrationsStores(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormIntegrationsStores)
{
    ui->setupUi(this);

    initControls();
    initVariables();
    initSignals();
}

FormIntegrationsStores::~FormIntegrationsStores()
{
    delete ui;
    delete mIntegrations;
}

void FormIntegrationsStores::getToken()
{
    if (!mIntegrations->size())
        return;

    QDesktopServices::openUrl(QUrl(mIntegrations->at(0)->property("urlOAuth").toString()));
    ui->widgetCheckToken->show();
    mPreloader->startAnimation();
    ui->pushButtonGetToken->setEnabled(0);
    checkToken();
}

void FormIntegrationsStores::checkToken()
{
    if (!mIntegrations->size())
        return;

    ui->pushButtonCancelCheckToken->setEnabled(true);
    ui->pushButtonCancelCheckToken->setText("Отмена");
    QThread* thread = new QThread;
    CheckOAuthToken *checkToken = new CheckOAuthToken;
    checkToken->moveToThread(thread);    
    connect(ui->pushButtonCancelCheckToken, SIGNAL(clicked(bool)), checkToken, SLOT(stop()));
    connect(ui->pushButtonCancelCheckToken, SIGNAL(clicked(bool)), SLOT(stopCheckToken()));
    connect(thread, SIGNAL(started()), checkToken, SLOT(process()));    
    connect(checkToken, SIGNAL(finished()), thread, SLOT(quit()));
    connect(checkToken, SIGNAL(finished()), SLOT(tokenReceived()));
    connect(checkToken, SIGNAL(finished()), checkToken, SLOT(deleteLater()));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

    thread->start();
}

void FormIntegrationsStores::tokenReceived()
{
    mPreloader->stopAnimation();
    ui->widgetCheckToken->hide();
    ui->pushButtonGetToken->setEnabled(1);    
    refreshData();
}

void FormIntegrationsStores::stopCheckToken()
{
    ui->pushButtonCancelCheckToken->setEnabled(false);
    ui->pushButtonCancelCheckToken->setText("Ожидайте отмены!");
}

void FormIntegrationsStores::saveIsActive()
{
    if (!mIntegrations->size())
        return;

    mIntegrations->at(0)->setIsActive(ui->checkBoxIsActive->isChecked());
    if (ui->checkBoxIsActive->isChecked())
        SEConfig::yandexToken = mIntegrations->at(0)->property("token").toString();
    else SEConfig::yandexToken.clear();
    if (mIntegrations->at(0)->isModified()) {
        setCursor(Qt::WaitCursor);
        ApiAdapter::getApi()->saveIntegrationsStores(mIntegrations->at(0));
        setCursor(Qt::ArrowCursor);
    }
}

void FormIntegrationsStores::showEvent(QShowEvent *)
{
    if (!mIsShowen)
        refreshData();
}

void FormIntegrationsStores::initControls()
{
    ui->checkBoxIsActive->hide();
    ui->widgetCheckToken->hide();
    ui->widgetTokenInfo->hide();
    ui->pushButtonGetToken->setEnabled(0);
}

void FormIntegrationsStores::initVariables()
{
    mIsShowen = false;
    mIntegrations = new ListDataItems;
    mPreloader = new Preloader(ui->widgetPreloader);
}

void FormIntegrationsStores::initSignals()
{
   connect(ui->pushButtonGetToken, SIGNAL(clicked(bool)), SLOT(getToken()));
   connect(ui->checkBoxIsActive, SIGNAL(clicked(bool)), SLOT(saveIsActive()));
}

void FormIntegrationsStores::refreshData()
{
    if (ApiAdapter::getApi()->getListIntegrationsServices(mIntegrations) && mIntegrations->size()) {
        ui->pushButtonGetToken->setEnabled(mIntegrations->size());
        if (!mIntegrations->at(0)->property("token").toString().isEmpty()) {
            SEConfig::yandexApi = mIntegrations->at(0)->property("urlApi").toString();
            SEConfig::yandexToken = mIntegrations->at(0)->property("token").toString();
            SEConfig::yandexLogin = mIntegrations->at(0)->property("login").toString();
            ui->widgetTokenInfo->show();
            ui->lineEditToken->setText(mIntegrations->at(0)->property("token").toString());
            ui->lineEditLogin->setText(mIntegrations->at(0)->property("login").toString());
            ui->lineEditTokenDateExpires->setText(mIntegrations->at(0)->property("expired").toString());
            ui->checkBoxIsActive->show();
            ui->checkBoxIsActive->setChecked(mIntegrations->at(0)->getIsActive());
            if (!mIntegrations->at(0)->property("login").toString().isEmpty())
                ui->labelURLYandexPhotos->setText("<a href=\"https://fotki.yandex.ru/users/" +
                                                  SEConfig::yandexLogin + "\">https://fotki.yandex.ru/users/" +
                                                  SEConfig::yandexLogin + "</a>");
        }
        mIsShowen = true;
    }
}
