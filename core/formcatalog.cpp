#include "formcatalog.h"
#include "userpermission.h"

FormCatalog::FormCatalog(QWidget *parent, const QString &apiObject, const QString &permissionObject) :
    QDialog(parent)
{    
    this->apiObject = apiObject;
    this->permissionObject = permissionObject;
    this->items = new ListDataItems;

    initVariables();
    initControls();
    initSignals();
    initPermissions();
}

FormCatalog::FormCatalog(QWidget *parent, ListDataItems *items) :
    QDialog(parent)
{
    this->items = items;

    initVariables();
    initControls();
    initSignals();
    initPermissions();
}

FormCatalog::~FormCatalog()
{    
    apiFetch->deleteLater();
    apiInfo->deleteLater();
}

void FormCatalog::setSelectMode(const bool &isMultiSelect)
{
    isSelectMode = true;
    this->isMultiSelect = isMultiSelect;
    if (!isMultiSelect)
        treeViewItems->setSelectionMode(QAbstractItemView::ExtendedSelection);
    else treeViewItems->setSelectionMode(QAbstractItemView::SingleSelection);
}

void FormCatalog::setTitleButtonAdd(const QString &title, const QString &toolTip)
{
    actionAdd->setText(title);
    actionAdd->setToolTip(toolTip);
}

void FormCatalog::setVisiblePaginator(const bool &visible)
{
    paginator->setVisible(visible);
}

void FormCatalog::setVisibleSearch(const bool &visible)
{
    widgetSearch->setVisible(visible);
}

void FormCatalog::setRootIsDecorated(const bool &isDecorated)
{
    treeViewItems->setRootIsDecorated(isDecorated);
}

void FormCatalog::setSortingEnabled(const bool &isEnabled)
{
    treeViewItems->setSortingEnabled(isEnabled);
}

void FormCatalog::setIsCheckedMode(const bool &isCheckedMode)
{
    modelItems->setCheckedMode(isCheckedMode);
}

void FormCatalog::setDialogCard(DialogCard *dialogCard)
{
    this->dialogCard = dialogCard;
}

void FormCatalog::setRefreshModeAfterChange()
{
    mIsRefreshAfterChange = true;
}

NameTableModel *FormCatalog::getModelItems() const
{
    return modelItems;
}

void FormCatalog::refreshData()
{    
    actionEdit->setEnabled(false);
    actionDelete->setEnabled(false);

    paginator->setNumRecord(0, 0);
    SEConfig::saveStateModel(treeViewItems);
    if (!apiObject.isEmpty()) {
        resetRequest();
        preloader->startAnimation();
        apiFetch->requestInThread(apiObject, SEConsts::APIMETHOD_FETCH, request);
    } else {
        modelItems->resetModel();
        setStateTree();
    }
}

void FormCatalog::refreshCurrentInfo()
{

}

void FormCatalog::createItem()
{
    if (!dialogCard)
        return;

    dialogCard->setItem();
    if (dialogCard->exec() == QDialog::Accepted) {
        if (mIsRefreshAfterChange)
            refreshData();
        else {
            DataItem *item = new DataItem(dialogCard->getItem());
            modelItems->addItem(item, false);
            countItems++;
            treeViewItems->selectionModel()->clearSelection();
            treeViewItems->selectionModel()->
                    setCurrentIndex(modelItems->index(0, 0), QItemSelectionModel::SelectCurrent |
                                    QItemSelectionModel::Rows);            
        }
        emit modified();
    }
}

void FormCatalog::editItem()
{
    if (!dialogCard || !treeViewItems->currentIndex().isValid())
        return;

    DataItem *item = modelItems->dataItem(treeViewItems->currentIndex());
    dialogCard->setItem(item);
    if (dialogCard->exec() == QDialog::Accepted) {
        if (mIsRefreshAfterChange)
            refreshData();
        else {
            item->copy(dialogCard->getItem());
            modelItems->setData(modelItems->index(0, treeViewItems->currentIndex().row()), item->getId(),
                                Qt::EditRole);            
        }
        emit modified();
    }
}

void FormCatalog::deleteItem()
{
    if (!treeViewItems->currentIndex().isValid())
        return;

    Request request;
    QModelIndexList list = treeViewItems->selectionModel()->selectedRows();

    if (apiObject.isNull()) {
        QStringList delList;
        for (int i = 0; i < list.count(); i++)
            delList.append(modelItems->dataItem(list.at(i))->getId());
        for (int i = 0; i < items->size(); i++)
            if (delList.contains(items->at(i)->getId()))
                items->at(i)->setIsSelected(true);
        for (int i = items->size() - 1; i >= 0; i--)
            if (items->at(i)->getIsSelected())
                delete items->takeAt(i);
        modelItems->resetModel();
        items->setModified();
        emit modified();
        return;
    }

    QString msg;
    if (treeViewItems->selectionModel()->selectedRows().count() > 1)
        msg = tr("Удалить выбранные элементы?");
    else {
        if (!modelItems->dataItem(treeViewItems->currentIndex())->getName().isEmpty())
            msg = tr("Удалить элемент") + ": " +
                modelItems->dataItem(treeViewItems->currentIndex())->getName() + "?";
        else msg = tr("Удалить выбранный элемент?");
    }

    QMessageBox::StandardButton mesRepl;

    mesRepl = QMessageBox::question(this, tr("Подтверждение удаления"),
                                    msg, QMessageBox::Yes | QMessageBox::No);
    if(mesRepl == QMessageBox::Yes)
    {
        for (int i = 0; i < list.count(); i++)
                request.addId(modelItems->dataItem(list.at(i))->getId());

        if (list.count()) {
            ApiAdapter::getApi()->deleteItem(apiObject, request);
            refreshData();
        }
    }
}

void FormCatalog::initVariables()
{    
    apiFetch = new Api;
    apiInfo = new Api;
    dialogCard = 0;
    preloader = new Preloader(this);
    isRefreshOnShow = false;
    isShown = false;
    isRestoreStates = false;
    isRefreshedCurrentInfo = false;
    isSelectMode = false;
    mIsRefreshAfterChange = false;
    numPage = 0;
    timerRefreshCurrentInfo = new QTimer(this);
    timerRefreshCurrentInfo->setSingleShot(true);
    timerSearch = new QTimer(this);
    timerSearch->setSingleShot(true);
}

void FormCatalog::initControls()
{
    initActions();
    initButtons();
    initTreeView();

    modelItems = new NameTableModel(treeViewItems, items, isSelectMode);
    modelItems->clearFields();
    treeViewItems->setModel(modelItems);

    widgetSearch = new FrameSearchTable(this);
    widgetSearch->setIsHotSearchMode();
    paginator = new FramePageNavigator(this);
    paginator->hide();

    QVBoxLayout *layoutMain = new QVBoxLayout;
    layoutMain->addLayout(&layoutButtons);
    layoutMain->addWidget(widgetSearch);
    layoutMain->addWidget(treeViewItems);
    layoutMain->addWidget(paginator);

    if (!isSelectMode) {
        layoutMain->setMargin(6);
        layoutMain->setContentsMargins(2, 2, 2, 2);
    }
    else layoutMain->setMargin(8);
    layoutMain->setSpacing(2);    

    setLayout(layoutMain);
    restoreStateTree();
}

void FormCatalog::initActions()
{
    actionAdd = new QAction(QIcon(":/32/icons/32x32/add.png"), "Создать", this);
    actionAdd->setToolTip("Создать новый элемент");
    actionAdd->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Insert));
    addAction(actionAdd);
    actionEdit = new QAction(QIcon(":/32/icons/32x32/edit.png"), "Изменить", this);
    actionEdit->setToolTip("Редактировать");
    addAction(actionEdit);
    actionDelete = new QAction(QIcon(":/32/icons/32x32/delete.png"), "Удалить", this);
    actionDelete->setToolTip("Удалить выбранный элемент");
    actionDelete->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Delete));
    addAction(actionEdit);
    actionRefresh = new QAction(QIcon(":/32/icons/32x32/refresh.png"), "Обновить", this);
    actionRefresh->setToolTip("Обновить данные");
    actionRefresh->setShortcut(QKeySequence(Qt::Key_F5));
    addAction(actionRefresh);
}

void FormCatalog::initButtons()
{
    QToolButton *button = new QToolButton;
    button->setDefaultAction(actionAdd);
    button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    button->setIconSize(QSize(24, 24));
    layoutButtons.addWidget(button);

    button = new QToolButton;
    button->setDefaultAction(actionEdit);
    button->setIconSize(QSize(24, 24));
    layoutButtons.addWidget(button);

    button = new QToolButton;
    button->setDefaultAction(actionDelete);
    button->setIconSize(QSize(24, 24));
    layoutButtons.addWidget(button);

    if (!apiObject.isNull()) {
        QFrame *line = new QFrame;
        line->setFrameShape(QFrame::VLine);
        line->setFrameShadow(QFrame::Sunken);
        layoutButtons.addWidget(line);

        button = new QToolButton;
        button->setDefaultAction(actionRefresh);
        button->setIconSize(QSize(24, 24));
        layoutButtons.addWidget(button);
    }

    layoutButtons.addStretch();
    layoutButtons.setSpacing(2);
    layoutButtons.setMargin(2);
}

void FormCatalog::initSignals()
{    
    connect(apiFetch, SIGNAL(finishedRequest(bool,QString)), SLOT(finishedRefreshList(bool,QString)));

    connect(widgetSearch, SIGNAL(runSearch()), SLOT(runSearch()));

    connect(treeViewItems->header(), SIGNAL(sectionClicked(int)), SLOT(refreshData()));
    connect(treeViewItems->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)), SLOT(currentItemChaned()));
    connect(treeViewItems, SIGNAL(customContextMenuRequested(QPoint)), SLOT(showContextMenu(QPoint)));
    connect(treeViewItems, SIGNAL(doubleClicked(QModelIndex)), actionEdit, SLOT(trigger()));

    connect(actionAdd, SIGNAL(triggered(bool)), SLOT(createItem()));
    connect(actionEdit, SIGNAL(triggered(bool)), SLOT(editItem()));
    connect(actionDelete, SIGNAL(triggered(bool)), SLOT(deleteItem()));
    connect(actionRefresh, SIGNAL(triggered(bool)), SLOT(refreshData()));

    connect(paginator, SIGNAL(goNextPage()), this, SLOT(goNextPage()));
    connect(paginator, SIGNAL(goPreviosPage()), this, SLOT(goPreviosPage()));
    connect(paginator, SIGNAL(goLastPage()), this, SLOT(goLastPage()));
    connect(paginator, SIGNAL(goFirstPage()), this, SLOT(goFirstPage()));
    connect(paginator, SIGNAL(refreshDataStart()), this, SLOT(goFirstPage()));

    connect(timerRefreshCurrentInfo, &QTimer::timeout, this, &FormCatalog::refreshCurrentInfo);
    connect(timerSearch, &QTimer::timeout, this, &FormCatalog::goFirstPage);

    connect(modelItems, SIGNAL(checked(QString,bool)), SLOT(onCheckedItems()));
}

void FormCatalog::initTreeView()
{
    treeViewItems = new QTreeView;
    treeViewItems->setObjectName(QString("treeView" + apiObject));
    treeViewItems->setSelectionBehavior(QAbstractItemView::SelectRows);
    treeViewItems->setSelectionMode(QAbstractItemView::ExtendedSelection);
    treeViewItems->setRootIsDecorated(true);
    treeViewItems->header()->setSortIndicatorShown(true);
    treeViewItems->header()->setSortIndicator(0, Qt::DescendingOrder);
    treeViewItems->header()->setSectionsMovable(true);
    treeViewItems->setContextMenuPolicy(Qt::CustomContextMenu);
    treeViewItems->setSortingEnabled(true);
}

void FormCatalog::resetRequest()
{
    request.clear();

    QString sortField = modelItems->getFields().at(treeViewItems->header()->sortIndicatorSection())->name();
    request.setSortBy(sortField);
    request.setSearchText(widgetSearch->searchText().trimmed());
    request.setAscendingOrder(treeViewItems->header()->sortIndicatorOrder() == Qt::AscendingOrder);

    request.setOffset(numPage * paginator->getLimit());
    request.setLimit(paginator->getLimit());
}

void FormCatalog::finishedRefreshList(const bool status, const QString message)
{
    if (status &&
            apiFetch->getListData(apiObject, items, request, true)) {
        countItems = items->countInBase();
        modelItems->resetModel();
        if (!isShown)
            paginator->setVisible(countItems > paginator->getLimit());
        isShown = true;        
    }
    preloader->stopAnimation();    

    setStateTree();
    if (!status && !message.isEmpty())
        QMessageBox::critical(0, tr("Ой, ошибка запроса!"), message);
}

void FormCatalog::finishedRefreshCurrentInfo(const bool /*status*/, const QString )
{
//    if (status && ApiAdapter::api()->getInfoProduct(mProductInfo, true)) {
//        mModelImages->setListData(mProductInfo->listImages());
//        mModelImages->resetModel();
//    }
}

void FormCatalog::currentItemChaned()
{
    if (items->size() > 0 && treeViewItems->currentIndex().isValid())
        paginator->setNumRecord(numPage * paginator->getLimit() + treeViewItems->currentIndex().row() + 1, countItems);
    else paginator->setNumRecord(0, 0);

    QModelIndex index = treeViewItems->currentIndex();

    actionEdit->setEnabled(index.isValid() && isEditPermission);
    actionDelete->setEnabled(index.isValid() && isDeletePermission);

    if (timerRefreshCurrentInfo->isActive())
        timerRefreshCurrentInfo->stop();
    timerRefreshCurrentInfo->start(300);
}

void FormCatalog::runSearch()
{
    if (timerSearch->isActive())
        timerSearch->stop();
    timerSearch->start(300);
}

void FormCatalog::onCheckedItems()
{
    QStringList listIds;
    for (int i = 0; i < items->size(); ++i) {
        if (items->at(i)->getIsChecked())
            listIds.append(items->at(i)->getId());
    }
    emit checked(listIds);
}

void FormCatalog::goNextPage()
{
    numPage++;
    if (numPage > int(countItems / paginator->getLimit()))
        numPage = int(countItems / paginator->getLimit());
    refreshData();
}

void FormCatalog::goPreviosPage()
{
    numPage--;
    if (numPage < 0)
        numPage = 0;
    refreshData();
}

void FormCatalog::goLastPage()
{
    numPage = int(countItems / paginator->getLimit());
    refreshData();
}

void FormCatalog::goFirstPage()
{
    numPage = 0;
    refreshData();
}

void FormCatalog::showContextMenu(QPoint point)
{
    currentItemChaned();
    QPoint globalPos;

    if (sender()->inherits("QAbstractScrollArea"))
        globalPos = ((QAbstractScrollArea*)sender())->viewport()->mapToGlobal(point);
    else globalPos = ((QWidget*)sender())->mapToGlobal(point);

    QMenu menu;
    menu.addAction(actionAdd);
    menu.addAction(actionEdit);
    menu.addAction(actionDelete);
    if (!apiObject.isNull()) {
        menu.addSeparator();
        menu.addAction(actionRefresh);
    }
    menu.exec(globalPos);
}

void FormCatalog::showEvent(QShowEvent *)
{    
    if (isRefreshOnShow || !isShown)
        refreshData();
}

void FormCatalog::hideEvent(QHideEvent *)
{
    if (isShown)
        saveStateTree();
}

void FormCatalog::initPermissions()
{
    isAddPermission = permissionObject.isEmpty() ? true : UserPermission::isAdd(UserPermission::PRODUCTS);
    isEditPermission = permissionObject.isEmpty() ? true : UserPermission::isEdit(UserPermission::PRODUCTS);
    isDeletePermission = permissionObject.isEmpty() ? true : UserPermission::isDelete(UserPermission::PRODUCTS);

    actionAdd->setEnabled(isAddPermission);
    actionEdit->setEnabled(isEditPermission);
    actionDelete->setEnabled(isDeletePermission);
}

void FormCatalog::restoreStateTree()
{
    isRestoreStates = SEConfig::restoreTableState(treeViewItems->header(), treeViewItems->objectName(), isSelectMode);
}

void FormCatalog::setStateTree()
{
    if (!isRestoreStates)
        setStateTreeDefault();
    SEConfig::restoreStateModel(treeViewItems);
    if (!treeViewItems->currentIndex().isValid() && items->size())
        treeViewItems->selectionModel()->setCurrentIndex(
                    modelItems->index(0, 0), QItemSelectionModel::SelectCurrent | QItemSelectionModel::Rows);
}

void FormCatalog::setStateTreeDefault()
{
    for (int i = 0; i < modelItems->columnCount() - 1; i++)
        treeViewItems->resizeColumnToContents(i);
    isRestoreStates = true;
}

void FormCatalog::saveStateTree()
{
    SEConfig::saveTableState(treeViewItems->header(), treeViewItems->objectName(), isSelectMode);
}

void FormCatalog::addField(Field *field)
{
    modelItems->appendField(field);

}
