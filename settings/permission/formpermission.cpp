#include "formpermission.h"
#include "ui_formpermission.h"


FormPermission::FormPermission(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormPermission)
{
    ui->setupUi(this);
    
    initVariables();
    initControls();
    initSignals();
}

FormPermission::~FormPermission()
{    
    delete ui;
}

void FormPermission::showEvent(QShowEvent *)
{
    if (!isShown)
        SEConfig::loadSplitterLayout(splittter);
    isShown = true;
}

void FormPermission::hideEvent(QHideEvent *)
{
    SEConfig::saveSplitterLayout(splittter);
}

void FormPermission::initControls()
{
    initControlsRoles();
    initControlsObjects();
    ui->tabPermission->setCurrentIndex(0);
}

void FormPermission::initControlsRoles()
{
    splittter = new QSplitter(ui->tabRoles);
    splittter->setObjectName("splitterRoles");
    splittter->setOrientation(Qt::Horizontal);

    formRoles = new FormCatalog(splittter, SEConsts::API_PERMISSION_ROLES);
    formRoles->setWindowFlags(Qt::Widget);
    formRoles->setSortingEnabled(false);
    formRoles->setRootIsDecorated(false);
    formRoles->setVisiblePaginator(false);
    formRoles->setVisibleSearch(false);
    formRoles->setWindowTitle("Роли пользователей");
    formRoles->setTitleButtonAdd("Создать роль", "Создать новую роль пользователя");
    formRoles->addField(new Field("name", "Наименование"));
    formRoles->setIsCheckedMode(true);
    cardRole = new DialogCardRole(formRoles);
    formRoles->setDialogCard(cardRole);

    formManagers = new FormManagers(splittter);
    formManagers->setModelRoles(formRoles->getModelItems());
    QVBoxLayout *layoutRoles = new QVBoxLayout(this);
    layoutRoles->addWidget(splittter);
    layoutRoles->setMargin(0);
    layoutRoles->setContentsMargins(0, 0, 0, 0);
    ui->tabRoles->setLayout(layoutRoles);
}

void FormPermission::initControlsObjects()
{
    formObjects = new FormPermissionObjects(this, *formRoles->getModelItems()->getItems());
    QVBoxLayout *layoutObjects = new QVBoxLayout(this);
    layoutObjects->addWidget(formObjects);
    layoutObjects->setMargin(0);
    layoutObjects->setContentsMargins(0, 0, 0, 0);
    ui->tabObjects->setLayout(layoutObjects);
}

void FormPermission::initVariables()
{
    isShown = false;
}

void FormPermission::initSignals()
{
    connect(formRoles, SIGNAL(checked(QStringList)), formManagers, SLOT(filterManagers(QStringList)));
}
