#include "dialogregistration.h"
#include "ui_dialogregistration.h"
#include "seconfig.h"
#include "apiadapter.h"
#include "dialogproxysettings.h"

DialogRegistration::DialogRegistration(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogRegistration)
{
    ui->setupUi(this);
    initVariables();
    initSignals();
}

DialogRegistration::~DialogRegistration()
{
    delete ui;
}

const QString DialogRegistration::nameProject() const
{
    return ui->lineEditName->text().trimmed();
}

const QString DialogRegistration::serialNumber() const
{
    return ui->lineEditLogin->text().trimmed();
}

const QString DialogRegistration::serialKey() const
{
    return ui->lineEditPassword->text().trimmed();
}

void DialogRegistration::initVariables()
{    
    preloader = new Preloader(this);
    initValidators();
}

void DialogRegistration::initValidators()
{
    QRegExp regExpr("[\\w-_.\\s]*");
    ui->lineEditName->setValidator(new QRegExpValidator(regExpr, this));
}

void DialogRegistration::initSignals()
{
    connect(ui->pushButtonCancel, &QAbstractButton::clicked, this, &DialogRegistration::reject);
    connect(ui->pushButtonOk, &QAbstractButton::clicked, this, &DialogRegistration::registration);
    connect(ui->toolButtonSettingConnected, &QAbstractButton::clicked, this, &DialogRegistration::onSetProxySettings);
}

void DialogRegistration::registration()
{
    if (ui->lineEditLogin->text().trimmed().isEmpty() ||
            ui->lineEditPassword->text().trimmed().isEmpty() ||
            ui->lineEditName->text().trimmed().isEmpty()) {
        QMessageBox::critical(this, tr("Регистрация проекта"), tr("Заполнены не все поля!"), QMessageBox::Ok);
        if (ui->lineEditLogin->text().trimmed().isEmpty())
            ui->lineEditLogin->setFocus();
        else ui->lineEditPassword->setFocus();
        if (ui->lineEditName->text().trimmed().isEmpty())
            ui->lineEditName->setFocus();
        return;
    }

    setCursor(QCursor(Qt::WaitCursor));
    preloader->startAnimation();

    SEConfig::projectAlias = ui->lineEditName->text().trimmed();
    SEConfig::serial = ui->lineEditLogin->text().trimmed();
    SEConfig::keyHash = QMD5(ui->lineEditPassword->text().trimmed());

    bool isReg = ApiAdapter::getApi()->registration(SEConfig::serial, SEConfig::keyHash, SEConfig::projectAlias) &&
            !SEConfig::token.isEmpty() && !SEConfig::apiUrl.isEmpty();

    preloader->stopAnimation();
    setCursor(QCursor(Qt::ArrowCursor));

    if (isReg) {
        SEConfig::createProjectFolder();
        accept();
    }
}

void DialogRegistration::onSetProxySettings()
{
    DialogProxySettings proxyDialog(this);
    proxyDialog.exec();
}
