#include "formlogin.h"
#include "ui_formlogin.h"
#include "seconfig.h"
#include "api.h"
#include "seconsts.h"
#include "apiadapter.h"
#include "dataitem.h"
#include "dialogregistration.h"
#include "updater.h"
#include "dialogproxysettings.h"

#include <QApplication>
#include <QKeyEvent>
#include <QFont>
#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <QProcess>

FormLogin::FormLogin(QWidget *parent) :
    QDialog(parent), ui(new Ui::FormLogin)
{
    ui->setupUi(this);    

    ui->labelNameApp->setText(QApplication::applicationName());
    ui->labelBuildNum->setText(QApplication::applicationVersion());
    modelProjects = 0;
    auth = new DataItem();    
    createFolders();
    SEConfig::loadSettings();
    ui->pushButtonOk->setDefault(true);
    ui->lineUpdate->hide();
    ui->widgetUpdate->hide();
    ui->widgetProjects->hide();
    setMaximumWidth(300);
    setMaximumHeight(250);

    preloader = new Preloader(this);
    preloader->hide();
    projects = new ListDataItems();       
    initProjects();    
}

bool FormLogin::authAuto()
{
    if (ApiAdapter::getApi()->registration(SEConfig::serial, SEConfig::keyHash, SEConfig::projectAlias)) {
        SEConfig::createProjectFolder();
        if (ApiAdapter::getApi()->auth()) {
            SEConfig::saveSettings();
            return true;
        }
    }
    QMessageBox::critical(this, tr("Ошибка подключения"),
                          tr("Неправильный логин или пароль!"), QMessageBox::Ok);
    return false;
}

FormLogin::~FormLogin()
{
    delete projects;
    delete ui;
    delete preloader;
    delete auth;    
}

void FormLogin::finishAuth()
{    
    hideLoader();
    setCursor(QCursor(Qt::ArrowCursor));
    bool isAuth = !SEConfig::token.isEmpty() && !SEConfig::apiUrl.isEmpty();

    if (isAuth) {
        SEConfig::saveSettings();
        SEConfig::saveAccountSettings(ui->checkBoxSavePassword->isChecked());
        accept();
    }
    else QMessageBox::critical(this, tr("Ошибка подключения"),
                                 tr("Неправильный логин или пароль!"), QMessageBox::Ok);
}

void FormLogin::showLoader()
{  
    preloader->startAnimation();
}

void FormLogin::hideLoader()
{  
   preloader->stopAnimation();
}

void FormLogin::checkEnabled()
{
   ui->pushButtonOk->setEnabled(!ui->lineEditLogin->text().isEmpty() &&
                                !ui->lineEditPassword->text().isEmpty());
   ui->toolButtonDelProject->setEnabled(ui->listView->currentIndex().isValid());
}

void FormLogin::onChangeProject()
{
    if (!ui->listView->currentIndex().isValid()) {
        setWindowTitle(tr("Авторизация"));
        return;
    }

    DataItem *project = modelProjects->dataItem(ui->listView->currentIndex());
    SEConfig::projectAlias = project->getName();
    SEConfig::loadAccountSettings();
    setWindowTitle(tr("Авторизация") + " [" + SEConfig::projectAlias + "]");
    ui->lineEditProject->setText(SEConfig::projectAlias);
    ui->lineEditLogin->setText(SEConfig::serial);
    if (!SEConfig::keyHash.isEmpty()) {
        ui->lineEditPassword->setText(SEConsts::ECHO_PASSWORDTEXT);
        keyHash = SEConfig::keyHash;
        ui->checkBoxSavePassword->setChecked(true);
    };
    checkEnabled();
}

void FormLogin::onAddProject()
{
    ui->lineEditProject->clear();
    ui->lineEditLogin->clear();
    ui->lineEditPassword->clear();
    ui->checkBoxSavePassword->setChecked(false);
    ui->lineEditProject->setFocus();
    SEConfig::token.clear();
    SEConfig::apiUrl.clear();
}

void FormLogin::onDelProject()
{
    if (!ui->listView->selectionModel()->selectedRows(0).count())
        return;

    QString msg;
    if (ui->listView->selectionModel()->selectedRows().count() > 1)
        msg = tr("Удалить выбранные проекты?");
    else msg = tr("Удалить проект") + ": " +
            modelProjects->nameData(ui->listView->currentIndex()) + "?";

    QMessageBox::StandardButton mesRepl;
    mesRepl = QMessageBox::question(this, tr("Подтверждение удаления"),
                                    msg, QMessageBox::Yes | QMessageBox::No);
    if(mesRepl == QMessageBox::Yes)
    {
        ui->toolButtonDelProject->setEnabled(false);
        QModelIndexList list = ui->listView->selectionModel()->selectedRows();
        for (int i = 0; i < list.size(); i++) {
             DataItem *project = modelProjects->dataItem(list.at(i));
             QDir dir(SEConfig::getFolderProjects() + QDir::separator() +
                      project->getName());
             dir.removeRecursively();
        }       
        ui->lineEditLogin->clear();
        ui->lineEditPassword->clear();
        ui->checkBoxSavePassword->setChecked(false);
        initProjects();
    }
}

void FormLogin::onSetProxySettings()
{
    DialogProxySettings proxyDialog(this);
    proxyDialog.exec();
}

void FormLogin::onSearchProject(const QString &searchStr)
{
    for (int i = 0; i < projects->size(); ++i) {
         QString s = searchStr.toLower();
         QString projectName = projects->at(i)->getName().toLower();
         if (i < modelProjects->rowCount())
            ui->listView->setRowHidden(i, !projectName.contains(s));

    }
}

void FormLogin::createFolders()
{
    SEConfig::createHomeFolder();
    SEConfig::createProjectsFolder();
    SEConfig::createProjectsFolder();
}

void FormLogin::checkUpdate()
{
#ifdef Q_OS_WIN32
    Updater updater;
    if (updater.isHaveUpdate()) {
        setMaximumHeight(290);
        ui->toolButtonUpdate->setIconSize(QSize(24, 24));
        ui->lineUpdate->show();
        ui->widgetUpdate->show();
    }
#endif
}

void FormLogin::startUpdate()
{
    QProcess *process = new QProcess();
    QStringList arguments;
    arguments.append(SEConsts::UPDATER_KEY);
    arguments.append(QApplication::applicationName());
    arguments.append(QApplication::applicationVersion());
    arguments.append(SEConsts::UPDATE_TYPE_VERSION);
    arguments.append(QApplication::applicationFilePath());
    process->start(SEConsts::UPDATER_NAME, arguments);
    QApplication::quit();
}

void FormLogin::on_pushButtonOk_clicked()
{
    if (ui->lineEditProject->text().trimmed().isEmpty() ||
            ui->lineEditLogin->text().trimmed().isEmpty() ||
            ui->lineEditPassword->text().trimmed().isEmpty())
        return;

    showLoader();
    setCursor(QCursor(Qt::WaitCursor));
    SEConfig::token.clear();
    SEConfig::apiUrl.clear();
    SEConfig::projectAlias = ui->lineEditProject->text().trimmed();
    if (SEConfig::projectAlias.contains("http://"))
        SEConfig::projectAlias = SEConfig::projectAlias.remove(0, 7);
    SEConfig::loadAccountSettings();
    SEConfig::serial = ui->lineEditLogin->text().trimmed();
    if (ui->lineEditPassword->text() == SEConsts::ECHO_PASSWORDTEXT)
        SEConfig::keyHash = keyHash;
    else SEConfig::keyHash = QMD5(ui->lineEditPassword->text().trimmed());
    if (ApiAdapter::getApi()->registration(SEConfig::serial, SEConfig::keyHash, SEConfig::projectAlias)) {
        if (ApiAdapter::getApi()->auth()) {
            SEConfig::createProjectFolder();
            finishAuth();
        }
    }
    hideLoader();
    setCursor(QCursor(Qt::ArrowCursor));
}

void FormLogin::on_lineEditPassword_editingFinished()
{
    keyHash = QMD5(ui->lineEditPassword->text());
}

bool FormLogin::eventFilter(QObject *, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->key() == Qt::Key_Return &&
                !ui->pushButtonOk->isEnabled())
            ui->lineEditLogin->setFocus();
    }
    return false;
}

void FormLogin::showEvent(QShowEvent *)
{    
    initSignals();
    if (projects->size() > 1)
        ui->lineEditSearch->setFocus();
    startTimer(1);
}

void FormLogin::timerEvent(QTimerEvent *event)
{
    killTimer(event->timerId());
    checkUpdate();
}

void FormLogin::initProjects()
{
    QDir dir(SEConfig::getFolderProjects());
    if (dir.exists()) {
        projects->deleteItems();
        QStringList list =
                dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);
        for (int i = 0; i < list.size(); i++) {
            QString dirName = list.at(i).toLower();
            QDir dirSub(SEConfig::getFolderProjects() + QDir::separator() + dirName);
            QStringList subList =
                    dirSub.entryList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);
            QString fileNameAdmin;
            for (int j = 0; j < subList.size(); j++) {
                QString subDirName = subList.at(j).toLower();
                fileNameAdmin = SEConfig::getFolderProjects() + QDir::separator() + dirName +
                        QDir::separator() + subDirName + QDir::separator() + SEConsts::FILE_ADMIN;
                if (QFileInfo(fileNameAdmin).exists()) {
                    DataItem *project = new DataItem();
                    project->setName(dirName + "/" + subDirName);
                    projects->append(project);
                }
            }
            fileNameAdmin = SEConfig::getFolderProjects() + QDir::separator() + dirName +
                    QDir::separator() + SEConsts::FILE_ADMIN;
            if (QFileInfo(fileNameAdmin).exists()) {
                DataItem *project = new DataItem();
                project->setName(list.at(i).toLower());
                projects->append(project);
            }
        }
    }

    if (!modelProjects) {
        modelProjects = new NameTableModel(this, projects);
        ui->listView->setModel(modelProjects);
        ui->listView->setSelectionBehavior(QAbstractItemView::SelectRows);
        ui->listView->setSelectionMode(QAbstractItemView::ExtendedSelection);
        connect(ui->listView->selectionModel(),
                SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
                SLOT(onChangeProject()));
    }    
    modelProjects->resetModel();
    if (projects->size() > 0) {        
        if (projects->size() > 1) {
            setMinimumWidth(450);
            setMaximumWidth(16777215);
            ui->widgetProjects->show();
        }
        if (SEConfig::projectAlias.isEmpty())
            ui->listView->setCurrentIndex(modelProjects->index(0, 0));
        else {
            QModelIndex index = modelProjects->getIndexByName(SEConfig::projectAlias);
            if (index.isValid())
                ui->listView->setCurrentIndex(index);
            else ui->listView->setCurrentIndex(modelProjects->index(0, 0));            
        }                
    } else onAddProject();
}

void FormLogin::initSignals()
{    
    connect(ui->lineEditLogin, SIGNAL(textChanged(QString)), this,
            SLOT(checkEnabled()));
    connect(ui->lineEditLogin, SIGNAL(returnPressed()),
            ui->lineEditPassword, SLOT(setFocus()));
    connect(ui->lineEditPassword, SIGNAL(textChanged(QString)), this,
            SLOT(checkEnabled()));
    connect(ui->toolButtonAddProject, &QAbstractButton::clicked, this, &FormLogin::onAddProject);
    connect(ui->toolButtonDelProject, &QAbstractButton::clicked, this, &FormLogin::onDelProject);
    connect(ui->listView, &QAbstractItemView::doubleClicked,
            ui->pushButtonOk, &QAbstractButton::click);
    connect(ui->toolButtonUpdate, &QAbstractButton::clicked, this, &FormLogin::startUpdate);
    connect(ui->toolButtonSettingConnected, &QAbstractButton::clicked, this, &FormLogin::onSetProxySettings);
    connect(ui->lineEditSearch, SIGNAL(textEdited(QString)), SLOT(onSearchProject(QString)));
}
