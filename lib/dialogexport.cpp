#include "dialogexport.h"
#include "ui_dialogexport.h"

#include "apiadapter.h"
#include "seconfig.h"

#include <QRegExp>
#include <QDesktopServices>
#include <QFileDialog>
#include <QFile>
#include <QDebug>
#include <QSettings>

DialogExport::DialogExport(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogExport)
{
    ui->setupUi(this);

    initVariables();
    initValidators();
    initSignals();
}

DialogExport::~DialogExport()
{
    delete ui;
    delete mData;
}

void DialogExport::setFilter(const Request &request)
{
    mRequest.setFilter(request.filter());
}

void DialogExport::setFileName(const QString &fileName)
{
    mFileName = fileName;
    ui->lineEditFileName->setText(fileName);
}

void DialogExport::setApiCatalogName(const QString &apiCatalogName)
{
    mApiCatalogName = apiCatalogName;
}

void DialogExport::setFormat(const QString &format)
{
    mFormat = format;
    ui->labelExt->setText("." + format);
}

void DialogExport::onChangeFileName(const QString &fileName)
{
    mIsModifiedFileName = 1;
    mFileName = fileName;
}

void DialogExport::onSetFolder()
{
    QString dirExport = QFileDialog::getExistingDirectory(this, tr("Папка эскпорта"), mFolderExport);
    if (!dirExport.trimmed().isEmpty()) {
        ui->lineEditFolder->setText(dirExport);
        mFolderExport = dirExport;
        QSettings settings;
        settings.setValue("exportsFolder", mFolderExport);
        settings.sync();
    }
}

void DialogExport::initVariables()
{        
    mPreloader = new Preloader(ui->widgetPreloader);
    mTimer = new QTimer(this);
    mData = new QByteArray;
    mFileName = "export";
    mFormat = "zip";
    mIsModifiedFileName = 0;
    QSettings settings;
    mFolderExport = SEConfig::getFolderExport();
    mFolderExport = settings.value("exportsFolder", mFolderExport).toString();
    ui->lineEditFolder->setText(mFolderExport);
    ui->lineEditFolder->setSelection(0, 0);
}

void DialogExport::initSignals()
{
    connect(ui->pushButtonCancel, &QAbstractButton::clicked, this, &QDialog::reject);
    connect(ui->pushButtonOK, &QAbstractButton::clicked, this, &DialogExport::startExport);
    connect(mTimer, &QTimer::timeout, this, &DialogExport::updateTimeTitle);    
    connect(ui->lineEditFileName, &QLineEdit::textEdited, this, &DialogExport::onChangeFileName);
    connect(ui->toolButtonSetFolder, &QAbstractButton::clicked, this, &DialogExport::onSetFolder);
}

void DialogExport::startExport()
{
    if (mApiCatalogName.isEmpty())
        return;

    bool result = 0;
    mTime = QTime::currentTime();
    ui->pushButtonOK->setEnabled(0);
    mRequest.addStringValue("format", "csv");

    mPreloader->startAnimation();
    mTimer->start(1);    
    result = (ApiAdapter::getApi()->exportData(mApiCatalogName, mRequest, mData) && isVisible());
    mTimer->stop();
    if (result) {        
        SEConfig::createDir(mFolderExport);
        QString fileExport = mFolderExport + QDir::separator() + ui->lineEditFileName->text() + "." + mFormat;
        QFile file(fileExport);
        if (file.open(QFile::WriteOnly))
            file.write(*mData);
        file.close();
        if (ui->checkBoxIsOpenArhiv->isChecked())
            QDesktopServices::openUrl(QUrl::fromLocalFile(fileExport));

    }
    mPreloader->stopAnimation();

    if (result)
        accept();
    else {
        QMessageBox::critical(0, tr("Ошибка экспорта"), "Не удаётся произвести экспорт данных!");
        ui->pushButtonOK->setEnabled(1);
    }
}

void DialogExport::updateTimeTitle()
{
    double time = mTime.msecsTo(QTime::currentTime());
    time = time / 1000;
    ui->lineEditTime->setText(QString::number(time, 'f', 3) + " сек.");
}

void DialogExport::initValidators()
{
    QRegExp regExprSimple("[\\w-_\\s]*");
    QRegExpValidator *regSimpleText = new QRegExpValidator(regExprSimple, this);
    ui->lineEditFileName->setValidator(regSimpleText);

    QRegExp regExprFolder("^\\w[\\w-/_\\\\s]*");
    QRegExpValidator *regFolderText = new QRegExpValidator(regExprFolder, this);
    ui->lineEditFolder->setValidator(regFolderText);
}

