#include "formseovariables.h"
#include "ui_formseovariables.h"

#include <QMenu>

#include "apiadapter.h"
#include "dialogseovariable.h"

FormSEOVariables::FormSEOVariables(QWidget *parent, const bool &isSelectMode) :
    QDialog(parent),
    ui(new Ui::FormSEOVariables), mIsSelectMode(isSelectMode)
{
    ui->setupUi(this);
    if (isSelectMode)
        setWindowFlags(Qt::Dialog);
    else setWindowFlags(Qt::Widget);

    initControls();
    initVariables();
    initSignals();
}

FormSEOVariables::~FormSEOVariables()
{
    delete ui;
}

void FormSEOVariables::accept()
{
    if (ui->treeViewSEOVariables->currentIndex().isValid())
        QDialog::accept();
}

void FormSEOVariables::showContextMenu(QPoint point)
{
    currentRowChanged();
    QPoint globalPos;

    if (sender()->inherits("QAbstractScrollArea"))
        globalPos = ((QAbstractScrollArea*)sender())->viewport()->mapToGlobal(point);
    else globalPos = ((QWidget*)sender())->mapToGlobal(point);

    QMenu menu;

    menu.addAction(ui->actionAdd);
    menu.addAction(ui->actionEdit);
    menu.addAction(ui->actionDel);
    menu.addSeparator();
    menu.addAction(ui->actionRefresh);

    menu.exec(globalPos);
}

void FormSEOVariables::currentRowChanged()
{
    ui->actionEdit->setEnabled(ui->treeViewSEOVariables->currentIndex().isValid());
    ui->actionDel->setEnabled(ui->treeViewSEOVariables->currentIndex().isValid());
    ui->toolButtonSelect->setEnabled(ui->treeViewSEOVariables->currentIndex().isValid());
}

void FormSEOVariables::refreshData()
{
    setCursor(Qt::WaitCursor);
    if (ApiAdapter::getApi()->getListSEODynVars(Request(), mItems)) {
        mModel->resetModel();
        for (int i = 0; i < mModel->columnCount() - 1; i++)
            ui->treeViewSEOVariables->resizeColumnToContents(i);
        goToRowById(QString());
    }
    ui->treeViewSEOVariables->setFocus();
    setCursor(Qt::ArrowCursor);
}

void FormSEOVariables::addEditItem()
{
    bool isNew = sender() == ui->actionAdd;
    if (!isNew && !ui->treeViewSEOVariables->currentIndex().isValid())
        return;

    const DataItem *item = 0;
    if (!isNew)
        item = mModel->dataItem(ui->treeViewSEOVariables->currentIndex());
    DialogSEOVariable dialog(this, item);
    if (dialog.exec() == QDialog::Accepted) {
        if (isNew)
            mModel->addItem(new DataItem(dialog.item()));
        else {
            mModel->dataItem(ui->treeViewSEOVariables->currentIndex())->copy(dialog.item());
            mModel->resetModel();
        }
        ui->treeViewSEOVariables->selectionModel()->clearSelection();
        goToRowById(dialog.item()->getId());
    }
}

void FormSEOVariables::deleteItem()
{
    QString msg;
    if (ui->treeViewSEOVariables->selectionModel()->selectedRows().count() > 1)
        msg = tr("Удалить выбранные переменные?");
    else msg = tr("Удалить переменную") + ": " +
            mModel->nameData(ui->treeViewSEOVariables->currentIndex()) + "?";

    QMessageBox::StandardButton mesRepl;

    mesRepl = QMessageBox::question(this, tr("Подтверждение удаления"),
                                    msg, QMessageBox::Yes | QMessageBox::No);
    if(mesRepl == QMessageBox::Yes)    {
        Request request;
        QModelIndexList list = ui->treeViewSEOVariables->selectionModel()->selectedRows();

        for (int i = 0; i < list.count(); i++)
            request.addId(mModel->idData(list.at(i)));
            ApiAdapter::getApi()->deleteSEODynVars(request);
            refreshData();
    }
}

void FormSEOVariables::showEvent(QShowEvent *)
{
    if (!mItems->size())
        refreshData();
}

void FormSEOVariables::goToRowById(const QString &id)
{
    QModelIndex modelIndex = mModel->getModelIndexById(id);

    if (modelIndex.isValid()) {
        ui->treeViewSEOVariables->selectionModel()->setCurrentIndex(
                    modelIndex, QItemSelectionModel::SelectCurrent | QItemSelectionModel::Rows);
        return;
    }

    if (mModel->rowCount() > 0)
        ui->treeViewSEOVariables->selectionModel()->
                setCurrentIndex(mModel->index(0, 0),
                                QItemSelectionModel::SelectCurrent |
                                QItemSelectionModel::Rows);
    else currentRowChanged();
}

void FormSEOVariables::initControls()
{
    ui->toolButtonSelect->setVisible(false);
    ui->toolButtonAdd->setDefaultAction(ui->actionAdd);
    ui->toolButtonEdit->setDefaultAction(ui->actionEdit);
    ui->toolButtonDelete->setDefaultAction(ui->actionDel);
    ui->toolButtonRefresh->setDefaultAction(ui->actionRefresh);
    ui->actionEdit->setEnabled(false);
    ui->actionDel->setEnabled(false);
    ui->treeViewSEOVariables->setContextMenuPolicy(Qt::CustomContextMenu);
}

void FormSEOVariables::initVariables()
{
    mIsSelectMode = false;
    mItems = new ListDataItems;
    mModel = new NameTableModel(this, mItems, mIsSelectMode);    
    mModel->appendField(new Field("value", "Значение"));
    ui->treeViewSEOVariables->setModel(mModel);
}

void FormSEOVariables::initSignals()
{
    connect(ui->treeViewSEOVariables, SIGNAL(customContextMenuRequested(QPoint)),
            SLOT(showContextMenu(QPoint)));
    connect(ui->actionAdd, SIGNAL(triggered(bool)), SLOT(addEditItem()));
    connect(ui->actionEdit, SIGNAL(triggered(bool)), SLOT(addEditItem()));
    connect(ui->actionDel, SIGNAL(triggered(bool)), SLOT(deleteItem()));
    connect(ui->actionRefresh, SIGNAL(triggered(bool)), SLOT(refreshData()));
    if (mIsSelectMode)
        connect(ui->treeViewSEOVariables, SIGNAL(doubleClicked(QModelIndex)), ui->toolButtonSelect, SLOT(click()));
    else connect(ui->treeViewSEOVariables, SIGNAL(doubleClicked(QModelIndex)), SLOT(addEditItem()));
    connect(ui->toolButtonSelect, SIGNAL(clicked(bool)), SLOT(accept()));
    connect(ui->treeViewSEOVariables->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)), SLOT(currentRowChanged()));
}
