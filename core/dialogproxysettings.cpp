#include "dialogproxysettings.h"
#include "ui_dialogproxysettings.h"
#include "seconfig.h"

DialogProxySettings::DialogProxySettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogProxySettings)
{
    ui->setupUi(this);

    initSignals();
}

DialogProxySettings::~DialogProxySettings()
{
    delete ui;
}

void DialogProxySettings::showEvent(QShowEvent *)
{
    ui->lineEditProxyHost->setText(SEConfig::proxyHost);
    ui->spinBoxPort->setValue(SEConfig::proxyPort);
    ui->lineEditProxyLogin->setText(SEConfig::proxyLogin);
    ui->lineEditProxyPassword->setText(SEConfig::proxyPassword);
    ui->groupBoxProxy->setChecked(SEConfig::isProxyConnected);
}

void DialogProxySettings::initSignals()
{
    connect(ui->pushButtonCancel, SIGNAL(clicked()), SLOT(reject()));
    connect(ui->pushButtonOK, &QAbstractButton::clicked, this, &DialogProxySettings::save);
}

void DialogProxySettings::save()
{
    SEConfig::proxyHost = ui->lineEditProxyHost->text().trimmed();
    SEConfig::proxyPort = ui->spinBoxPort->value();
    SEConfig::proxyLogin = ui->lineEditProxyLogin->text().trimmed();
    SEConfig::proxyPassword = ui->lineEditProxyPassword->text().trimmed();
    SEConfig::isProxyConnected = ui->groupBoxProxy->isChecked();

    SEConfig::saveProxySettings();

    accept();
}
