#include "formdeliveries.h"
#include "ui_formdeliveries.h"
#include "apiadapter.h"
#include "seconfig.h"
#include "formcarddelivery.h"
#include "request.h"
#include "field.h"
#include "userpermission.h"

#include <QMenu>

FormDeliveries::FormDeliveries(QWidget *parent, const bool &isSelectMode) :
    QDialog(parent), ui(new Ui::FormDeliveries)
{
    ui->setupUi(this);
    if (isSelectMode)
        setWindowFlags(Qt::Dialog);
    else setWindowFlags(Qt::Widget);
    mIsSelectMode = isSelectMode;

    initVariables();
    initActions();
    initSignals();
    initPermissions();
}

const DataDelivery *FormDeliveries::selectedDelivery() const
{
    QModelIndex index = ui->treeViewDeliveries->currentIndex();
    if (!index.isValid())
        return 0;

    return dynamic_cast <DataDelivery *> (mModel->dataItem(index));
}

FormDeliveries::~FormDeliveries()
{
    if (mIsShowed)
        SEConfig::saveTableState(ui->treeViewDeliveries->header(),
                                        ui->treeViewDeliveries->objectName(), mIsSelectMode);        
    delete ui;    
}

void FormDeliveries::setSelectMode(const bool& isSelectMode)
{
    if (isSelectMode)
         ui->treeViewDeliveries->setSelectionMode(QAbstractItemView::SingleSelection);
    else ui->treeViewDeliveries->setSelectionMode(QAbstractItemView::ExtendedSelection);

    ui->actionDel->setVisible(!isSelectMode);
    ui->actionSelect->setVisible(isSelectMode);
    ui->toolButtonDelete->setVisible(!isSelectMode);
    ui->toolButtonSelect->setVisible(isSelectMode);
}

void FormDeliveries::saveSort()
{
    ApiAdapter::getApi()->sortDeliveriesTypes(mModel->getSortParamsForApi());
}

void FormDeliveries::goToRowById(const QString &id)
{
    QModelIndex modelIndex = mModel->getModelIndexById(id);

    if (modelIndex.isValid()) {
        ui->treeViewDeliveries->selectionModel()->setCurrentIndex(
                    modelIndex, QItemSelectionModel::SelectCurrent | QItemSelectionModel::Rows);
        return;
    }

    if (mModel->rowCount() > 0)
        ui->treeViewDeliveries->selectionModel()->
                setCurrentIndex(mModel->index(0, 0),
                                QItemSelectionModel::SelectCurrent |
                                QItemSelectionModel::Rows);
    else currentRowChange();
}

void FormDeliveries::initVariables()
{
    mIdTimer = 0;
    mIsShowed = false;
    mIsLoadedLayouts = false;
    setSelectMode(mIsSelectMode);
    mListDelivs = new ListDeliveries();
    mModel = new NameTableModel(this, reinterpret_cast <ListDataItems *> (mListDelivs));    
    initFields(); 
}

void FormDeliveries::initFields()
{
    mModel->appendField(new Field("type", "Тип"));
    mModel->appendField(new Field("period", "Период", Qt::AlignRight | Qt::AlignVCenter));
    mModel->appendField(new Field("price", "Цена", Qt::AlignRight | Qt::AlignVCenter));    
    mModel->appendField(new Field("note", "Примечание"));
}

void FormDeliveries::currentRowChange()
{
    QModelIndex index = ui->treeViewDeliveries->currentIndex();

    if (index.isValid()) {
        DataDelivery *deliv = dynamic_cast <DataDelivery *> (mModel->dataItem(index));
        bool isTreeMode  = false;
        for (int i = 0; i < SEConfig::getTypesDeliveries()->size(); ++i) {
            isTreeMode = SEConfig::getTypesDeliveries()->at(i)->isTreeMode() &&
                                deliv->getCode() == SEConfig::getTypesDeliveries()->at(i)->getCode();
            if (isTreeMode)
                break;    
        }
    }

    ui->actionEdit->setEnabled(index.isValid() && isEditPermission);
    ui->actionDel->setEnabled(index.isValid() && isDeletePermission);
    ui->actionSelect->setEnabled(index.isValid());
    ui->actionUp->setEnabled(index.isValid() && isEditPermission &&
                                        ui->treeViewDeliveries->
                                        selectionModel()->currentIndex().row());
    ui->actionDown->setEnabled(index.isValid() && isEditPermission &&
                                          mModel->rowCount() - 1 -
                                          ui->treeViewDeliveries->
                                          selectionModel()->currentIndex().row());
}

void FormDeliveries::showEvent(QShowEvent *)
{
    if (mIsShowed)
        return;

    mModel->setListData(reinterpret_cast<ListDataItems *> (mListDelivs));    
    ui->treeViewDeliveries->setModel(mModel);
    mIsLoadedLayouts = SEConfig::restoreTableState(
                ui->treeViewDeliveries->header(),
                ui->treeViewDeliveries->objectName(), mIsSelectMode);
    ui->treeViewDeliveries->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->treeViewDeliveries->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->treeViewDeliveries->header()->stretchLastSection();
    connect(ui->treeViewDeliveries, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(showContextMenu(QPoint)));
    refreshData();
    mIsShowed = true;
}

void FormDeliveries::timerEvent(QTimerEvent *event)
{
    killTimer(event->timerId());
    saveSort();
}

void FormDeliveries::initSignals()
{
    connect(ui->actionAdd, SIGNAL(triggered()), SLOT(addEditDelivery()));
    connect(ui->actionEdit, SIGNAL(triggered()), SLOT(addEditDelivery()));
    connect(ui->actionDel, SIGNAL(triggered()), SLOT(deleteDelivery()));
    connect(ui->actionSelect, SIGNAL(triggered()), SLOT(accept()));
    if (mIsSelectMode)
        connect(ui->treeViewDeliveries, SIGNAL(doubleClicked(QModelIndex)),
                ui->actionSelect, SLOT(trigger()));
    else connect(ui->treeViewDeliveries, SIGNAL(doubleClicked(QModelIndex)),
                 ui->actionEdit, SLOT(trigger()));
    connect(ui->toolButtonRefresh, SIGNAL(clicked(bool)), SLOT(refreshData()));
    connect(ui->actionDown, SIGNAL(triggered(bool)), SLOT(sortDelivery()));
    connect(ui->actionUp, SIGNAL(triggered(bool)), SLOT(sortDelivery()));
}

void FormDeliveries::initActions()
{
    ui->toolButtonAdd->setDefaultAction(ui->actionAdd);
    ui->toolButtonEdit->setDefaultAction(ui->actionEdit);
    ui->toolButtonDelete->setDefaultAction(ui->actionDel);
    ui->toolButtonSelect->setDefaultAction(ui->actionSelect);    
    ui->toolButtonUp->setDefaultAction(ui->actionUp);
    ui->toolButtonDown->setDefaultAction(ui->actionDown);
}

void FormDeliveries::initPermissions()
{
    isAddPermission = UserPermission::isAdd(UserPermission::DELIVERIES);
    isEditPermission = UserPermission::isEdit(UserPermission::DELIVERIES);
    isDeletePermission = UserPermission::isDelete(UserPermission::DELIVERIES);

    ui->actionAdd->setEnabled(isAddPermission);
}

void FormDeliveries::refreshData()
{
    if (!UserPermission::isRead(UserPermission::DELIVERIES))
        return;

    setCursor(QCursor(Qt::WaitCursor));    

    SEConfig::saveStateModel(ui->treeViewDeliveries);
    disconnect(ui->treeViewDeliveries->selectionModel(),
               SIGNAL(selectionChanged(QItemSelection, QItemSelection)));

    QString id = QString();
    if (ui->treeViewDeliveries->currentIndex().isValid())
        id = mModel->idData(ui->treeViewDeliveries->currentIndex());
    mModel->beginReset();
    ApiAdapter::getApi()->getListDeliveries(mListDelivs);    
    mModel->endReset();
    if (!mIsLoadedLayouts)
        for (int i = 0; i < mModel->columnCount() - 1; i++)
            ui->treeViewDeliveries->resizeColumnToContents(i);

    connect(ui->treeViewDeliveries->selectionModel(),
            SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            SLOT(currentRowChange()));

    SEConfig::restoreStateModel(ui->treeViewDeliveries);    
    goToRowById(id);
    currentRowChange();
    ui->treeViewDeliveries->setFocus();
    setCursor(QCursor(Qt::ArrowCursor));
}

QString FormDeliveries::currentIdDelivery() const
{
    QModelIndex index = ui->treeViewDeliveries->currentIndex();
    if (index.isValid())
        return mModel->idData(index);
    else return 0;
}

void FormDeliveries::addEditDelivery()
{
    bool isNew = sender() == ui->actionAdd;

    ui->actionAdd->setEnabled(0);
    ui->actionEdit->setEnabled(0);
    ui->actionDel->setEnabled(0);

    const DataDelivery *deliv = 0;
    if (ui->treeViewDeliveries->currentIndex().isValid())
        deliv = dynamic_cast <DataDelivery *> (mModel->dataItem(ui->treeViewDeliveries->currentIndex()));
    if (!deliv && !isNew)
        return;

    setCursor(Qt::WaitCursor);
    FormCardDelivery cardDeliv(this);
    if (!isNew)
        cardDeliv.setDelivery(deliv);

    if (cardDeliv.exec() == QDialog::Accepted)
        refreshData();

    ui->actionAdd->setEnabled(1);
    currentRowChange();
    setCursor(Qt::ArrowCursor);
}

void FormDeliveries::deleteDelivery()
{
    QString msg;
    if (ui->treeViewDeliveries->selectionModel()->selectedRows().count() > 1)
        msg = tr("Удалить выбранные типы доставок?");
    else msg = tr("Удалить тип доставки") + ": " +
            mModel->nameData(ui->treeViewDeliveries->currentIndex()) + "?";

    QMessageBox::StandardButton mesRepl;

    mesRepl = QMessageBox::question(this, tr("Подтверждение удаления"),
                                    msg, QMessageBox::Yes | QMessageBox::No);
    if(mesRepl == QMessageBox::Yes)    {
        Request request;
        QModelIndexList list = ui->treeViewDeliveries->selectionModel()->selectedRows();

        for (int i = 0; i < list.count(); i++)
            request.addId(mModel->idData(list.at(i)));
            ApiAdapter::getApi()->deleteDeliveries(request);
            refreshData();        
    }
}

void FormDeliveries::showContextMenu(QPoint point)
{
    currentRowChange();
    QPoint globalPos;

    if (sender()->inherits("QAbstractScrollArea"))
        globalPos = ((QAbstractScrollArea*)sender())->viewport()->mapToGlobal(point);
    else globalPos = ((QWidget*)sender())->mapToGlobal(point);

    QMenu menu;

    menu.addAction(ui->actionAdd);
    menu.addAction(ui->actionEdit);
    menu.addAction(ui->actionDel);
    menu.addSeparator();
    menu.addAction(ui->actionUp);
    menu.addAction(ui->actionDown);
    if (ui->actionSelect->isVisible()) {
      menu.addSeparator();
      menu.addAction(ui->actionSelect);
    }
    menu.exec(globalPos);
}

void FormDeliveries::sortDelivery()
{
    if (!ui->treeViewDeliveries->currentIndex().isValid())
        return;
    if (mIdTimer)
        killTimer(mIdTimer);

    bool isMoveUp = sender() == ui->actionUp;

    QString id = mModel->idData(ui->treeViewDeliveries->currentIndex());
    mModel->beginReset();
    if (isMoveUp)
        mListDelivs->moveUp(mModel->dataItem(ui->treeViewDeliveries->selectionModel()->currentIndex()));
    else mListDelivs->moveDown(mModel->dataItem(ui->treeViewDeliveries->selectionModel()->currentIndex()));
    mModel->endReset();
    goToRowById(id);
    mIdTimer = startTimer(1000);
}

