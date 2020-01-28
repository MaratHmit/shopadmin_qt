#include "formpaysystems.h"
#include "ui_formpaysystems.h"
#include "apiadapter.h"
#include "formpaysystemcard.h"
#include "userpermission.h"

#include <QMenu>

FormPaySystems::FormPaySystems(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormPaySystems)
{
    ui->setupUi(this);
    mIsShowed = false;
    initVariables();
    initActions();
    initSignals();
    initPermissions();
}

FormPaySystems::~FormPaySystems()
{
    delete ui;
    delete mPaySystems;
}

void FormPaySystems::showContextMenu(QPoint point)
{
    onChange();
    QPoint globalPos;

    if (sender()->inherits("QAbstractScrollArea"))
        globalPos = ((QAbstractScrollArea*)sender())->viewport()->mapToGlobal(point);
    else globalPos = ((QWidget*)sender())->mapToGlobal(point);

    QMenu menu;

    menu.addAction(ui->actionAdd);
    menu.addAction(ui->actionEdit);
    menu.addAction(ui->actionDelete);
    menu.addSeparator();
    menu.addAction(ui->actionUp);
    menu.addAction(ui->actionDown);
    menu.exec(globalPos);
}

void FormPaySystems::onSetIsActive(const QString &id, const bool &isActive)
{
    if (id.isEmpty())
        return;

    mPreloader->startAnimation();
    DataPaySystem *item = new DataPaySystem();
    item->setId(id);
    item->setIsActive(!isActive);
    item->setIsActive(isActive);    
    ApiAdapter::getApi()->savePaySystem(item);
    delete item;
    mPreloader->stopAnimation();
}

void FormPaySystems::onChange()
{
    bool isValidIndex = ui->listViewPaySystems->currentIndex().isValid();
    ui->actionEdit->setEnabled(isValidIndex && isEditPermission);
    ui->actionDelete->setEnabled(isValidIndex && isDeletePermission);
    ui->actionDown->setEnabled(isValidIndex && isEditPermission);
    ui->actionUp->setEnabled(isValidIndex && isEditPermission);
}

void FormPaySystems::onUpDownSort()
{
    if (mIdTimer)
        killTimer(mIdTimer);

    bool isMoveUp = sender() == ui->actionUp;

    QString id = currentId();

    mPaysSystemsModel->beginReset();
    if (isMoveUp)
        mPaySystems->moveUp(mPaysSystemsModel->dataItem(ui->listViewPaySystems->selectionModel()->currentIndex()));
    else mPaySystems->moveDown(mPaysSystemsModel->dataItem(ui->listViewPaySystems->selectionModel()->currentIndex()));
    mPaysSystemsModel->endReset();

    goToRowById(id);
    mTypeTimer = 1;
    mIdTimer = startTimer(1000);
}

void FormPaySystems::onAddEditPaySystem()
{
    bool isNew = ui->actionAdd == sender();
    if (!isNew && !ui->listViewPaySystems->currentIndex().isValid())
        return;

    FormPaySystemCard card(this);
    QStringList listPlugins;
    for (int i = 0; i < mPaySystems->size(); i++)
        if (!mPaySystems->at(i)->identifier().isEmpty() && !listPlugins.contains(mPaySystems->at(i)->identifier()))
            listPlugins.append(mPaySystems->at(i)->identifier());    
    if (isNew)
        card.setWindowTitle(tr("Новая платежная система"));
    else {
        DataPaySystem *item = mPaySystems->at(ui->listViewPaySystems->currentIndex().row());        
        card.setPaySystem(item);
        card.setWindowTitle(tr("Карточка платежной системы") + " [" + item->getName() + "]");
    }
    card.setExistPlugins(listPlugins);
    connect(&card, SIGNAL(save(bool,DataPaySystem*)), SLOT(onSavePayment(bool,DataPaySystem*)));
    card.exec() ;
}

void FormPaySystems::onDeletePaySystem()
{
    if (!ui->listViewPaySystems->currentIndex().isValid())
        return;

    QString msg;
    if (ui->listViewPaySystems->selectionModel()->selectedRows().count() > 1)
        msg = tr("Удалить выбранные платежные системы?");
    else msg = tr("Удалить платежную систему") + ": " +
            mPaysSystemsModel->nameData(ui->listViewPaySystems->currentIndex()) + "?";

    QMessageBox::StandardButton mesRepl;

    mesRepl = QMessageBox::question(this, tr("Подтверждение удаления"),
                                    msg, QMessageBox::Yes | QMessageBox::No);
    if(mesRepl == QMessageBox::Yes)    {
        Request request;
        QModelIndexList list = ui->listViewPaySystems->selectionModel()->selectedRows();

        for (int i = 0; i < list.count(); i++)
            request.addId(mPaySystems->at(list.at(i).row())->getId());
            ApiAdapter::getApi()->deletePaySystems(request);
        refreshData();
    }
}

void FormPaySystems::onSavePayment(const bool &isNew, DataPaySystem *paySystem)
{
    mPaysSystemsModel->beginReset();
    if (isNew)
        mPaySystems->append(new DataPaySystem(paySystem));            
    else {        
        if (ui->listViewPaySystems->currentIndex().isValid())
            mPaySystems->at(ui->listViewPaySystems->currentIndex().row())->copy(paySystem);
    }
    mPaysSystemsModel->endReset();
    goToRowById(paySystem->getId());
}

void FormPaySystems::showEvent(QShowEvent *)
{
    if (!mIsShowed)
        startTimer(1);
}

void FormPaySystems::timerEvent(QTimerEvent *e)
{
    killTimer(e->timerId());    
    switch (mTypeTimer) {
    case 0:
        refreshData();
        break;
    case 1:
        saveSortPositions();
        break;
    default:
        break;
    }
}

QString FormPaySystems::currentId()
{
    QString result = mPaysSystemsModel->idData(ui->listViewPaySystems->currentIndex());
    return result;
}

void FormPaySystems::goToRowById(const QString &id)
{

    QModelIndex modelIndex = mPaysSystemsModel->getModelIndexById(id);

    if (modelIndex.isValid()) {
        ui->listViewPaySystems->selectionModel()->setCurrentIndex(
                    modelIndex, QItemSelectionModel::SelectCurrent | QItemSelectionModel::Rows);
        return;
    }

    if (mPaysSystemsModel->rowCount() > 0)
        ui->listViewPaySystems->selectionModel()->
                setCurrentIndex(mPaysSystemsModel->index(0, 0),
                                QItemSelectionModel::SelectCurrent |
                                QItemSelectionModel::Rows);
    else onChange();
}

void FormPaySystems::saveSortPositions()
{
    ApiAdapter::getApi()->sortPaySystems(mPaysSystemsModel->getSortParamsForApi());
}

void FormPaySystems::refreshData()
{
    ui->actionEdit->setEnabled(false);
    ui->actionDown->setEnabled(false);
    ui->actionUp->setEnabled(false);
    ui->actionDelete->setEnabled(false);

    mPreloader->startAnimation();
    mPaysSystemsModel->beginReset();
    mIsShowed = ApiAdapter::getApi()->getListPaySystems(mPaySystems);
    for (int i = 0; i < mPaySystems->size(); i++)
        mPaySystems->at(i)->setIsChecked(mPaySystems->at(i)->getIsActive());
    mPaysSystemsModel->endReset();
    onChange();
    mPreloader->stopAnimation();
}

void FormPaySystems::initActions()
{
    ui->toolButtonAdd->setDefaultAction(ui->actionAdd);
    ui->toolButtonEdit->setDefaultAction(ui->actionEdit);
    ui->toolButtonDel->setDefaultAction(ui->actionDelete);
    ui->toolButtonSortDown->setDefaultAction(ui->actionDown);
    ui->toolButtonSortUp->setDefaultAction(ui->actionUp);
}

void FormPaySystems::initVariables()
{
    mPreloader = new Preloader(this);
    mPaySystems = new ListDataPaySystems();
    mTypeTimer = 0;
    mIdTimer = 0;
    mPaysSystemsModel = new NameTableModel(this, reinterpret_cast <ListDataItems *> (mPaySystems));
    mPaysSystemsModel->setCheckedMode(true);
    ui->listViewPaySystems->setModel(mPaysSystemsModel);
    ui->listViewPaySystems->setContextMenuPolicy(Qt::CustomContextMenu);    
}

void FormPaySystems::initSignals()
{
    connect(ui->toolButtonRefresh, &QToolButton::clicked, this, &FormPaySystems::refreshData);
    connect(mPaysSystemsModel, SIGNAL(checked(QString, bool)), SLOT(onSetIsActive(QString, bool)));
    connect(ui->listViewPaySystems->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), SLOT(onChange()));
    connect(ui->listViewPaySystems, SIGNAL(customContextMenuRequested(QPoint)), SLOT(showContextMenu(QPoint)));
    connect(ui->actionDown, &QAction::triggered, this, &FormPaySystems::onUpDownSort);
    connect(ui->actionUp, &QAction::triggered, this, &FormPaySystems::onUpDownSort);
    connect(ui->actionAdd, &QAction::triggered, this, &FormPaySystems::onAddEditPaySystem);
    connect(ui->actionEdit, &QAction::triggered, this, &FormPaySystems::onAddEditPaySystem);
    connect(ui->actionDelete, &QAction::triggered, this, &FormPaySystems::onDeletePaySystem);
    connect(ui->listViewPaySystems, SIGNAL(doubleClicked(QModelIndex)), ui->actionEdit, SLOT(trigger()));
}

void FormPaySystems::initPermissions()
{
    isAddPermission = UserPermission::isAdd(UserPermission::PAYSYSTEMS);
    isEditPermission = UserPermission::isEdit(UserPermission::PAYSYSTEMS);
    isDeletePermission = UserPermission::isDelete(UserPermission::PAYSYSTEMS);

    ui->actionAdd->setEnabled(isAddPermission);
}
