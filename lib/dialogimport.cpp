#include "dialogimport.h"
#include "ui_dialogimport.h"
#include "apiadapter.h"
#include "seconfig.h"

#include <QFile>
#include <QFileDialog>

DialogImport::DialogImport(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogImport)
{
    ui->setupUi(this);

    initVariables();
    initSignals();
}

DialogImport::~DialogImport()
{
    delete ui;
}

void DialogImport::setApiCatalogName(const QString &apiCatalogName)
{
    mApiCatalogName = apiCatalogName;
}

void DialogImport::initVariables()
{
    mPreloader = new Preloader(ui->widgetPreloader);
    mTimer = new QTimer(this);
    ui->checkBoxIsDeleteAll->hide();
}

void DialogImport::initSignals()
{
    connect(ui->pushButtonCancel, &QAbstractButton::clicked, this, &QDialog::reject);
    connect(ui->pushButtonOK, &QAbstractButton::clicked, this, &DialogImport::startImport);
    connect(mTimer, &QTimer::timeout, this, &DialogImport::updateTimeTitle);
    connect(ui->toolButtonFileOpen, &QAbstractButton::clicked, this, &DialogImport::onGetFile);
    connect(ui->radioButtonInsert, &QRadioButton::toggled, this, &DialogImport::onChangeTypeImport);
}

void DialogImport::onGetFile()
{
    QString fileImport =
            QFileDialog::getOpenFileName(this, tr("Файл импорта"), "",
                                         "Файл импорта (*.zip *.csv *.yml *.xml)");


    if (!fileImport.trimmed().isEmpty())
        ui->lineEditImport->setText(fileImport);
}

void DialogImport::onChangeTypeImport()
{
    ui->checkBoxIsDeleteAll->setVisible(ui->radioButtonInsert->isChecked());
}

void DialogImport::startImport()
{
    QString fileName = ui->lineEditImport->text().trimmed();
    if (fileName.trimmed().isEmpty()) {
        QMessageBox::critical(0, tr("Ошибка чтения файла или ссылки"),
                              tr("Выберите файл для импорта или укажите URL!"));
        return;
    }

    if (fileName.indexOf("http") == 0) {
        mData = new QByteArray(ApiAdapter::getApi()->getContentFromUrl(fileName));
        if (mData->isEmpty()) {
            QMessageBox::critical(0, tr("Ой, ошибка сервера"), "Не удаётся получить данные по указанному URL!");
            return;
        }

    } else {
        QFile *file = new QFile(fileName);
        if (!file->exists()) {
            QMessageBox::critical(0, tr("Ошибка чтения файла"),
                                  tr("Не удаётся найти файл") + ": " +
                                  fileName + "!");
            delete file;
            return;
        }

        if(!file->open(QIODevice::ReadOnly))
        {
            QMessageBox::critical(0, tr("Ошибка чтения файла"),
                                  tr("Не удаётся прочитать файл") + ": " +
                                  fileName + "!");
            delete file;
            return;
        } else {
            mData = new QByteArray(file->readAll());
            delete file;
        }
    }

    if (mApiCatalogName.isEmpty())
        return;

    QString result;
    if (ui->checkBoxIsDeleteAll->isChecked())
        mParams = "isClear=1";
    if (ui->radioButtonInsert->isChecked()) {
        if (!mParams.isEmpty())
            mParams += "&";
        mParams += "isInsert=1";
    }

    mTime = QTime::currentTime();
    ui->pushButtonOK->setEnabled(0);

    QFileInfo fileInfo(fileName);

    mPreloader->startAnimation();
    mTimer->start(1);
    result = ApiAdapter::getApi()->importData(mApiCatalogName, mParams, mData, fileInfo.fileName());
    mTimer->stop();
    mPreloader->stopAnimation();
    ui->pushButtonOK->setEnabled(1);
    delete mData;

    if (result == "ok") {
        if (isVisible())
            accept();
    }
    else QMessageBox::critical(0, tr("Ой, ошибка сервера"), result);
}

void DialogImport::updateTimeTitle()
{
    double time = mTime.msecsTo(QTime::currentTime());
    time = time / 1000;
    ui->lineEditTime->setText(QString::number(time, 'f', 3) + " сек.");
}
