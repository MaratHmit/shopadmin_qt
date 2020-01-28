#include "formlicense.h"
#include "ui_formlicense.h"
#include <seconfig.h>
#include <formcardlicense.h>
#include <apiadapter.h>
#include "request.h"

FormLicense::FormLicense(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormLicense)
{
    ui->setupUi(this);
    initSignals();
    mIsShowed = false;
    mIsLoadedLayouts = false;    
}

FormLicense::~FormLicense()
{
    if (mIsShowed)
        SEConfig::saveTableState(ui->tableView->horizontalHeader(),
                                        ui->tableView->objectName());
    delete ui;

}

void FormLicense::onChangeDelivery()
{

}

void FormLicense::onAddEditClicked()
{
    int isNew = false;

    QPushButton *button = qobject_cast<QPushButton *> (sender());
    isNew = button == ui->pushButtonAdd;

    FormCardLicense *license = new FormCardLicense(this);

    QString id;
    if (!isNew && ui->tableView->currentIndex().isValid()) {
        id = mModel->idData(ui->tableView->currentIndex());
        license->setIdLicense(id);
    }
    connect(license, SIGNAL(accepted()), SLOT(refreshData()));
    license->show();
}

void FormLicense::onDeleteClicked()
{
    QString msg;
    if (ui->tableView->selectionModel()->selectedRows().count() > 1)
        msg = tr("Удалить выбранные Лицензии?");
    else msg = tr("Удалить Лицензию") + ": " +
            mModel->nameData(ui->tableView->currentIndex()) + "?";

    QMessageBox::StandardButton mesRepl;

    mesRepl = QMessageBox::question(this, tr("Подтверждение удаления"),
                                    msg, QMessageBox::Yes | QMessageBox::No);
    if(mesRepl == QMessageBox::Yes)    {
        Request request;
        QModelIndexList list = ui->tableView->selectionModel()->selectedRows();

        for (int i = 0; i < list.count(); i++)
            request.addId(mModel->idData(list.at(i)));
        ApiAdapter::getApi()->deleteLicenses(request);
        refreshData();
    }
}

void FormLicense::showEvent(QShowEvent *)
{
    if (mIsShowed)
        return;

    mModel = new LicenseTableModel(this);
    ui->tableView->setModel(mModel);
    mIsLoadedLayouts = SEConfig::restoreTableState(ui->tableView->horizontalHeader(),
                                                       ui->tableView->objectName());
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    refreshData();
    mIsShowed = true;
}

void FormLicense::initSignals()
{
    connect(ui->pushButtonAdd, SIGNAL(clicked()), SLOT(onAddEditClicked()));
    connect(ui->pushButtonEdit, SIGNAL(clicked()), SLOT(onAddEditClicked()));
    connect(ui->pushButtonDelete, SIGNAL(clicked()), SLOT(onDeleteClicked()));
}

void FormLicense::refreshData()
{
    QString id;

    disconnect(ui->tableView->selectionModel(),
               SIGNAL(selectionChanged(QItemSelection,QItemSelection)));

    QModelIndex index = ui->tableView->currentIndex();
    if (index.isValid())
        id = mModel->idData(ui->tableView->currentIndex());

    mModel->beginReset();
    ApiAdapter::getApi()->getListLicenses(mModel->listLicense());
    mModel->endReset();
//    if (!mIsLoadedLayouts)
        ui->tableView->resizeColumnsToContents();

    connect(ui->tableView->selectionModel(),
            SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            SLOT(onChangeDelivery()));

    goToRowById(id);
}

void FormLicense::goToRowById(QString id)
{
    for (int i = 0; i < mModel->rowCount(); i ++)
        if (mModel->getItems()->at(i)->getId() == id) {
            ui->tableView->selectRow(i);
            return;
        }
    if (mModel->rowCount() > 0)
        ui->tableView->selectRow(0);
    else onChangeDelivery();
}
