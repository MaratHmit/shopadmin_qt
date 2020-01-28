#include "formcoupons.h"
#include "ui_formcoupons.h"
#include "field.h"
#include "apiadapter.h"
#include "formcouponcard.h"
#include "seconfig.h"
#include "userpermission.h"

#include <QMenu>

FormCoupons::FormCoupons(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormCoupons)
{
    ui->setupUi(this);
    initVariables();
    initActions();
    initSignals();
    initPermissions();
}

FormCoupons::~FormCoupons()
{
    delete ui;
    delete mCoupons;
}

void FormCoupons::showEvent(QShowEvent *)
{    
    startTimer(1);
}

void FormCoupons::timerEvent(QTimerEvent *e)
{
    killTimer(e->timerId());

    if (!SEConfig::loadSplitterLayout(ui->splitterCoupons))
        setDefaultStateSplitter();    
    refreshData();
}

void FormCoupons::hideEvent(QHideEvent *)
{
    SEConfig::saveSplitterLayout(ui->splitterCoupons);
}

void FormCoupons::showContextMenu(QPoint point)
{
    showHistory();
    QPoint globalPos;

    if (sender()->inherits("QAbstractScrollArea"))
        globalPos = ((QAbstractScrollArea*)sender())->viewport()->mapToGlobal(point);
    else globalPos = ((QWidget*)sender())->mapToGlobal(point);

    QMenu menu;

    menu.addAction(ui->actionAdd);
    menu.addAction(ui->actionEdit);
    menu.addAction(ui->actionDel);
    menu.addSeparator();
    menu.addAction(ui->actionCopy);
    menu.addAction(ui->actionCopyCode);

    menu.exec(globalPos);
}

void FormCoupons::onAddEditCoupon()
{
    bool isNew = sender() == ui->actionAdd;
    if (!isNew && !ui->treeViewCoupons->currentIndex().isValid())
        return;

    ui->actionAdd->setEnabled(false);
    ui->actionEdit->setEnabled(false);
    ui->actionDel->setEnabled(false);

    FormCouponCard card(this);
    if (!isNew) {
        card.setCoupon(dynamic_cast <DataCoupon *> (mModel->dataItem(ui->treeViewCoupons->currentIndex())));
        card.setWindowTitle(tr("Редактирование купона"));
    }
    if (card.exec() == QDialog::Accepted) {
        refreshData();
        if (isNew)
            goToRowById(card.coupon()->getId());
    } else checkEnabled();
}

void FormCoupons::onDeleteCoupon()
{
    QString msg;
    if (ui->treeViewCoupons->selectionModel()->selectedRows().count() > 1)
        msg = tr("Удалить выбранные купоны?");
    else msg = tr("Удалить купон") + ": " +
            mModel->dataItem(ui->treeViewCoupons->currentIndex())->getCode() + "?";

    QMessageBox::StandardButton mesRepl;

    mesRepl = QMessageBox::question(this, tr("Подтверждение удаления"),
                                    msg, QMessageBox::Yes | QMessageBox::No);
    if(mesRepl == QMessageBox::Yes)    {
        Request request;
        QModelIndexList list = ui->treeViewCoupons->selectionModel()->selectedRows();

        for (int i = 0; i < list.count(); i++)
            request.addId(mModel->idData(list.at(i)));
            ApiAdapter::getApi()->deleteCoupons(request);
            refreshData();
    }
}

void FormCoupons::onChangeCurrentItem(const QModelIndex &current, const QModelIndex &previous)
{
    if (current == previous)
        return;

    checkEnabled();
    showHistory();
}

void FormCoupons::checkEnabled()
{
    ui->actionAdd->setEnabled(isAddPermission);
    ui->actionEdit->setEnabled(ui->treeViewCoupons->currentIndex().isValid() &&
                               isEditPermission);
    ui->actionDel->setEnabled(ui->treeViewCoupons->currentIndex().isValid() &&
                              isDeletePermission);
}

void FormCoupons::setDefaultStateSplitter()
{
    QList<int> listSizes;
    listSizes.append(2 * height() / 3);
    listSizes.append(height() / 3);
    ui->splitterCoupons->setSizes(listSizes);
}

void FormCoupons::onCopyCellText()
{
    if (!ui->treeViewCoupons->currentIndex().isValid())
        return;

    QApplication::clipboard()->setText(
                mModel->data(ui->treeViewCoupons->currentIndex(), Qt::DisplayRole).toString());
}

void FormCoupons::onCopyCodeCoupon()
{
    if (!ui->treeViewCoupons->currentIndex().isValid())
        return;

    QApplication::clipboard()->setText(mModel->dataItem(ui->treeViewCoupons->currentIndex())->getCode());
}

void FormCoupons::goToRowById(const QString &id)
{
    for (int i = 0; i < mModel->rowCount(); i ++)
        if (mModel->getItems()->at(i)->getId() == id) {
            ui->treeViewCoupons->selectionModel()->
                    setCurrentIndex(mModel->index(i, 0),
                                    QItemSelectionModel::SelectCurrent |
                                    QItemSelectionModel::Rows);
            return;
        }

    if (mModel->rowCount() > 0)
        ui->treeViewCoupons->selectionModel()->
                setCurrentIndex(mModel->index(0, 0),
                                QItemSelectionModel::SelectCurrent |
                                QItemSelectionModel::Rows);
    else {
        ui->toolButtonEdit->setEnabled(0);
        ui->toolButtonDelete->setEnabled(0);
    }
}

void FormCoupons::initActions()
{
    ui->toolButtonAdd->setDefaultAction(ui->actionAdd);
    ui->toolButtonEdit->setDefaultAction(ui->actionEdit);
    ui->toolButtonDelete->setDefaultAction(ui->actionDel);
}

void FormCoupons::initVariables()
{
    mCoupons = new ListCoupons();
    mModel = new NameTableModel(this, reinterpret_cast <ListDataItems *> (mCoupons));
    mModel->clearFields();
    mModel->appendField(new Field("id", "Ид."));
    mModel->appendField(new Field("code", "Код"));
    mModel->appendField(new Field("type", "Тип"));
    mModel->appendField(new Field("discount", "Скидка", Qt::AlignRight | Qt::AlignVCenter));    
    mModel->appendField(new Field("minSum", "Мин. сумма", Qt::AlignRight | Qt::AlignVCenter));
    mModel->appendField(new Field("isActive", "Активен", Qt::AlignCenter | Qt::AlignVCenter));
    mModel->appendField(new Field("count", "Кол-во", Qt::AlignRight | Qt::AlignVCenter));
    mModel->appendField(new Field("isRegUser", "Зарегистрированным", Qt::AlignCenter | Qt::AlignVCenter));
    mModel->appendField(new Field("timeEnd", "Окончание"));

    ui->treeViewCoupons->setModel(mModel);
    ui->treeViewCoupons->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->treeViewCoupons, &QWidget::customContextMenuRequested,
            this, &FormCoupons::showContextMenu);

    mIsShowed = false;
    mPreloader = new Preloader(this);    
    mFrameOrders = new FormOrders(ui->groupBoxHistory);
    mThreadShowHistory = new QThread(mFrameOrders);
    mFrameOrders->moveToThread(mThreadShowHistory);
    connect(mThreadShowHistory, &QThread::started, mFrameOrders, &FormOrders::refreshData);
    mFrameOrders->setWindowFlags(Qt::Widget);
    mFrameOrders->setBrowseMode();
    mFrameOrders->setMainFilter("id IS NULL");
    QVBoxLayout *layout = new QVBoxLayout(ui->groupBoxHistory);
    layout->addWidget(mFrameOrders);
    layout->setMargin(2);
    ui->groupBoxHistory->setLayout(layout);
}

void FormCoupons::initSignals()
{
    connect(ui->treeViewCoupons->selectionModel(), &QItemSelectionModel::currentRowChanged, this,
        &FormCoupons::onChangeCurrentItem);
    connect(ui->actionAdd, &QAction::triggered, this, &FormCoupons::onAddEditCoupon);
    connect(ui->actionEdit, &QAction::triggered, this, &FormCoupons::onAddEditCoupon);
    connect(ui->actionDel, &QAction::triggered, this, &FormCoupons::onDeleteCoupon);
    connect(ui->toolButtonRefresh, &QToolButton::clicked, this, &FormCoupons::refreshData);
    connect(ui->treeViewCoupons, &QTreeView::doubleClicked, ui->actionEdit, &QAction::trigger);
    connect(ui->treeViewCoupons->header(), &QHeaderView::sectionClicked, this, &FormCoupons::refreshData);
    connect(ui->widgetSearch, &FrameSearchTable::runSearch, this, &FormCoupons::refreshData);
    connect(ui->actionCopy, &QAction::triggered, this, &FormCoupons::onCopyCellText);
    connect(ui->actionCopyCode, &QAction::triggered, this, &FormCoupons::onCopyCodeCoupon);
}

void FormCoupons::initPermissions()
{
    isAddPermission = UserPermission::isAdd(UserPermission::PRODUCTS);
    isEditPermission = UserPermission::isEdit(UserPermission::PRODUCTS);
    isDeletePermission = UserPermission::isDelete(UserPermission::PRODUCTS);

    ui->actionAdd->setEnabled(isAddPermission);
    ui->actionEdit->setEnabled(isEditPermission);
    ui->actionDel->setEnabled(isDeletePermission);
}

void FormCoupons::resetRequest()
{
    mRequest.clear();

    QString sortField = mModel->getFields().at(ui->treeViewCoupons->header()->sortIndicatorSection())->name();
    mRequest.setSortBy(sortField);
    mRequest.setSearchText(ui->widgetSearch->searchText().trimmed());
    mRequest.setAscendingOrder(ui->treeViewCoupons->header()->sortIndicatorOrder() ==
            Qt::AscendingOrder);
}

void FormCoupons::refreshData()
{
    if (!UserPermission::isRead(UserPermission::PRODUCTS))
        return;

    setCursor(QCursor(Qt::WaitCursor));
    resetRequest();
    mPreloader->startAnimation();

    QString id;
    if (ui->treeViewCoupons->currentIndex().isValid())
        id = mModel->dataItem(ui->treeViewCoupons->currentIndex())->getId();

    if (ApiAdapter::getApi()->getListCoupons(mRequest, mCoupons)) {
        mIsShowed = true;
        mModel->resetModel();
        for (int i = 0; i < mModel->columnCount() - 1; i++)
            ui->treeViewCoupons->resizeColumnToContents(i);
    }

    goToRowById(id);
    mPreloader->stopAnimation();
    setCursor(QCursor(Qt::ArrowCursor));
}

void FormCoupons::showHistory()
{
    if (!ui->treeViewCoupons->currentIndex().isValid())
        return;

    QString idCoupon = mModel->dataItem(ui->treeViewCoupons->currentIndex())->getId();

    QString filter = "[idCoupon] = " + idCoupon;
    mFrameOrders->setMainFilter(filter);

    mThreadShowHistory->terminate();
    mThreadShowHistory->wait();
    mThreadShowHistory->start();
}
