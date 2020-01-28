#include "formdiscounts.h"
#include "ui_formdiscounts.h"
#include "apiadapter.h"
#include "seconfig.h"
#include "formdiscountcard.h"
#include "userpermission.h"

#include <QPoint>
#include <QMenu>

FormDiscounts::FormDiscounts(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FormDiscounts)
{
    ui->setupUi(this);        
    initVariables();
    initActions();
    initSignals();
    initPermissions();
}

FormDiscounts::~FormDiscounts()
{
    delete ui;
    delete mSelectedDiscounts;
}

void FormDiscounts::setSelectMode(const bool &isMultiSelect)
{
    setWindowFlags(Qt::Window);
    mIsSelectMode = true;    

    if (!isMultiSelect)
        ui->treeViewDiscounts->setSelectionMode(QAbstractItemView::SingleSelection);
    else ui->treeViewDiscounts->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->toolButtonDelete->hide();
    ui->actionDelete->setVisible(false);
    ui->toolButtonSelect->show();
    ui->treeViewDiscounts->disconnect();
    connect(ui->treeViewDiscounts, SIGNAL(doubleClicked(QModelIndex)), ui->toolButtonSelect,
            SLOT(click()));
    connect(ui->toolButtonSelect, SIGNAL(clicked()), SLOT(accept()));
}

const ListDiscounts *FormDiscounts::selectedDiscounts()
{
    QModelIndexList list =
            ui->treeViewDiscounts->selectionModel()->selectedRows();
    mSelectedDiscounts->clear();
    for (int i = 0; i < list.size(); i++)
        mSelectedDiscounts->append(mDiscounts->at(list.at(i).row()));
    return mSelectedDiscounts;
}

void FormDiscounts::onSort()
{
    if (!ui->treeViewDiscounts->currentIndex().isValid())
        return;

    bool isMoveUp = sender() == ui->actionUp;

    QString id = mModel->dataItem(ui->treeViewDiscounts->currentIndex())->getId();
    mModel->beginReset();
    if (isMoveUp)
        mDiscounts->moveUp(mModel->dataItem(ui->treeViewDiscounts->selectionModel()->currentIndex()));
    else mDiscounts->moveDown(mModel->dataItem(ui->treeViewDiscounts->selectionModel()->currentIndex()));
    mModel->endReset();
    goToRowById(id);

    if (timerSort->isActive())
        timerSort->stop();
    timerSort->start(300);
}

void FormDiscounts::saveSort()
{
     ApiAdapter::getApi()->sortDiscounts(mModel->getSortParamsForApi());
}

void FormDiscounts::showEvent(QShowEvent *)
{    
    startTimer(1);
}

void FormDiscounts::timerEvent(QTimerEvent *e)
{
    killTimer(e->timerId());
    if (!mIsShowed)
        refreshData();
}

void FormDiscounts::showContextMenu(QPoint point)
{
    onChangeCurrentItem();
    QPoint globalPos;

    if (sender()->inherits("QAbstractScrollArea"))
        globalPos = ((QAbstractScrollArea*)sender())->viewport()->mapToGlobal(point);
    else globalPos = ((QWidget*)sender())->mapToGlobal(point);

    QMenu menu;

    menu.addAction(ui->actionAdd);
    menu.addAction(ui->actionEdit);
    if (!mIsSelectMode)
        menu.addAction(ui->actionDelete);
    menu.addSeparator();
    if (mIsSelectMode) {
        menu.addSeparator();
        menu.addAction(ui->actionSelect);
    }
    menu.addSeparator();
    menu.addAction(ui->actionAddProductsGroups);
    menu.addAction(ui->actionAddProducts);
    menu.addAction(ui->actionAddModificationsProducts);
    menu.addAction(ui->actionAddContacts);
    menu.exec(globalPos);
}

void FormDiscounts::onAddEditDiscount()
{
    bool isNew = sender() == ui->actionAdd;
    if (!isNew && !ui->treeViewDiscounts->currentIndex().isValid())
        return;

    FormDiscountCard card;
    if (!isNew)
        card.setDiscount(dynamic_cast <DataDiscount *> (mModel->dataItem(ui->treeViewDiscounts->currentIndex())));

    if (card.exec() == QDialog::Accepted) {
        refreshData();
        if (isNew)
            goToRowById(card.discount()->getId());
    }
}

void FormDiscounts::onDeleteDiscount()
{
    QString msg;
    if (ui->treeViewDiscounts->selectionModel()->selectedRows().count() > 1)
        msg = tr("Удалить выбранные скидки?");
    else msg = tr("Удалить скидку") + ": " +
            mModel->dataItem(ui->treeViewDiscounts->currentIndex())->getName() + "?";

    QMessageBox::StandardButton mesRepl;

    mesRepl = QMessageBox::question(this, tr("Подтверждение удаления"),
                                    msg, QMessageBox::Yes | QMessageBox::No);
    if(mesRepl == QMessageBox::Yes) {
        Request request;
        QModelIndexList list = ui->treeViewDiscounts->selectionModel()->selectedRows();
        for (int i = 0; i < list.count(); i++)
            request.addId(mModel->idData(list.at(i)));

        ApiAdapter::getApi()->deleteDiscounts(request);
        refreshData();
    }
}

void FormDiscounts::onChangeCurrentItem()
{
    ui->actionUp->setEnabled(ui->treeViewDiscounts->currentIndex().isValid() &&
                             mModel->dataItem(ui->treeViewDiscounts->currentIndex())->row() && isEditPermission);
    ui->actionDown->setEnabled(ui->treeViewDiscounts->currentIndex().isValid() &&
                               mModel->dataItem(ui->treeViewDiscounts->currentIndex())->parent() &&
                               mModel->dataItem(ui->treeViewDiscounts->currentIndex())->parent()->children().count() - 1 !=
            mModel->dataItem(ui->treeViewDiscounts->currentIndex())->row() && isEditPermission);
    ui->actionEdit->setEnabled(ui->treeViewDiscounts->currentIndex().isValid() &&
                               isEditPermission);
    ui->actionDelete->setEnabled(ui->treeViewDiscounts->currentIndex().isValid() &&
                                 isDeletePermission);
}

void FormDiscounts::goToRowById(const QString &id)
{
    if (!id.isEmpty()) {
        ui->treeViewDiscounts->selectionModel()->setCurrentIndex(
                    mModel->getModelIndexById(id), QItemSelectionModel::SelectCurrent |
                    QItemSelectionModel::Rows);
        ui->treeViewDiscounts->setFocus();
        return;
    }

    if (mModel->getItems()->size())
        ui->treeViewDiscounts->selectionModel()->setCurrentIndex(
                    mModel->index(0, 0), QItemSelectionModel::SelectCurrent |
                    QItemSelectionModel::Rows);
    else onChangeCurrentItem();
    ui->treeViewDiscounts->setFocus();

}

void FormDiscounts::initActions()
{
    ui->toolButtonAdd->setDefaultAction(ui->actionAdd);
    ui->toolButtonEdit->setDefaultAction(ui->actionEdit);
    ui->toolButtonDelete->setDefaultAction(ui->actionDelete);
    ui->toolButtonDown->setDefaultAction(ui->actionDown);
    ui->toolButtonUp->setDefaultAction(ui->actionUp);
}

void FormDiscounts::initVariables()
{
    mIsSelectMode = false;
    mIsShowed = false;
    ui->toolButtonSelect->hide();

    timerSort = new QTimer(this);
    timerSort->setSingleShot(true);

    mDiscounts = new ListDiscounts();
    mModel = new NameTableModel(this, reinterpret_cast <ListDataItems *> (mDiscounts));    
    mModel->clearFields();
    mModel->appendField(new Field("id", "Ид."));
    mModel->appendField(new Field("name", "Наименование"));    
    mModel->appendField(new Field("dateTimeFrom", "Старт"));
    mModel->appendField(new Field("dateTimeTo", "Завершение"));
    mModel->appendField(new Field("stepTime", "Шаг времени", Qt::AlignRight | Qt::AlignVCenter));
    mModel->appendField(new Field("stepDiscount", "Шаг скидки", Qt::AlignRight | Qt::AlignVCenter));
    mModel->appendField(new Field("sumFrom", "От суммы", Qt::AlignRight | Qt::AlignVCenter));
    mModel->appendField(new Field("sumTo", "До суммы", Qt::AlignRight | Qt::AlignVCenter));
    mModel->appendField(new Field("countFrom", "От кол-во", Qt::AlignRight | Qt::AlignVCenter));
    mModel->appendField(new Field("countTo", "До кол-во", Qt::AlignRight | Qt::AlignVCenter));
    mModel->appendField(new Field("discount", "Скидка", Qt::AlignRight | Qt::AlignVCenter));
    mModel->appendField(new Field("week", "Дни недели"));
    ui->treeViewDiscounts->setModel(mModel);
    mPreloader = new Preloader(this);
    mSelectedDiscounts = new ListDiscounts();
}

void FormDiscounts::initSignals()
{
    connect(ui->treeViewDiscounts->selectionModel(), &QItemSelectionModel::currentChanged, this,
        &FormDiscounts::onChangeCurrentItem);
    connect(ui->actionAdd, &QAction::triggered, this, &FormDiscounts::onAddEditDiscount);
    connect(ui->actionEdit, &QAction::triggered, this, &FormDiscounts::onAddEditDiscount);
    connect(ui->actionDelete, &QAction::triggered, this, &FormDiscounts::onDeleteDiscount);
    connect(ui->toolButtonRefresh, &QToolButton::clicked, this, &FormDiscounts::refreshData);
    connect(ui->treeViewDiscounts, &QTreeView::doubleClicked, ui->toolButtonEdit, &QToolButton::click);
    ui->treeViewDiscounts->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->treeViewDiscounts, &QTreeView::customContextMenuRequested, this, &FormDiscounts::showContextMenu);
    connect(ui->actionDown, SIGNAL(triggered()), SLOT(onSort()));
    connect(ui->actionUp, SIGNAL(triggered()), SLOT(onSort()));
    connect(timerSort, SIGNAL(timeout()), SLOT(saveSort()));
}

void FormDiscounts::initPermissions()
{
    isAddPermission = UserPermission::isAdd(UserPermission::PRODUCTS);
    isEditPermission = UserPermission::isEdit(UserPermission::PRODUCTS);
    isDeletePermission = UserPermission::isDelete(UserPermission::PRODUCTS);

    ui->actionAdd->setEnabled(isAddPermission);
    ui->actionEdit->setEnabled(isEditPermission);
    ui->actionDelete->setEnabled(isDeletePermission);
}

void FormDiscounts::refreshData()
{
    if (!UserPermission::isRead(UserPermission::PRODUCTS))
        return;

    setCursor(QCursor(Qt::WaitCursor));    
    mPreloader->startAnimation();

    QString id;
    if (ui->treeViewDiscounts->currentIndex().isValid())
        id = mModel->dataItem(ui->treeViewDiscounts->currentIndex())->getId();

    if (ApiAdapter::getApi()->getListDiscounts(mDiscounts)) {
        mIsShowed = true;
        mModel->resetModel();
        for (int i = 0; i < mModel->columnCount() - 1; i++)
            ui->treeViewDiscounts->resizeColumnToContents(i);
    }

    goToRowById(id);
    mPreloader->stopAnimation();
    setCursor(QCursor(Qt::ArrowCursor));
}
