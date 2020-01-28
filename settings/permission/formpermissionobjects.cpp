#include "formpermissionobjects.h"
#include "ui_formpermissionobjects.h"

#include <QMessageBox>

FormPermissionObjects::FormPermissionObjects(QWidget *parent, const ListDataItems &items) :
    QWidget(parent),
    ui(new Ui::FormPermissionObjects),
    roles(items)
{
    ui->setupUi(this);

    initVariables();
    initControls();
    initSignals();
}

FormPermissionObjects::~FormPermissionObjects()
{
    delete ui;
}

void FormPermissionObjects::completedRefreshData(const bool status, const QString message)
{    
    preloader->stopAnimation();
    if (!status)
        QMessageBox::critical(0, tr("Ой, ошибка запроса!"), message);
}

void FormPermissionObjects::refreshData()
{
    preloader->startAnimation();
    modelPermission->refreshData(ui->comboBoxRoles->currentData().toString());
}

void FormPermissionObjects::showEvent(QShowEvent *)
{    
    if (!roles.size())
        return;

    refreshRoles();
    refreshData();
}

void FormPermissionObjects::initVariables()
{

}

void FormPermissionObjects::initSignals()
{
    connect(modelPermission, SIGNAL(completedRefreshData(bool,QString)),
            SLOT(completedRefreshData(bool,QString)));    
    connect(ui->treeViewObjects, SIGNAL(clicked(QModelIndex)), modelPermission,
            SLOT(clicked(QModelIndex)));
    connect(ui->treeViewObjects->header(), SIGNAL(sectionClicked(int)), modelPermission,
            SLOT(headerClicked(int)));
}

void FormPermissionObjects::initControls()
{
    modelPermission = new PermissionsModel(this);
    delegatePermissions = new PermissionsDelegate(this);
    ui->treeViewObjects->setModel(modelPermission);    
    ui->treeViewObjects->setItemDelegate(delegatePermissions);    
    ui->treeViewObjects->header()->setSectionsClickable(true);
    preloader = new Preloader(this);
    ui->treeViewObjects->setColumnWidth(0, 150);
    for (int i = 1; i < modelPermission->columnCount(); ++i)
        ui->treeViewObjects->setColumnWidth(i, 100);
}

void FormPermissionObjects::refreshRoles()
{
    disconnect(ui->comboBoxRoles, SIGNAL(currentIndexChanged(int)));
    int currentIndex = ui->comboBoxRoles->currentIndex() >= 0 ? ui->comboBoxRoles->currentIndex() : 0;
    ui->comboBoxRoles->clear();
    for (int i = 0; i < roles.size(); ++i)
        ui->comboBoxRoles->addItem(roles.at(i)->getName(), roles.at(i)->getId());
    ui->comboBoxRoles->setCurrentIndex(currentIndex);
    connect(ui->comboBoxRoles, SIGNAL(currentIndexChanged(int)), SLOT(refreshData()));
}
