#include "dialogproductsmasterimport.h"
#include "ui_dialogproductsmasterimport.h"

#include <apiadapter.h>

#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>

#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>

DialogProductsMasterImport::DialogProductsMasterImport(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogProductsMasterImport)
{
    ui->setupUi(this);

    initVariables();
    initSignals();
    loadProfiles();
}

DialogProductsMasterImport::~DialogProductsMasterImport()
{
    delete ui;
}

void DialogProductsMasterImport::getFile()
{
    QSettings settings;
    QString dir = settings.value("folderImport", QString()).toString();

    QString fileImport =
            QFileDialog::getOpenFileName(this, tr("Файл импорта"), dir,
                                         "Файл импорта (*.xls *.xlsx *.csv *.xml)");

    if (!fileImport.trimmed().isEmpty()) {
        QFileInfo fileInfo(fileImport);
        settings.setValue("folderImport", fileInfo.path());

        ui->lineEditFile->setText(fileImport);
        ui->buttonRun->setEnabled(true);

        QString ext = fileImport.mid(fileImport.lastIndexOf(".") + 1).toLower();
        isXml = ext.toLower().compare("xml") == 0;
        ui->widgetSettings->setHidden(isXml);
        if (isXml)
            ui->buttonRun->setText("Импорт");
        else ui->buttonRun->setText("Далее");
    }
}

void DialogProductsMasterImport::run()
{
    switch (step) {
    case 0:
        runAnalysis();
        break;
    case 1:
        runImport();
        break;
    case 2:
        saveProfile();
        break;
    default:
        break;
    }
}

void DialogProductsMasterImport::setProfile()
{
    currentProfile = nullptr;
    QString profile = ui->comboBoxProfile->currentText();
    ui->lineEditProfileName->setText(profile);
    for (int i = 0; i < profiles->size(); i++) {
        if (!profiles->at(i)->getName().compare(profile) &&
                !profiles->at(i)->property("target").toString().compare("product")) {
            currentProfile = profiles->at(i);
            QByteArray bytes(currentProfile->property("settings").toByteArray());
            QJsonDocument json;
            QJsonObject objectJson = json.fromJson(bytes).object();

            currentProfile->setProperty("keyField", objectJson["key"].toString());
            currentProfile->setProperty("delimiter", objectJson["delimiter"].toString());
            currentProfile->setProperty("encoding", objectJson["encoding"].toString());
            currentProfile->setProperty("enclosure", objectJson["enclosure"].toString());
            currentProfile->setProperty("skip", objectJson["skip"].toString().toInt());

            QJsonArray jsonCols = objectJson["cols"].toArray();
            foreach (const QJsonValue &value, jsonCols)
                currentProfile->addInListValues(value.toString());

            ui->comboBoxKeyField->setCurrentText(currentProfile->property("keyField").toString());
            ui->comboBoxDelimiter->setCurrentText(currentProfile->property("delimiter").toString());
            ui->comboBoxEncoding->setCurrentText(currentProfile->property("encoding").toString());
            ui->lineEditEnclosure->setText(currentProfile->property("enclosure").toString());
            ui->spinBoxSkipRows->setValue(currentProfile->property("skip").toInt());

            break;
        }
    }
}

void DialogProductsMasterImport::initSignals()
{
    connect(ui->buttonCancel, SIGNAL(clicked(bool)), SLOT(reject()));
    connect(ui->buttonRun, SIGNAL(clicked(bool)), SLOT(run()));
    connect(ui->toolButtonFileOpen, SIGNAL(clicked(bool)), SLOT(getFile()));
    connect(ui->comboBoxProfile, SIGNAL(currentIndexChanged(int)), SLOT(setProfile()));
}

void DialogProductsMasterImport::initVariables()
{
    step = 0;
    isXml = false;
    ui->stackedWidget->setCurrentIndex(0);
    mPreloader = new Preloader(this);
    profiles = new ListDataItems;
    currentProfile = nullptr;
}

void DialogProductsMasterImport::createFields(QByteArray bytes)
{
    QVBoxLayout *layout = new QVBoxLayout;

    QJsonDocument json;
    if(json.fromJson(bytes).object()["status"].toString() == "ok") {
        QJsonObject objectData = json.fromJson(bytes).object()["data"].toObject();
        QJsonArray colsJson = objectData["cols"].toArray();
        QJsonArray fieldsJson = objectData["fields"].toArray();
        QStringList fields;
        fields.append(QString());
        foreach (const QJsonValue& value, fieldsJson)
            fields.append(value.toString());
        int k = 0, r = 0;
        foreach (const QJsonValue& value, colsJson) {
            FrameImportColumn *frame =
                    new FrameImportColumn(ui->scrollAreaWidgetContents, new DataItem(value.toObject()), fields);
            if (currentProfile && currentProfile->getValues().count()) {
                if (k < currentProfile->getValues().count())
                    frame->setCurrentValue(currentProfile->getValues().at(k++));
            }
            layout->addWidget(frame);
            framesColumns.append(frame);
            r++;
        }

        ui->scrollAreaWidgetContents->setLayout(layout);
        if (r > 5)
            ui->scrollAreaWidgetContents->setMinimumHeight(50 * r);
        ui->buttonRun->setText("Импорт");
    }

    ui->stackedWidget->setCurrentIndex(1);
}

void DialogProductsMasterImport::loadProfiles()
{
    if (ApiAdapter::getApi()->getListData(SEConsts::API_IMPORT_PROFILE, profiles)) {
        for (int i = 0; i < profiles->size(); ++i) {
            if (profiles->at(i)->property("target").toString() == "product")
                ui->comboBoxProfile->addItem(profiles->at(i)->property("name").toString());
        }
        if (ui->comboBoxProfile->count())
            ui->comboBoxProfile->setCurrentIndex(0);
    }
}

void DialogProductsMasterImport::runAnalysis()
{
    QString fileName = ui->lineEditFile->text().trimmed();
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

    QString ext = fileName.mid(fileName.lastIndexOf(".") + 1).toLower();

    mParams = "step=0";
    mParams += "&ext=" + ext.toLower();
    mParams += "&key=" + ui->comboBoxKeyField->currentText();
    mParams += "&encoding=" + ui->comboBoxEncoding->currentText();
    mParams += "&skip=" + ui->spinBoxSkipRows->text();
    mParams += "&delimiter=" + ui->comboBoxDelimiter->currentText();
    mParams += "&enclosure=" + ui->lineEditEnclosure->text();

    QString result;

    mPreloader->startAnimation();
    result = ApiAdapter::getApi()->importData(SEConsts::API_PRODUCTS, mParams, mData, "catalog." + ext);
    mPreloader->stopAnimation();

    if (!isXml) {
        QByteArray bytes(result.toLatin1());
        QJsonDocument json;
        if(json.fromJson(bytes).object()["status"].toString() == "ok") {
            ui->labelStepInfo->setText("Шаг 2. Структура данных");
            createFields(bytes);
            step++;
        }
        if(json.fromJson(bytes).object()["status"].toString() == "error") {
            QMessageBox::critical(0, tr("Ой, ошибка импорта"),
                                  json.fromJson(bytes).object()["error"].toString());
        }
    } else {
        if (result == "ok") {
            if (isVisible())
                accept();
        }
        else QMessageBox::critical(0, tr("Ой, ошибка сервера"), result);
    }
}

void DialogProductsMasterImport::runImport()
{
    DataItem item;
    for (int i = 0; i < framesColumns.count(); ++i)
        item.addInListValues(framesColumns.at(i)->getFieldName());
    QJsonObject object;
    QString params = item.getAsJsonString();
    ui->buttonRun->setEnabled(false);
    mPreloader->startAnimation();
    bool result = ApiAdapter::getApi()->executeAPI(SEConsts::API_PRODUCTS,
                                                   SEConsts::APIMETHOD_IMPORT, params, &object);
    mPreloader->stopAnimation();
    ui->buttonRun->setEnabled(true);

    ui->labelStepInfo->setText("Результаты импорта");
    if (result) {
        int countInsert = object["countInsert"].toInt();
        int countUpdate = object["countUpdate"].toInt();
        ui->labelCountInsert->setText(QString::number(countInsert));
        ui->labelCountUpdate->setText(QString::number(countUpdate));
    }
    ui->buttonRun->setText("OK");
    ui->stackedWidget->setCurrentIndex(2);
    step++;
}

void DialogProductsMasterImport::saveProfile()
{
    if (ui->checkBoxIsSaveProfile && !ui->lineEditProfileName->text().trimmed().isEmpty()) {
        DataItem item;
        item.setProperty("profile", ui->lineEditProfileName->text());
        item.setProperty("target", "product");
        ApiAdapter::getApi()->save(SEConsts::API_IMPORT_PROFILE, &item);
    }
    accept();
}
