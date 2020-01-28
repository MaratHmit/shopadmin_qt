#include "formorders.h"
#include "ui_formorders.h"
#include "apiadapter.h"
#include "seconfig.h"
#include "formcontactcard.h"
#include "formcompanycard.h"
#include "formordercard.h"
#include "dialogexport.h"
#include "dialogsetorderstatus.h"
#include "userpermission.h"
#include "dialogcardcreditrequest.h"

#include <QComboBox>
#include <QLineEdit>
#include <QMenu>

FormOrders::FormOrders(QWidget *parent) :
    QDialog(parent), ui(new Ui::FormOrders)
{   
    ui->setupUi(this);    
    mDisplayMode = 0;

    if (!SEConfig::getMainInfo()->isShowRequestCredit())
        ui->pushButtonCreditRequest->hide();
    if (!SEConfig::getMainInfo()->isShowBlankShipment())
        ui->toolButtonOrderShipment->hide();
    ui->stackedWidget->setCurrentIndex(0);

    initVariables();
    initActions();
    initSignals();
    initPermissions();
}

FormOrders::FormOrders(QWidget *parent, const bool &isMultiSelect) :
    QDialog(parent),
    ui(new Ui::FormOrders)
{
    ui->setupUi(this);

    if (!SEConfig::getMainInfo()->isShowRequestCredit())
        ui->pushButtonCreditRequest->hide();
    ui->stackedWidget->setCurrentIndex(0);

    setWindowFlags(Qt::Window);
    mDisplayMode = 1;
    ui->frameButtons->hide();
    initVariables();
    initSignals();
    initActions();
    if (isMultiSelect)
        ui->treeViewOrders->setSelectionMode(QAbstractItemView::ExtendedSelection);
}

FormOrders::~FormOrders()
{    
    delete ui;        

    mOrders->deleteItems();
    mApi->deleteLater();
    mApiInfo->deleteLater();
}

void FormOrders::setBrowseMode()
{
    mDisplayMode = 2;
    ui->groupBoxFilters->hide();
    ui->widgetButtons->hide();
    ui->tabWidgetDetails->removeTab(1);
    ui->widgetCardCustomer->hide();
    ui->frameButtons->hide();
}

void FormOrders::setFilterOnlyNotPaidOrders()
{
    ui->labelOrderStatus->hide();
    ui->comboBoxOrderStatus->hide();
    mIsOnlyNotPaidOrders = true;
}

void FormOrders::setMainFilter(const QString &filter)
{
    mMainFilter = filter;
}

const ListOrders *FormOrders::selectedOrders()
{
    if (mSelectedOrders)
        return mSelectedOrders;

    mSelectedOrders = new ListOrders();
    QModelIndexList list =
            ui->treeViewOrders->selectionModel()->selectedRows();
    for (int i = 0; i < list.size(); i++)
        mSelectedOrders->append(mOrders->at(list.at(i).row()));
    return mSelectedOrders;
}

void FormOrders::onChangeOrder()
{
    if (mModel->rowCount() > 0 &&
            ui->treeViewOrders->currentIndex().isValid())
        mNavigator->setNumRecord(mNumPage * mNavigator->getLimit() +
                                 ui->treeViewOrders->currentIndex().row() + 1,
                                 mCountOrders);
    else mNavigator->setNumRecord(0, 0);    
    checkAccessFunctions();
    if (mIdTimer)
        killTimer(mIdTimer);
    mIdTimer = startTimer(300);
}

void FormOrders::onChangePreorder()
{
    if (mModelPreorders->rowCount() > 0 &&
            ui->treeViewPreorders->currentIndex().isValid())
        mNavigatorPreorders->setNumRecord(mNumPagePreorders * mNavigatorPreorders->getLimit() +
                                 ui->treeViewPreorders->currentIndex().row() + 1,
                                 mCountPreorders);
    else mNavigatorPreorders->setNumRecord(0, 0);
    ui->toolButtonEditPreorder->setEnabled(ui->treeViewPreorders->currentIndex().isValid() &&
                                    isEditPermission);
    ui->toolButtonDeletePreorder->setEnabled(ui->treeViewPreorders->currentIndex().isValid() &&
                                   isDeletePermission);

}

void FormOrders::onChangeCreditRequest()
{
    if (mModelCreditRequests->rowCount() > 0 &&
            ui->treeViewCreditRequests->currentIndex().isValid())
        mNavigatorCreditRequests->setNumRecord(mNumPageCreditRequests * mNavigatorCreditRequests->getLimit() +
                                 ui->treeViewCreditRequests->currentIndex().row() + 1,
                                 mCountCreditRequest);
    else mNavigatorCreditRequests->setNumRecord(0, 0);
    ui->toolButtonEditCreditRequest->setEnabled(ui->treeViewCreditRequests->currentIndex().isValid() &&
                                    isEditPermission);
    ui->toolButtonDeleteCreditRequest->setEnabled(ui->treeViewCreditRequests->currentIndex().isValid() &&
                                   isDeletePermission);
    ui->toolButtonSendToBank->setEnabled(false);
    if (ui->treeViewCreditRequests->currentIndex().isValid() && isEditPermission) {
        DataOrder *order = dynamic_cast <DataOrder *>
                (mModelCreditRequests->dataItem(ui->treeViewCreditRequests->currentIndex()));
        ui->toolButtonSendToBank->setEnabled(!order->property("isSent").toBool());
    }
}

void FormOrders::onRunSearch()
{
    refreshData();
    ui->treeViewOrders->setFocus();
}

void FormOrders::finishedRefreshData(const bool status, const QString message)
{
    if (status && ApiAdapter::getApi()->getListOrders(mRequest, mOrders, true)) {
        mCountOrders = mOrders->countInBase();
        ui->lineEditAmount->setText(formatNumberString(QString::number(mOrders->amount(), 'f', 2)));
        mModel->resetModel();
    }
    if (!mIsLoadedLayouts)
        for (int i = 0; i < mModel->columnCount(); ++i)
            ui->treeViewOrders->resizeColumnToContents(i);
    mPreloader->stopAnimation();
    SEConfig::restoreStateModel(ui->treeViewOrders);
    if (!ui->treeViewOrders->currentIndex().isValid() && mModel->getItems()->size())
        ui->treeViewOrders->selectionModel()->setCurrentIndex(
                    mModel->index(0, 0), QItemSelectionModel::SelectCurrent | QItemSelectionModel::Rows);
    if (!status && !message.isEmpty())
        QMessageBox::critical(0, tr("Ой, ошибка запроса!"), message);
}

void FormOrders::finishedShowInfoOrder(const bool status, const QString)
{    
    if (status && ApiAdapter::getApi()->getInfoOrder(mOrderInfo, true)) {
        mModelProducts->setListData(reinterpret_cast <ListDataItems *> (mOrderInfo->listOrderItems()));
        for (int i = 0; i < mModelProducts->columnCount(); i++)
            ui->treeViewProducts->resizeColumnToContents(i);
        mModelProducts->resetModel();
        ui->labelCutomer->setText(mOrderInfo->customer());
        ui->lineEditTypeDelivery->setText(mOrderInfo->deliveryName());
        ui->lineEditTimeCallDelivery->setText(mOrderInfo->deliveryCallTime());
        ui->lineEditEmailDelivery->setText(mOrderInfo->deliveryEmail());
        ui->lineEditPhoneDelivery->setText(mOrderInfo->deliveryPhone());
        ui->lineEditPostIndex->setText(mOrderInfo->deliveryPostIndex());
        ui->lineEditAddressDelivery->setText(mOrderInfo->deliveryAddress());        
        if (!mOrderInfo->property("deliveryNote").isNull())
            ui->lineEditDeliveryNote->setText(mOrderInfo->property("deliveryNote").toString());
        if (!mOrderInfo->property("deliveryCityId").isNull()) {
            ListDataItems cities;
            Request request;
            request.addId(mOrderInfo->property("deliveryCityId").toString());
            if (ApiAdapter::getApi()->getListCities(request, &cities) && cities.size()) {
                ui->lineEditDeliveryCity->setText(cities.at(0)->getName());
            }
        }
    }
}

void FormOrders::onClickButtonSubMenu()
{
    if (sender() == ui->pushButtonOrders)
        ui->stackedWidget->setCurrentIndex(0);
    if (sender() == ui->pushButtonPreorders) {
        ui->stackedWidget->setCurrentIndex(1);
        if (!mPreorders->size())
            refreshDataPreorders();
    }
    if (sender() == ui->pushButtonCreditRequest) {
        ui->stackedWidget->setCurrentIndex(2);
        if (!mCreditRequests->size())
            refreshDataCreditRequests();
    }

}

void FormOrders::storeHeaderSize()
{
    if (!mIsShowed)
        return;

    if (sender() == ui->treeViewOrders->header())
        SEConfig::saveTableState(ui->treeViewOrders->header(),
                             ui->treeViewOrders->objectName(), mDisplayMode + 1);

    if (sender() == ui->treeViewPreorders->header())
        SEConfig::saveTableState(ui->treeViewPreorders->header(),
                                ui->treeViewPreorders->objectName(), mDisplayMode + 1);

    if (sender() == ui->treeViewCreditRequests->header())
        SEConfig::saveTableState(ui->treeViewCreditRequests->header(),
                                 ui->treeViewCreditRequests->objectName(), mDisplayMode + 1);
}

void FormOrders::addEditPreorder()
{
    ui->toolButtonAddPreorder->setEnabled(false);
    ui->toolButtonEditPreorder->setEnabled(false);

    bool isNew = sender() ==  ui->toolButtonAddPreorder;
    if (!isNew && !ui->treeViewPreorders->currentIndex().isValid())
        return;

    DialogCardPreorder *card;

    if (isNew)
        card = new DialogCardPreorder(this);
    else {
        DataOrder *order = dynamic_cast <DataOrder *>
                (mModelPreorders->dataItem(ui->treeViewPreorders->currentIndex()));
        card = new DialogCardPreorder(this, order);
    }
    if (card->exec() == QDialog::Accepted)
        refreshDataPreorders();

    delete card;

    ui->toolButtonAddPreorder->setEnabled(true);
    onChangePreorder();
}

void FormOrders::removePreorder()
{
    QString msg;
    if (ui->treeViewPreorders->selectionModel()->selectedRows().count() > 1)
        msg = tr("Удалить выбранные предзаказы?");
    else msg = tr("Удалить предзаказ") + ": " +
            mModelPreorders->dataItem(ui->treeViewPreorders->currentIndex())->display() + "?";

    QMessageBox::StandardButton mesRepl;

    mesRepl = QMessageBox::question(this, tr("Подтверждение удаления"),
                                    msg, QMessageBox::Yes | QMessageBox::No);
    if(mesRepl == QMessageBox::Yes)
    {
        Request request;
        QModelIndexList list = ui->treeViewPreorders->selectionModel()->selectedRows();
        for (int i = 0; i < list.count(); i++)
            request.addId(mPreorders->at(list.at(i).row())->getId());

        if (list.count()) {
            ApiAdapter::getApi()->deletePreorders(request);
            refreshDataPreorders();
        }
    }
}

void FormOrders::addEditCreditRequest()
{
    ui->toolButtonAddCreditRequest->setEnabled(false);
    ui->toolButtonEditCreditRequest->setEnabled(false);

    bool isNew = sender() ==  ui->toolButtonAddCreditRequest;
    if (!isNew && !ui->treeViewCreditRequests->currentIndex().isValid())
        return;

    DialogCardCreditRequest *card;

    if (isNew)
        card = new DialogCardCreditRequest(this);
    else {
        DataOrder *order = dynamic_cast <DataOrder *>
                (mModelCreditRequests->dataItem(ui->treeViewCreditRequests->currentIndex()));
        card = new DialogCardCreditRequest(this, order);
    }
    if (card->exec() == QDialog::Accepted)
        refreshDataCreditRequests();

    delete card;

    ui->toolButtonAddCreditRequest->setEnabled(true);
    onChangeCreditRequest();
}

void FormOrders::removeCreditRequest()
{
    QString msg;
    if (ui->treeViewCreditRequests->selectionModel()->selectedRows().count() > 1)
        msg = tr("Удалить выбранные заявки?");
    else msg = tr("Удалить заявку") + ": " +
            mModelCreditRequests->dataItem(ui->treeViewCreditRequests->currentIndex())->display() + "?";

    QMessageBox::StandardButton mesRepl;

    mesRepl = QMessageBox::question(this, tr("Подтверждение удаления"),
                                    msg, QMessageBox::Yes | QMessageBox::No);
    if(mesRepl == QMessageBox::Yes)
    {
        Request request;
        QModelIndexList list = ui->treeViewCreditRequests->selectionModel()->selectedRows();
        for (int i = 0; i < list.count(); i++)
            request.addId(mCreditRequests->at(list.at(i).row())->getId());

        if (list.count()) {
            ApiAdapter::getApi()->deleteCreditRequests(request);
            refreshDataCreditRequests();
        }
    }
}

void FormOrders::sendRequestInBank()
{
    if (!ui->treeViewCreditRequests->currentIndex().isValid())
        return;

    DataOrder *order = dynamic_cast <DataOrder *>
            (mModelCreditRequests->dataItem(ui->treeViewCreditRequests->currentIndex()));
    QString params = order->getAsJsonString();
    if (ApiAdapter::getApi()->executeAPI(SEConsts::API_CREDIT_REQUESTS,
                                         SEConsts::APIMETHOD_SEND, params, 0)) {
        QMessageBox::information(this, tr("Отправка заявки в банк"),
                                 tr("Заявка в банк успешно отправлена!"), QMessageBox::Ok);
    }
}

void FormOrders::reject()
{
    if (mDisplayMode)
        QDialog::reject();
}

void FormOrders::onClickHeaderItem(int index)
{
    Q_UNUSED(index);    
    refreshData();
}

void FormOrders::goNextPage()
{
    if (sender() == mNavigator) {
        mNumPage++;
        if (mNumPage > int(mCountOrders / mNavigator->getLimit()))
            mNumPage = int(mCountOrders / mNavigator->getLimit());
        refreshData();
    }
    if (sender() == mNavigatorPreorders) {
        mNumPagePreorders++;
        if (mNumPagePreorders > int(mCountPreorders / mNavigatorPreorders->getLimit()))
            mNumPagePreorders = int(mCountPreorders / mNavigatorPreorders->getLimit());
        refreshDataPreorders();
    }
    if (sender() == mNavigatorCreditRequests) {
        mNumPageCreditRequests++;
        if (mNumPageCreditRequests > int(mCountCreditRequest / mNavigatorCreditRequests->getLimit()))
            mNumPageCreditRequests = int(mCountCreditRequest / mNavigatorCreditRequests->getLimit());
        refreshDataCreditRequests();
    }

}

void FormOrders::goPreviosPage()
{
    if (sender() == mNavigator) {
        mNumPage--;
        if (mNumPage < 0)
            mNumPage = 0;
        refreshData();
    }
    if (sender() == mNavigatorPreorders) {
        mNumPagePreorders--;
        if (mNumPagePreorders < 0)
            mNumPagePreorders = 0;
        refreshDataPreorders();
    }
    if (sender() == mNavigatorCreditRequests) {
        mNumPageCreditRequests--;
        if (mNumPageCreditRequests < 0)
            mNumPageCreditRequests = 0;
        refreshDataCreditRequests();
    }
}

void FormOrders::goLastPage()
{
    if (sender() == mNavigator) {
        mNumPage = int(mCountOrders / mNavigator->getLimit());
        refreshData();
    }
    if (sender() == mNavigatorPreorders) {
        mNumPagePreorders = int(mCountPreorders / mNavigatorPreorders->getLimit());
        refreshDataPreorders();
    }
    if (sender() == mNavigatorCreditRequests) {
        mNumPageCreditRequests = int(mCountPreorders / mNavigatorCreditRequests->getLimit());
        refreshDataCreditRequests();
    }

}

void FormOrders::goFirstPage()
{
    if (sender() == mNavigator) {
        mNumPage = 0;
        refreshData();
    }
    if (sender() == mNavigatorPreorders) {
        mNumPagePreorders = 0;
        refreshDataPreorders();
    }
    if (sender() == mNavigatorCreditRequests) {
        mNumPageCreditRequests = 0;
        refreshDataCreditRequests();
    }
}

void FormOrders::showEvent(QShowEvent *)
{
    mIsLoadedLayouts = SEConfig::restoreTableState(ui->treeViewOrders->header(),
                                                   ui->treeViewOrders->objectName(), mDisplayMode + 1);
    mIsLoadedLayoutsPreorders = SEConfig::restoreTableState(ui->treeViewPreorders->header(),
                                                   ui->treeViewPreorders->objectName(), mDisplayMode + 1);
    mIsLoadedLayoutsCreditRequests = SEConfig::restoreTableState(ui->treeViewCreditRequests->header(),
                                                   ui->treeViewCreditRequests->objectName(), mDisplayMode + 1);

    if (!SEConfig::loadSplitterLayout(ui->splitterOrders, mDisplayMode))
        setDefaultStateSplitter();
    if (mDisplayMode < 2 && !mIsShowed)
        refreshData();
    mIsShowed = true;
}

void FormOrders::timerEvent(QTimerEvent *e)
{
    killTimer(e->timerId());
    showInfoOrder();
}

void FormOrders::hideEvent(QHideEvent *)
{
    SEConfig::saveSplitterLayout(ui->splitterOrders, mDisplayMode);
}

void FormOrders::addEditOrder()
{
    ui->actionAddOrder->setEnabled(false);
    ui->actionEditOrder->setEnabled(false);

    bool isNew = sender() == ui->actionAddOrder;
    if (!isNew && !ui->treeViewOrders->currentIndex().isValid())
        return;

    FormOrderCard *card;    

    if (isNew)
        card = new FormOrderCard(this);    
    else {
        showInfoOrder();
        DataOrder *order = dynamic_cast <DataOrder *>
                (mModel->dataItem(ui->treeViewOrders->currentIndex()));
        card = new FormOrderCard(this, order);     
    }

    if (mDisplayMode == 2)
        card->setInfoMode();
    else connect(card, &FormOrderCard::saved, this, &FormOrders::onSaveOrder);

    card->exec();
    delete card;

    ui->actionAddOrder->setEnabled(true);
    onChangeOrder();
}

void FormOrders::delOrders()
{
    QString msg;
    if (ui->treeViewOrders->selectionModel()->selectedRows().count() > 1)
        msg = tr("Удалить выбранные заказы?");
    else msg = tr("Удалить заказ") + ": " +
            mModel->dataItem(ui->treeViewOrders->currentIndex())->display() + "?";

    QMessageBox::StandardButton mesRepl;

    mesRepl = QMessageBox::question(this, tr("Подтверждение удаления"),
                                    msg, QMessageBox::Yes | QMessageBox::No);
    if(mesRepl == QMessageBox::Yes)
    {
        Request request;

        QModelIndexList list = ui->treeViewOrders->selectionModel()->selectedRows();

        for (int i = 0; i < list.count(); i++)
            request.addId(mOrders->at(list.at(i).row())->getId());

        if (list.count()) {
            ApiAdapter::getApi()->deleteOrders(request);
            refreshData();
        }
    }
}

void FormOrders::setOrderStatus()
{
    DialogSetOrderStatus dialogSetStatus(this);
    if (sender() == ui->actionSetDeliveryStatus)
        dialogSetStatus.setTypeStatus(DialogSetOrderStatus::DELIVERY_STATUS);

    DataOrder order;
    QModelIndexList list = ui->treeViewOrders->selectionModel()->selectedRows();
    for (int i = 0; i < list.count(); i++)
        order.addId(mOrders->at(list.at(i).row())->getId());
    dialogSetStatus.setOrder(&order);

    if (dialogSetStatus.exec() == QDialog::Accepted)
        refreshData();
}

void FormOrders::cancelOrders()
{
    QString msg;
    if (ui->treeViewOrders->selectionModel()->selectedRows().count() > 1)
        msg = tr("Отменить выбранные заказы?");
    else msg = tr("Отменить заказ") + ": " +
            mModel->dataItem(ui->treeViewOrders->currentIndex())->display() + "?";

    QMessageBox::StandardButton mesRepl;

    mesRepl = QMessageBox::question(this, tr("Подтверждение отмены"),
                                    msg, QMessageBox::Yes | QMessageBox::No);
    if(mesRepl == QMessageBox::Yes)
    {
        DataOrder order;

        QModelIndexList list = ui->treeViewOrders->selectionModel()->selectedRows();

        for (int i = 0; i < list.count(); i++)
            order.addId(mOrders->at(list.at(i).row())->getId());
        order.setCanceled(true);

        if (list.count()) {
            ApiAdapter::getApi()->saveOrder(&order);
            refreshData();
        }
    }
}

void FormOrders::exportData()
{
    DialogExport dlgExport(this);
    dlgExport.setApiCatalogName(SEConsts::API_ORDERS);
    dlgExport.setFilter(mRequest);
    dlgExport.setFileName("orders");
    dlgExport.exec();
}

void FormOrders::exportItemData()
{
    if (!ui->treeViewOrders->currentIndex().isValid())
        return;

    setCursor(Qt::WaitCursor);
    DataOrder *order = qobject_cast <DataOrder *> (mModel->dataItem(ui->treeViewOrders->currentIndex()));
    Request request;
    request.addId(order->getId());
    QByteArray *bytes = new QByteArray;

    if (ApiAdapter::getApi()->exportItemData(SEConsts::API_ORDERS, request, bytes)) {
        QString folderExport = SEConfig::getFolderExport();
        SEConfig::createDir(folderExport);
        QString fileName = folderExport + QDir::separator() + "Заказ" + order->getId() + ".xlsx";
        QFile fileExport(fileName);
        if (fileExport.exists())
            fileExport.remove();
        if (fileExport.open(QFile::WriteOnly))
            fileExport.write(*bytes);
        fileExport.close();
        QDesktopServices::openUrl(QUrl::fromLocalFile(fileName));
    }
    setCursor(Qt::ArrowCursor);
}

void FormOrders::resetRequest()
{
    mRequest.clear();
    mRequest.setOffset(mNumPage * ui->widgetNavigator->getLimit());
    mRequest.setLimit(ui->widgetNavigator->getLimit());

    mRequest.clearFilter();
    QString filter;
    // от даты
    if (ui->checkBoxDateFrom->isChecked()) {
        filter = "[dateOrder]>='" + ui->dateEditDateFrom->date().toString("yyyy-MM-dd") + "'";
    }
    // до даты
    if (ui->checkBoxDateTo->isChecked()) {
        if (!filter.isEmpty())
            filter += " and ";
        filter += "[dateOrder]<='" + ui->dateEditDateTo->date().toString("yyyy-MM-dd") + "'";
    }
    // статус заказов
    if (mIsOnlyNotPaidOrders) {
        if (!filter.isEmpty())
            filter += " and ";
        filter += "[statusOrder] <> 'Y'";
    } else {
        if (ui->comboBoxOrderStatus->currentIndex()) {
            QString code = ui->comboBoxOrderStatus->currentData().toString();
            if (!filter.isEmpty())
                filter += " and ";
            filter += "[statusOrder] = '" + code + "'";
        }
    }
    // статус доставок
    if (ui->comboBoxDeliveryStatus->currentIndex()) {
        QString code = ui->comboBoxDeliveryStatus->currentData().toString();
        if (!filter.isEmpty())
            filter += " and ";
        filter += "[statusDelivery] = '" + code + "'";
    }
    // менеджеры
    if (ui->comboBoxManager->currentIndex()) {
        QString code = ui->comboBoxManager->currentData().toString();
        if (!filter.isEmpty())
            filter += " and ";
        filter += "[idAdmin] = '" + code + "'";
    }
    // удаленные
    if (ui->checkBoxIsDelete->isChecked()) {
        if (!filter.isEmpty())
            filter += " and ";
        filter += "[isDelete]";
    }
    if (!filter.isEmpty() && !ui->checkBoxIsDelete->isChecked())
        filter += "and not [isDelete]";
    if (!mMainFilter.isEmpty()) {
        if (!filter.isEmpty())
            filter += " and (" + mMainFilter + ")";
        else filter = mMainFilter;
    }

    mRequest.setFilter(filter);

    QString sortField = mFields->at(ui->treeViewOrders->header()->sortIndicatorSection())->name();
    mRequest.setSortBy(sortField);
    mRequest.setSearchText(ui->widgetSearch->searchText().trimmed());
    mRequest.setAscendingOrder(ui->treeViewOrders->header()->sortIndicatorOrder() ==
                               Qt::AscendingOrder);
}

void FormOrders::initActions()
{
    ui->toolButtonAdd->setDefaultAction(ui->actionAddOrder);
    ui->toolButtonEdit->setDefaultAction(ui->actionEditOrder);
    ui->toolButtonDelete->setDefaultAction(ui->actionDelOrder);
    ui->toolButtonSelect->setDefaultAction(ui->actionSelect);
    ui->toolButtonRefresh->setDefaultAction(ui->actionRefresh);
    ui->toolButtonOrderExport->setDefaultAction(ui->actionOrderExport);
    ui->toolButtonOrderCheckPay->setDefaultAction(ui->actionCheckPay);

}

void FormOrders::initTreeViewOrders()
{    
    ui->treeViewOrders->setModel(mModel);

    ui->treeViewOrders->header()->setSortIndicator(0, Qt::DescendingOrder);        
    ui->treeViewOrders->header()->setSortIndicatorShown(true);    
    ui->treeViewOrders->setAlternatingRowColors(true);

    connect(ui->treeViewOrders->selectionModel(),
            &QItemSelectionModel::currentRowChanged,
            this, &FormOrders::onChangeOrder);
    connect(ui->treeViewOrders->header(), &QHeaderView::sectionClicked,
            this, &FormOrders::onClickHeaderItem);
    ui->treeViewOrders->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->treeViewOrders, &QWidget::customContextMenuRequested,
            this, &FormOrders::showContextMenu);
}

void FormOrders::initTreeViewPreorders()
{
    ui->treeViewPreorders->setModel(mModelPreorders);

    ui->treeViewPreorders->header()->setSortIndicator(0, Qt::DescendingOrder);
    ui->treeViewPreorders->header()->setSortIndicatorShown(true);
    ui->treeViewPreorders->setAlternatingRowColors(true);

    connect(ui->treeViewPreorders->selectionModel(),
            &QItemSelectionModel::currentRowChanged, this, &FormOrders::onChangePreorder);
}

void FormOrders::initTreeViewCreditRequest()
{
    ui->treeViewCreditRequests->setModel(mModelCreditRequests);

    ui->treeViewCreditRequests->header()->setSortIndicator(0, Qt::DescendingOrder);
    ui->treeViewCreditRequests->header()->setSortIndicatorShown(true);
    ui->treeViewCreditRequests->setAlternatingRowColors(true);

    connect(ui->treeViewCreditRequests->selectionModel(),
            &QItemSelectionModel::currentRowChanged, this, &FormOrders::onChangeCreditRequest);
}

void FormOrders::initNavigator()
{
    mNavigator = ui->widgetNavigator;
    connect(mNavigator, SIGNAL(goNextPage()), this, SLOT(goNextPage()));
    connect(mNavigator, SIGNAL(goPreviosPage()), this, SLOT(goPreviosPage()));
    connect(mNavigator, SIGNAL(goLastPage()), this, SLOT(goLastPage()));
    connect(mNavigator, SIGNAL(goFirstPage()), this, SLOT(goFirstPage()));    
    connect(mNavigator, SIGNAL(refreshDataStart()), this, SLOT(goFirstPage()));
    connect(ui->widgetSearch, SIGNAL(runSearch()), SLOT(onRunSearch()));

    mNavigatorPreorders = ui->widgetNavigatorPreorders;
    connect(mNavigatorPreorders, SIGNAL(goNextPage()), this, SLOT(goNextPage()));
    connect(mNavigatorPreorders, SIGNAL(goPreviosPage()), this, SLOT(goPreviosPage()));
    connect(mNavigatorPreorders, SIGNAL(goLastPage()), this, SLOT(goLastPage()));
    connect(mNavigatorPreorders, SIGNAL(goFirstPage()), this, SLOT(goFirstPage()));
    connect(mNavigatorPreorders, SIGNAL(refreshDataStart()), this, SLOT(goFirstPage()));
    connect(ui->widgetSearchPreorders, SIGNAL(runSearch()), SLOT(onRunSearch()));

    mNavigatorCreditRequests = ui->widgetNavigatorCreditRequests;
    connect(mNavigatorCreditRequests, SIGNAL(goNextPage()), this, SLOT(goNextPage()));
    connect(mNavigatorCreditRequests, SIGNAL(goPreviosPage()), this, SLOT(goPreviosPage()));
    connect(mNavigatorCreditRequests, SIGNAL(goLastPage()), this, SLOT(goLastPage()));
    connect(mNavigatorCreditRequests, SIGNAL(goFirstPage()), this, SLOT(goFirstPage()));
    connect(mNavigatorCreditRequests, SIGNAL(refreshDataStart()), this, SLOT(goFirstPage()));
    connect(ui->widgetSearchCreditRequests, SIGNAL(runSearch()), SLOT(onRunSearch()));
}

void FormOrders::initVariables()
{
    mApi = new Api;
    mApiInfo = new Api;
    mOrderInfo = new DataOrder();
    mOrders = new ListOrders();
    mPreorders = new ListOrders();
    mCreditRequests = new ListOrders();

    mModel = new NameTableModel(this, reinterpret_cast<ListDataItems *>(mOrders));    
    mModelPreorders = new NameTableModel(this, reinterpret_cast<ListDataItems *>(mPreorders));
    mModelCreditRequests = new NameTableModel(this, reinterpret_cast<ListDataItems *>(mCreditRequests));
    mModelProducts = new NameTableModel(this);    

    mPreloader = new Preloader(this);    
    mNumPage = 0;
    mNumPagePreorders = 0;
    mNumPageCreditRequests = 0;
    mIdTimer = 0;
    mSelectedOrders = 0;        
    mIsOnlyNotPaidOrders = false;
    mIsShowed = false;
    mIsLoadedLayouts = false;
    mIsLoadedLayoutsPreorders = false;
    mIsLoadedLayoutsCreditRequests = false;
    ui->tabWidgetDetails->setCurrentIndex(0);
    ui->dateEditDateFrom->setDate(QDate::currentDate());
    ui->dateEditDateTo->setDate(QDate::currentDate());
    ui->groupBoxInfo->hide();
    ui->treeViewProducts->setModel(mModelProducts);
    if (mDisplayMode == 1) {
        ui->actionDelOrder->setVisible(false);
        ui->toolButtonDelete->hide();
        ui->toolButtonSelect->show();
    } else ui->toolButtonSelect->hide();

    initFields();
    initTreeViewOrders();
    initTreeViewPreorders();
    initTreeViewCreditRequest();
    initNavigator();
    initFilters();
}

void FormOrders::initFilters()
{
    ui->comboBoxManager->addItem(tr("Все"));
//    if (!SEConfig::isUserAdmin) {
//        ui->labelManager->hide();
//        ui->comboBoxManager->hide();
//    } else {
        ListContacts listManagers;
        if (ApiAdapter::getApi()->getListUsers(Request(), &listManagers)) {
            for (int i = 0; i < listManagers.size(); ++i) {
                ui->comboBoxManager->addItem(listManagers.at(i)->displayName(),
                                             listManagers.at(i)->getId());
            }
        }
        ui->comboBoxManager->setCurrentIndex(0);
//    }
    ui->comboBoxOrderStatus->addItem(tr("Все"));
    for (int i = 0; i < SEConfig::getOrdersStatuses()->size(); i++)
        ui->comboBoxOrderStatus->addItem(SEConfig::getOrdersStatuses()->at(i)->getName(),
                                         SEConfig::getOrdersStatuses()->at(i)->getId());
    ui->comboBoxDeliveryStatus->addItem(tr("Все"));
    for (int i = 0; i < SEConfig::getDeliveriesStatuses()->size(); i++)
        ui->comboBoxDeliveryStatus->addItem(SEConfig::getDeliveriesStatuses()->at(i)->getName(),
                                            SEConfig::getDeliveriesStatuses()->at(i)->getId());
}

void FormOrders::initSignals()
{    
    connect(ui->pushButtonOrders, &QAbstractButton::clicked, this, &FormOrders::onClickButtonSubMenu);
    connect(ui->pushButtonPreorders, &QAbstractButton::clicked, this, &FormOrders::onClickButtonSubMenu);
    connect(ui->pushButtonCreditRequest, &QAbstractButton::clicked, this, &FormOrders::onClickButtonSubMenu);

    connect(ui->toolButtonAddPreorder, SIGNAL(clicked(bool)), SLOT(addEditPreorder()));
    connect(ui->toolButtonEditPreorder, SIGNAL(clicked(bool)), SLOT(addEditPreorder()));
    connect(ui->toolButtonDeletePreorder, SIGNAL(clicked(bool)), SLOT(removePreorder()));
    connect(ui->toolButtonRefreshPreorders, SIGNAL(clicked(bool)), SLOT(refreshDataPreorders()));
    connect(ui->toolButtonSendToBank, SIGNAL(clicked(bool)), SLOT(sendRequestInBank()));
    connect(ui->treeViewPreorders, SIGNAL(doubleClicked(QModelIndex)), ui->toolButtonEditPreorder, SLOT(click()));

    connect(ui->toolButtonAddCreditRequest, SIGNAL(clicked(bool)), SLOT(addEditCreditRequest()));
    connect(ui->toolButtonEditCreditRequest, SIGNAL(clicked(bool)), SLOT(addEditCreditRequest()));
    connect(ui->toolButtonDeleteCreditRequest, SIGNAL(clicked(bool)), SLOT(removeCreditRequest()));
    connect(ui->toolButtonRefreshCreditRequests, SIGNAL(clicked(bool)), SLOT(refreshDataCreditRequests()));
    connect(ui->treeViewCreditRequests, SIGNAL(doubleClicked(QModelIndex)), ui->toolButtonEditCreditRequest, SLOT(click()));

    connect(ui->checkBoxDateFrom, static_cast<void (QCheckBox::*)(bool)>
            (&QAbstractButton::clicked), this, &FormOrders::refreshData);
    connect(ui->checkBoxDateFrom, static_cast<void (QCheckBox::*)(bool)>
            (&QAbstractButton::clicked), ui->dateEditDateFrom, &QWidget::setEnabled);
    connect(ui->dateEditDateFrom, &QDateEdit::userDateChanged, this, &FormOrders::refreshData);
    connect(ui->checkBoxDateTo, static_cast<void (QCheckBox::*)(bool)>
            (&QAbstractButton::clicked), this, &FormOrders::refreshData);
    connect(ui->checkBoxDateTo, static_cast<void (QCheckBox::*)(bool)>
            (&QAbstractButton::clicked), ui->dateEditDateTo, &QWidget::setEnabled);
    connect(ui->dateEditDateTo, &QDateEdit::userDateChanged, this, &FormOrders::refreshData);
    connect(ui->comboBoxOrderStatus, static_cast<void (QComboBox::*)(int)>
            (&QComboBox::currentIndexChanged), this, &FormOrders::refreshData);
    connect(ui->comboBoxDeliveryStatus, static_cast<void (QComboBox::*)(int)>
            (&QComboBox::currentIndexChanged), this, &FormOrders::refreshData);
    connect(ui->comboBoxManager, static_cast<void (QComboBox::*)(int)>
            (&QComboBox::currentIndexChanged), this, &FormOrders::refreshData);
    connect(ui->checkBoxIsDelete, static_cast<void (QCheckBox::*)(bool)>
            (&QAbstractButton::clicked), this, &FormOrders::refreshData);
    connect(ui->toolButtonCustomer, &QAbstractButton::clicked, this, &FormOrders::showContactCard);

    connect(ui->actionAddOrder, &QAction::triggered, this, &FormOrders::addEditOrder);
    connect(ui->actionEditOrder, &QAction::triggered, this, &FormOrders::addEditOrder);
    connect(ui->actionDelOrder, &QAction::triggered, this, &FormOrders::delOrders);
    connect(ui->actionRestore, &QAction::triggered, this, &FormOrders::restoreOrders);
    connect(ui->actionRefresh, &QAction::triggered, this, &FormOrders::refreshData);
    connect(ui->actionSetOrderStatus, &QAction::triggered, this, &FormOrders::setOrderStatus);
    connect(ui->actionSetDeliveryStatus, &QAction::triggered, this, &FormOrders::setOrderStatus);
    if (mDisplayMode == 1) {
        connect(ui->treeViewOrders, &QTreeView::doubleClicked, ui->actionSelect, &QAction::trigger);
        connect(ui->actionSelect, &QAction::triggered, this, &FormOrders::accept);
    }
    else
        connect(ui->treeViewOrders, &QTreeView::doubleClicked, ui->actionEditOrder, &QAction::trigger);
    connect(ui->toolButtonExport, &QAbstractButton::clicked, this, &FormOrders::exportData);    
    connect(ui->actionOrderExport, &QAction::triggered, this, &FormOrders::exportItemData);
    connect(ui->actionCheckPay, &QAction::triggered, this, &FormOrders::printCheckPayment);
    connect(ui->actionSetCanceled, &QAction::triggered, this, &FormOrders::cancelOrders);
    connect(mApi, SIGNAL(finishedRequest(bool,QString)), SLOT(finishedRefreshData(bool,QString)));
    connect(mApiInfo, SIGNAL(finishedRequest(bool,QString)), SLOT(finishedShowInfoOrder(bool,QString)));

    connect(ui->toolButtonOrderShipment, SIGNAL(clicked(bool)), SLOT(printShipmentBlank()));

    connect(ui->treeViewOrders->header(), SIGNAL(sectionResized(int,int,int)), SLOT(storeHeaderSize()));
    connect(ui->treeViewPreorders->header(), SIGNAL(sectionResized(int,int,int)), SLOT(storeHeaderSize()));
    connect(ui->treeViewCreditRequests->header(), SIGNAL(sectionResized(int,int,int)), SLOT(storeHeaderSize()));
}

void FormOrders::initFields()
{
    mModel->clearFields();
    mFields = new QVector<Field *>();
    mFields->append(new Field("id", "Ид.", Qt::AlignRight | Qt::AlignVCenter));
    mFields->append(new Field("dateOrder", "Дата заказа"));
    mFields->append(new Field("customer", "Заказчик"));
    mFields->append(new Field("customerPhone", "Телефон"));
    mFields->append(new Field("sum", "Сумма", Qt::AlignRight | Qt::AlignVCenter));
    mFields->append(new Field("dateDelivery", "Дата доставки"));
    mFields->append(new Field("statusOrder", "Статус заказы"));
    mFields->append(new Field("statusDelivery", "Статус доставки"));
    mFields->append(new Field("paymentTypePrimary", "Способ оплаты"));
    if (SEConfig::getMainInfo()->isShowDocDelivery())
        mFields->append(new Field("deliveryDocInfo", "Доставка"));
    mFields->append(new Field("note", "Примечание"));
    mFields->append(new Field("manager", "Менеджер"));
    mModel->appendFields(mFields);

    mModelProducts->clearFields();
    mFieldsProducts = new QVector<Field *>();    
    mFieldsProducts->append(new Field("article", "Артикул"));
    mFieldsProducts->append(new Field("name", "Наименование"));
    mFieldsProducts->append(new Field("count", "Кол-во", Qt::AlignRight | Qt::AlignVCenter));
    mFieldsProducts->append(new Field("price", "Цена", Qt::AlignRight | Qt::AlignVCenter));
    mFieldsProducts->append(new Field("discount", "Скидка", Qt::AlignRight | Qt::AlignVCenter));
    mFieldsProducts->append(new Field("cost", "Стоимость", Qt::AlignRight | Qt::AlignVCenter));
    mModelProducts->appendFields(mFieldsProducts);

    mModelPreorders->clearFields();
    mFieldsPreorders = new QVector<Field *>();
    mFieldsPreorders->append(new Field("id", "Ид.", Qt::AlignRight | Qt::AlignVCenter));
    mFieldsPreorders->append(new Field("dateOrder", "Дата"));
    mFieldsPreorders->append(new Field("customer", "Заказчик"));
    mFieldsPreorders->append(new Field("customerEmail", "Email"));
    mFieldsPreorders->append(new Field("customerPhone", "Телефон"));
    mFieldsPreorders->append(new Field("productName", "Наименование товара"));
    mFieldsPreorders->append(new Field("count", "Кол-во", Qt::AlignRight | Qt::AlignVCenter));
    mFieldsPreorders->append(new Field("isSentTitle", "Письмо"));
    mModelPreorders->appendFields(mFieldsPreorders);

    mModelCreditRequests->clearFields();
    mFieldsCreditRequests = new QVector<Field *>();
    mFieldsCreditRequests->append(new Field("id", "Ид.", Qt::AlignRight | Qt::AlignVCenter));
    mFieldsCreditRequests->append(new Field("dateOrder", "Дата"));
    mFieldsCreditRequests->append(new Field("customer", "Заказчик"));
    mFieldsCreditRequests->append(new Field("customerPhone", "Телефон"));
    mFieldsCreditRequests->append(new Field("customerINN", "ИИН"));
    mFieldsCreditRequests->append(new Field("requestStatus", "Статус"));
    mFieldsCreditRequests->append(new Field("sum", "Сумма", Qt::AlignRight | Qt::AlignVCenter));
    mFieldsCreditRequests->append(new Field("note", "Комментарий"));
    mModelCreditRequests->appendFields(mFieldsCreditRequests);
}

void FormOrders::initPermissions()
{
    isAddPermission = UserPermission::isAdd(UserPermission::ORDERS);
    isEditPermission = UserPermission::isEdit(UserPermission::ORDERS);
    isDeletePermission = UserPermission::isDelete(UserPermission::ORDERS);

    ui->toolButtonCustomer->setVisible(UserPermission::isRead(UserPermission::CONTACTS));
    ui->actionAddOrder->setEnabled(isAddPermission);
    ui->toolButtonAddPreorder->setEnabled(isAddPermission);    
}

void FormOrders::goToRowById(QString id)
{    
    if (!id.isEmpty()) {
            for (int i = 0; i < mOrders->size(); i ++)
                if (mOrders->at(i)->getId() == id) {
                    ui->treeViewOrders->selectionModel()->setCurrentIndex(
                                mModel->index(i, 0), QItemSelectionModel::SelectCurrent |
                                QItemSelectionModel::Rows);
                    ui->treeViewOrders->setFocus();
                    return;
                }
    }
    if (mOrders->size())
        ui->treeViewOrders->selectionModel()->setCurrentIndex(
                    mModel->index(0, 0), QItemSelectionModel::SelectCurrent |
                    QItemSelectionModel::Rows);
    else onChangeOrder();
    ui->treeViewOrders->setFocus();
}

void FormOrders::checkAccessFunctions()
{
    ui->actionEditOrder->setEnabled(ui->treeViewOrders->currentIndex().isValid() &&
                                    isEditPermission);
    ui->actionDelOrder->setEnabled(ui->treeViewOrders->currentIndex().isValid() &&
                                   isDeletePermission);
    ui->actionSetCanceled->setEnabled(ui->treeViewOrders->currentIndex().isValid() &&
                                      isDeletePermission);
    ui->actionSetOrderStatus->setEnabled(ui->treeViewOrders->currentIndex().isValid() &&
                                         isEditPermission);
    ui->actionSetDeliveryStatus->setEnabled(ui->treeViewOrders->currentIndex().isValid() &&
                                            isEditPermission);
    ui->actionOrderExport->setEnabled(ui->treeViewOrders->currentIndex().isValid());
    ui->actionCheckPay->setEnabled(ui->treeViewOrders->currentIndex().isValid());

    ui->toolButtonOrderShipment->setEnabled(ui->treeViewOrders->currentIndex().isValid());
}

void FormOrders::onSaveOrder()
{
    refreshData();
}

void FormOrders::setDefaultStateSplitter()
{
    QList<int> listSizes;
    listSizes.append(2 * height() / 3);
    listSizes.append(height() / 3);
    ui->splitterOrders->setSizes(listSizes);
}

void FormOrders::showContactCard()
{
    if (!ui->treeViewOrders->currentIndex().isValid())
        return;

    DataOrder *order = dynamic_cast <DataOrder *> (mModel->dataItem(ui->treeViewOrders->currentIndex()));
    qDebug() << order->property("idCompany").toString();

    if (order->property("idCompany").isNull()) {
        FormContactCard card(this);
        card.setIdContact(order->idCustomer(), true);
        if (card.exec() ==  QDialog::Accepted)
            refreshData();
    } else {
        FormCompanyCard card(this);
        card.setIdCompany(order->property("idCompany").toString(), true);
        if (card.exec() ==  QDialog::Accepted)
            refreshData();
    }
}

void FormOrders::showInfoOrder()
{
    if (!ui->treeViewOrders->currentIndex().isValid()) {
        ui->groupBoxInfo->hide();
        return;
    }

    mOrderInfo->setProperty("deliveryCityId", QVariant());
    mOrderInfo->setProperty("deliveryNote", QVariant());
    ui->groupBoxInfo->show();
    mOrderInfo->clearData();
    mOrderInfo->setId(mModel->dataItem(ui->treeViewOrders->currentIndex())->getId());    
    mModelProducts->unBindItems();
    mModelProducts->resetModel();
    ui->labelCutomer->clear();
    ui->lineEditTypeDelivery->clear();
    ui->lineEditTimeCallDelivery->clear();
    ui->lineEditEmailDelivery->clear();
    ui->lineEditPhoneDelivery->clear();
    ui->lineEditPostIndex->clear();
    ui->lineEditAddressDelivery->clear();
    ui->lineEditDeliveryNote->clear();
    ui->lineEditDeliveryCity->clear();
    Request request;
    request.addId(mOrderInfo->getId());
    mApiInfo->requestInThread(SEConsts::API_ORDERS, SEConsts::APIMETHOD_INFO, request);
}

void FormOrders::showContextMenu(QPoint point)
{
    if (mDisplayMode == 2)
        return;

    onChangeOrder();
    QPoint globalPos;

    if (sender()->inherits("QAbstractScrollArea"))
        globalPos = ((QAbstractScrollArea*)sender())->viewport()->mapToGlobal(point);
    else globalPos = ((QWidget*)sender())->mapToGlobal(point);

    QMenu menu;

    menu.addAction(ui->actionAddOrder);
    menu.addAction(ui->actionEditOrder);
    if (mDisplayMode == 1) {
        menu.addSeparator();
        menu.addAction(ui->actionSelect);
    } else menu.addAction(ui->actionDelOrder);
    if (ui->checkBoxIsDelete->isChecked())
        menu.addAction(ui->actionRestore);
    else menu.addAction(ui->actionSetCanceled);
    menu.addSeparator();
    menu.addAction(ui->actionSetOrderStatus);
    menu.addAction(ui->actionSetDeliveryStatus);
    menu.addSeparator();
    menu.addAction(ui->actionCheckPay);
    menu.addAction(ui->actionOrderExport);
    menu.exec(globalPos);
}

void FormOrders::printCheckPayment()
{
    if (!ui->treeViewOrders->currentIndex().isValid())
        return;

    setCursor(Qt::WaitCursor);
    DataOrder *order = qobject_cast <DataOrder *> (mModel->dataItem(ui->treeViewOrders->currentIndex()));
    Request request;
    request.addId(order->getId());
    QByteArray *bytes = new QByteArray;

    if (ApiAdapter::getApi()->getPaymentCheck(request, bytes)) {
        QString folderDocs = SEConfig::getFolderDocs();
        SEConfig::createDir(folderDocs);
        QString ext = "html";
        if (bytes->size() && bytes->at(0) == '%')
            ext = "pdf";
        QString fileName = folderDocs + QDir::separator() + "Счёт № " + order->getId() + "." + ext;
        QFile fileExport(fileName);
        if (fileExport.exists())
            fileExport.remove();
        if (fileExport.open(QFile::WriteOnly))
            fileExport.write(*bytes);
        fileExport.close();
        QDesktopServices::openUrl(QUrl::fromLocalFile(fileName));
    }
    setCursor(Qt::ArrowCursor);
}

void FormOrders::printShipmentBlank()
{
    if (!ui->treeViewOrders->currentIndex().isValid())
        return;

    setCursor(Qt::WaitCursor);
    DataOrder *order = qobject_cast <DataOrder *> (mModel->dataItem(ui->treeViewOrders->currentIndex()));
    Request request;
    request.addId(order->getId());
    QByteArray *bytes = new QByteArray;

    if (ApiAdapter::getApi()->getShipmentBlank(request, bytes)) {
        QString folderDocs = SEConfig::getFolderDocs();
        SEConfig::createDir(folderDocs);
        QString fileName = folderDocs + QDir::separator() + "Блан на доставку № " + order->getId() + ".pdf";
        QFile fileExport(fileName);
        if (fileExport.exists())
            fileExport.remove();
        if (fileExport.open(QFile::WriteOnly))
            fileExport.write(*bytes);
        fileExport.close();
        QDesktopServices::openUrl(QUrl::fromLocalFile(fileName));
    }
    setCursor(Qt::ArrowCursor);
}

void FormOrders::refreshData()
{ 
    resetRequest();
    mNavigator->setNumRecord(0, 0);
    ui->lineEditAmount->setText(formatNumberString(QString::number(0, 'f', 2)));
    SEConfig::saveStateModel(ui->treeViewOrders);
    mPreloader->startAnimation();
    mApi->requestInThread(SEConsts::API_ORDERS, SEConsts::APIMETHOD_FETCH, mRequest);
}

void FormOrders::refreshDataPreorders()
{
    mNavigatorPreorders->setNumRecord(0, 0);
    if (mApi->getListPreorders(Request(), mPreorders)) {
        mCountPreorders = mPreorders->countInBase();
        mModelPreorders->resetModel();
        if (!mIsLoadedLayoutsPreorders)
            for (int i = 0; i < mModelPreorders->columnCount(); ++i)
                ui->treeViewPreorders->resizeColumnToContents(i);
        if (!ui->treeViewPreorders->currentIndex().isValid() && mModelPreorders->getItems()->size())
            ui->treeViewPreorders->selectionModel()->setCurrentIndex(
                        mModelPreorders->index(0, 0), QItemSelectionModel::SelectCurrent | QItemSelectionModel::Rows);
    }
}

void FormOrders::refreshDataCreditRequests()
{
    mNavigatorCreditRequests->setNumRecord(0, 0);
    if (mApi->getListCreditRequests(Request(), mCreditRequests)) {
        mCountCreditRequest = mCreditRequests->countInBase();
        mModelCreditRequests->resetModel();
        if (!mIsLoadedLayoutsCreditRequests)
            for (int i = 0; i < mModelCreditRequests->columnCount(); ++i)
                ui->treeViewCreditRequests->resizeColumnToContents(i);
        if (!ui->treeViewCreditRequests->currentIndex().isValid() && mModelCreditRequests->getItems()->size())
            ui->treeViewCreditRequests->selectionModel()->setCurrentIndex(
                        mModelCreditRequests->index(0, 0), QItemSelectionModel::SelectCurrent | QItemSelectionModel::Rows);

    }
}


void FormOrders::restoreOrders()
{
    if (!ui->treeViewOrders->currentIndex().isValid())
        return;

    QString msg;
    if (ui->treeViewOrders->selectionModel()->selectedRows().count() > 1)
        msg = tr("Восстановить выбранные заказы?");
    else msg = tr("Восстановить заказ") + ": " +
            mModel->dataItem(ui->treeViewOrders->currentIndex())->display() + "?";

    QMessageBox::StandardButton mesRepl;

    mesRepl = QMessageBox::question(this, tr("Подтверждение восстановления"),
                                    msg, QMessageBox::Yes | QMessageBox::No);
    if(mesRepl == QMessageBox::Yes)
    {
        DataOrder order;

        QModelIndexList list = ui->treeViewOrders->selectionModel()->selectedRows();

        for (int i = 0; i < list.count(); i++)
            order.addId(mOrders->at(list.at(i).row())->getId());

        if (list.count()) {
            ApiAdapter::getApi()->restoreOrder(&order);
            refreshData();
        }
    }
}
