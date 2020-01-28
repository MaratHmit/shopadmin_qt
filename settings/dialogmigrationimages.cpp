#include "dialogmigrationimages.h"
#include "ui_dialogmigrationimages.h"

#include "seconsts.h"
#include "seconfig.h"
#include "dataitem.h"
#include "apiadapter.h"

#include <QSortFilterProxyModel>
#include <QMessageBox>
#include <QDir>

DialogMigrationImages::DialogMigrationImages(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogMigrationImages)
{
    ui->setupUi(this);

    initVariables();
    initControls();
    initSignals();
}

DialogMigrationImages::~DialogMigrationImages()
{
    delete ui;
    delete api;
}

const QString &DialogMigrationImages::getProjectMigration() const
{
    return projectMigration;
}

void DialogMigrationImages::finishedMigration(bool status, QString message)
{
    if (status)
        QDialog::accept();
    else QMessageBox::critical(this, "Миграция изображений", message);
}

void DialogMigrationImages::accept()
{
    execImagesMigration();
}

bool DialogMigrationImages::auth()
{
    if (itemsTrusted.existByName(ui->comboBox->currentText().trimmed().toLower()))
        return true;

    QString project = ui->comboBox->currentText().trimmed().toLower();
    QString serial, keyHash;
    SEConfig::getKeysProject(project, &serial, &keyHash);
    if (!serial.isEmpty() && !keyHash.isEmpty() && api->registration(serial, keyHash, project, true))
        return true;

    DialogAuthCheck dialogAuth(this, project, serial);
    if (dialogAuth.exec() == QDialog::Accepted)
        return true;

    return false;
}

void DialogMigrationImages::execImagesMigration()
{
    if (ui->comboBox->currentText().isEmpty() || !auth())
        return;

    projectMigration = ui->comboBox->currentText();

    Request request;
    request.addStringValue("command", "copyImages");
    request.addStringValue("destProject", projectMigration);
    if (ui->checkBoxIsOverride->isChecked())
        request.addStringValue("override", "true");
    QStringList dirImages;
    if (ui->checkBoxBrands->isChecked())
        dirImages << SEConsts::DIR_IMAGES_BRANDS;
    if (ui->checkBoxFeatures->isChecked())
        dirImages << SEConsts::DIR_IMAGES_FEATURES;
    if (ui->checkBoxGroups->isChecked())
        dirImages << SEConsts::DIR_IMAGES_GROUPSPRODUCTS;
    if (ui->checkBoxNews->isChecked())
        dirImages << SEConsts::DIR_IMAGES_NEWS;
    if (ui->checkBoxPaysystems->isChecked())
        dirImages << SEConsts::DIR_IMAGES_PAYMENT;
    if (ui->checkBoxProducts->isChecked())
        dirImages << SEConsts::DIR_IMAGES_PRODUCTS;
    if (!dirImages.count()) {
        QDialog::accept();
        return;
    }
    request.addStringValue("folders", dirImages.join(";"));
    api->requestInThread(SEConsts::API_MIGRATION, SEConsts::APIMETHOD_COMMAND, request);
}

void DialogMigrationImages::initControls()
{

    setCursor(Qt::WaitCursor);

    if (api->getListProjects(&itemsTrusted)) {
        for (int i = 0; i < itemsTrusted.size(); ++i)
            if (SEConfig::projectName.compare(itemsTrusted.at(i)->getName()))
                ui->comboBox->addItem(itemsTrusted.at(i)->getName().toLower());
    }

    QDir dir(SEConfig::getFolderProjects());
    if (dir.exists()) {
        QStringList list =
                dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);
        for (int i = 0; i < list.size(); i++) {
            if (!itemsTrusted.existByName(list.at(i).toLower()) &&
                    !itemsTrusted.existByName(list.at(i).toLower() + ".e-stile.ru")) {
                DataItem *item = new DataItem();
                item->setName(list.at(i).toLower());
                if (SEConfig::projectName.compare(item->getName()))
                    ui->comboBox->addItem(item->getName().toLower());
            }
        }
    }

    QSortFilterProxyModel* proxy = new QSortFilterProxyModel(ui->comboBox);
    proxy->setSourceModel(ui->comboBox->model());
    ui->comboBox->model()->setParent(proxy);
    ui->comboBox->setModel(proxy);
    ui->comboBox->model()->sort(0);

    setCursor(Qt::ArrowCursor);
}

void DialogMigrationImages::initVariables()
{
    api = new Api;
}

void DialogMigrationImages::initSignals()
{
    connect(ui->pushButtonCancel, SIGNAL(clicked(bool)), SLOT(reject()));
    connect(ui->pushButtonOk, SIGNAL(clicked(bool)), SLOT(accept()));
    connect(api, SIGNAL(finishedRequest(bool,QString)), SLOT(finishedMigration(bool,QString)));
}
