#include "formproductsgroups.h"
#include "ui_formproductsgroups.h"

#include <QtWidgets>
#include <QMenu>
#include <QPoint>
#include <QThread>

#include "seconfig.h"
#include "apiadapter.h"
#include "request.h"
#include "userpermission.h"
#include "dialogexport.h"
#include "dialoggroupsmasterimport.h"

FormProductsGroups::FormProductsGroups(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FormProductsGroups)
{
    mSelectMode = false;
    mIsShowed = false;
    ui->setupUi(this);
    ui->toolButtonSelect->setHidden(true);
    mGroups = new ListProductsGroups;
    mExludingGroups = new ListProductsGroups;    
    mIdTimer = 0;      
    mFormSetColumns = 0;
    mApi = new Api;
    setWindowFlags(Qt::Widget);    
    initSignals();
    initActions();
    setContextMenu();
    initFields();    
    initPermissions();
    ui->treeViewGroups->installEventFilter(this);
}

FormProductsGroups::~FormProductsGroups()
{
    delete mExludingGroups;
    delete ui;
    mApi->deleteLater();
}

QStringList FormProductsGroups::getIdsGroupsChecked()
{
    return mModel->idsChecked();
}

void FormProductsGroups::setSelectMode(const bool &isMultiSelect)
{
    mSelectMode = true;
    setWindowFlags(Qt::Dialog);

    if (isMultiSelect)
        ui->treeViewGroups->setSelectionMode(QAbstractItemView::ExtendedSelection);
    else  ui->treeViewGroups->setSelectionMode(QAbstractItemView::SingleSelection);

    ui->toolButtonEditGroup->setHidden(true);
    ui->toolButtonDeleteGroup->setHidden(true);    
    ui->line->setHidden(true);    
    ui->toolButtonMoveDown->setHidden(true);
    ui->toolButtonMoveUp->setHidden(true);
    ui->toolButtonSelect->setVisible(true);
}

void FormProductsGroups::refreshData(const QString &idParent, const QModelIndex &index)
{
    if ((index.isValid() && mModel->dataItem(index)->getIsFilledChilds()) ||
            !UserPermission::isRead(UserPermission::PRODUCTS))
        return;

    setCursor(QCursor(Qt::WaitCursor));
    disconnect(ui->treeViewGroups, SIGNAL(expanded(QModelIndex)));
    resetRequest(idParent);
    SEConfig::saveStateModel(ui->treeViewGroups);
    disconnect(ui->treeViewGroups->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)));    
    mApi->requestInThread(SEConsts::API_PRODUCTS_GROUPS, SEConsts::APIMETHOD_FETCH, mRequest);
}

void FormProductsGroups::showEvent(QShowEvent *)
{
    if (!mIsShowed) {
        mModel = new NameTableModel(this, reinterpret_cast<ListDataItems *>(mGroups));
        mModel->setIsHandleEmtyIditem(true);
        mModel->clearFields();
        foreach (Field *field, mFields)
            mModel->appendField(field);
        if (!mSelectMode)
            mModel->setCheckedMode(true);
        else ui->toolButtonImport->setHidden(true);
        ui->treeViewGroups->setModel(mModel);
        mIsRestoreStates = SEConfig::restoreTableState(ui->treeViewGroups->header(), ui->treeViewGroups->objectName());        
        connect(mModel, SIGNAL(checked(QString,bool)), SLOT(onItemChecked()));
        connect(mModel, SIGNAL(rowsInserted(QModelIndex,int,int)), SLOT(onMoved(QModelIndex,int,int)));        
        refreshData();
    }
}

void FormProductsGroups::hideEvent(QHideEvent *)
{  
    SEConfig::saveTableState(ui->treeViewGroups->header(),
                             ui->treeViewGroups->objectName());
}

void FormProductsGroups::timerEvent(QTimerEvent *event)
{
    killTimer(event->timerId());
    saveSortGroups();
}


bool FormProductsGroups::eventFilter(QObject *obj, QEvent *e)
{
    if (obj == ui->treeViewGroups) {
        if (e->type() == QEvent::KeyPress) {
            QKeyEvent *keyEvent = static_cast <QKeyEvent *> (e);
            if (keyEvent->key() == Qt::Key_Escape && !mSelectMode)
                return true;
            if (keyEvent->modifiers() & Qt::ControlModifier) {
                switch (keyEvent->key()) {
                case Qt::Key_A: selectAll();
                    return true;
                default:
                    break;
                }
            }
            if (keyEvent->modifiers() & Qt::ControlModifier) {
                switch (keyEvent->key()) {
                case 1045:
                case 'T':  if (ui->actionSetCodeUrl->isEnabled())
                        ui->actionSetCodeUrl->trigger();
                    break;
                    break;
                default:
                    break;
                }
            }
        }
    }
    return false;
}

void FormProductsGroups::showContextMenu(QPoint point)
{
    onChangeGroup();

    QPoint globalPos;

    if (sender()->inherits("QAbstractScrollArea"))
        globalPos = ((QAbstractScrollArea*)sender())->viewport()->mapToGlobal(point);
    else globalPos = ((QWidget*)sender())->mapToGlobal(point);

    QModelIndexList list = ui->treeViewGroups->selectionModel()->selectedRows();
    if (list.size() > 1)
        ui->actionEditGroup->setText(tr("Мультиредактирование групп"));
    else ui->actionEditGroup->setText(tr("Редактировать группу"));

    QMenu menu;
    if (mSelectMode)
        menu.addAction(ui->actionSelect);
    else {
        menu.addAction(ui->actionAddGroup);
        menu.addAction(ui->actionAddSubGroup);
        menu.addAction(ui->actionEditGroup);
        menu.addAction(ui->actionDeleteGroups);
        menu.addSeparator();
        menu.addAction(ui->actionSetParentRoot);
        menu.addAction(ui->actionSetParentGroup);
        menu.addSeparator();
        menu.addAction(ui->actionSetCodeUrl);
        menu.addSeparator();
        menu.addAction(ui->actionUp);
        menu.addAction(ui->actionDown);
        if (SEConfig::getMainInfo()->isShowIncPrices()) {
            menu.addSeparator();
            menu.addAction(ui->actionSetIncPrices);
        }
    }
    menu.exec(globalPos);
}

void FormProductsGroups::finishedRefreshData(const bool status, const QString message)
{    
    if (status) {
        storeCheckedItems();
        ApiAdapter::getApi()->getListProductsGroups(mRequest, mGroups, true);
        ui->lineEditCount->setText(QString::number(mGroups->size()));
        for (int i = 0; i < mExludingGroups->size(); i++)
            for (int j = 0; j < mGroups->size(); j++)
                if (mGroups->at(j)->getId() == mExludingGroups->at(i)->getId()) {
                    mGroups->deleteItem(mGroups->at(j));
                    break;
                }        
        if (!mSelectMode) {
            DataProductsGroup *group = new DataProductsGroup;
            group->setName("БЕЗ ГРУППЫ");
            mGroups->prepend(group);
        }
        restoreCheckedItems();
        mModel->resetModel();        
        connect(ui->treeViewGroups->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
                SLOT(onChangeGroup()));
        if (mIsShowed)
            SEConfig::restoreStateModel(ui->treeViewGroups);
        if (!mIsRestoreStates)
            for (int i = 0; i < mModel->columnCount() - 1; ++i)
                ui->treeViewGroups->resizeColumnToContents(i);        
        if (!ui->treeViewGroups->currentIndex().isValid())
            goToRowById(QString());
        mIsShowed = true;
    } else QMessageBox::critical(0, tr("Ой, ошибка запроса!"), message);
    setCursor(QCursor(Qt::ArrowCursor));
}

void FormProductsGroups::initWidgetGroups()
{
    ui->treeViewGroups->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->treeViewGroups->setSortingEnabled(true);
}

void FormProductsGroups::initPermissions()
{

}

void FormProductsGroups::saveSortGroups()
{
    ApiAdapter::getApi()->sortProductsGroups(mModel->getSortParamsForApi());
}

void FormProductsGroups::expandCollapseTree()
{
    if (sender() == ui->toolButtonExpandAll)
        ui->treeViewGroups->expandAll();
    else ui->treeViewGroups->collapseAll();
}

void FormProductsGroups::checkedUncheckedTree()
{
    bool isChecked = sender() == ui->toolButtonCheckedAll;
    mModel->beginReset();
    for (int i = 0; i < mGroups->size(); ++i)
        mGroups->at(i)->setIsChecked(isChecked);
    mModel->endReset();
    ui->treeViewGroups->expandAll();
    onItemChecked();
}

void FormProductsGroups::setGroups(const ListProductsGroups &groups, const QModelIndex &index)
{
    if (!index.isValid()) {
        mGroups->deleteItems();
        mGroups->clear();
        for (int i = 0; i < groups.size(); ++i)
            mGroups->append(new DataProductsGroup(groups.at(i)));
    } else {
        DataItem *parentGroup = mModel->dataItem(index);
        if (parentGroup->getIsFilledChilds())
            return;
        parentGroup->setIsFilledChilds(true);
        for (int i = 0; i < groups.size(); ++i) {
            DataProductsGroup *group = new DataProductsGroup(groups.at(i));
            group->setParent(parentGroup);
            mGroups->append(group, 0);
        }
    }
}

void FormProductsGroups::setCodeURL()
{
    if (!ui->treeViewGroups->currentIndex().isValid())
        return;

    DataProductsGroup *group = new DataProductsGroup;
    QModelIndexList list = ui->treeViewGroups->selectionModel()->selectedRows();
    for (int i = 0; i < list.size(); i++) {
        DataProductsGroup *item = static_cast <DataProductsGroup *> (list.at(i).internalPointer());
        group->addId(item->getId());
    }
    if (ApiAdapter::getApi()->setCodeUrlProductGroup(group))
        refreshData();

    delete group;
}

void FormProductsGroups::goToRowById(const QString &id)
{
    if (!id.isEmpty()) {
        ui->treeViewGroups->selectionModel()->setCurrentIndex(
                    mModel->getModelIndexById(id), QItemSelectionModel::SelectCurrent |
                    QItemSelectionModel::Rows);
        ui->treeViewGroups->setFocus();
        return;
    }

    if (mGroups->size())
        ui->treeViewGroups->selectionModel()->setCurrentIndex(
                    mModel->index(0, 0), QItemSelectionModel::SelectCurrent |
                    QItemSelectionModel::Rows);
    else onChangeGroup();
    ui->treeViewGroups->setFocus();
}

void FormProductsGroups::setSimpleMode(const bool &isSimpleMode)
{
    if (isSimpleMode)
        for (int i = 1; i < ui->treeViewGroups->header()->count(); i++)
            ui->treeViewGroups->header()->showSection(i);
     else
        for (int i = 1; i < ui->treeViewGroups->header()->count(); i++)
            ui->treeViewGroups->header()->hideSection(i);
}

void FormProductsGroups::setExludingGroups(const ListProductsGroups *groups)
{
    for (int i = 0; i < groups->size(); i++)
        mExludingGroups->append(new DataProductsGroup(groups->at(i)));
}

const DataProductsGroup *FormProductsGroups::checkedGroup() const
{
    QStringList list = mModel->idsChecked();
    if (list.count()) {
        for (int i = 0; i < mGroups->size(); i++)
            if (mGroups->at(i)->getId() == list.at(0))
                return mGroups->at(i);
    }
    return 0;
}

void FormProductsGroups::setVisibleCheckeds(const bool &isVisible)
{
    if (mModel) {
        mModel->setCheckedMode(isVisible);
        ui->toolButtonCheckedAll->setVisible(isVisible);
        ui->toolButtonUncheckedAll->setVisible(isVisible);
        mModel->resetModel();
    }
}

QList<DataProductsGroup *> FormProductsGroups::listSelected()
{
    return mListSelected;
}

QTreeWidgetItem *FormProductsGroups::getTreeItemByIdInChilds(const int &id,
                                                             const QTreeWidgetItem &itemParent)
{
    for (int i = 0; i < itemParent.childCount(); i++) {
        QTreeWidgetItem *item = itemParent.child(i);
        if (item->data(0, Qt::UserRole).toInt() == id)
            return item;
        else {
            QTreeWidgetItem *itemChild = getTreeItemByIdInChilds(id, *item);
            if (itemChild)
                return itemChild;
        }
    }
    return 0;
}

void FormProductsGroups::onAddGroupProduct()
{
    QModelIndex currentIndex = ui->treeViewGroups->currentIndex();
    QModelIndex parentIndex;
    if (currentIndex.isValid())
        parentIndex = currentIndex.parent();

    FormProductGroupCard mCardGroup(this);
    mCardGroup.setWindowTitle(tr("Добавление категории"));
    if (parentIndex.isValid()) {
        DataProductsGroup *item = dynamic_cast <DataProductsGroup *> (mModel->dataItem(parentIndex));
        mCardGroup.setIdParentGroup(item->getId(), item->getName());
    }

    if (mCardGroup.exec() == QDialog::Accepted) {
        disconnect(mModel, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(onMoved(QModelIndex,int,int)));
        mModel->addItem(new DataProductsGroup(mCardGroup.group()));
        ui->treeViewGroups->selectionModel()->clearSelection();
        goToRowById(mCardGroup.group()->getId());                
        connect(mModel, SIGNAL(rowsInserted(QModelIndex,int,int)), SLOT(onMoved(QModelIndex,int,int)));
        ui->lineEditCount->setText(QString::number(mGroups->size()));
    }
}

void FormProductsGroups::onAddSubGroupProduct()
{
    QModelIndex currentIndex = ui->treeViewGroups->currentIndex();
    if (!currentIndex.isValid()) {
        QMessageBox::warning(this, tr("Добавление подкатегории"),
                             tr("Выберите надкатегорию"));
        return;
    }
    if (!ui->treeViewGroups->isExpanded(currentIndex))
        ui->treeViewGroups->expand(currentIndex);
    FormProductGroupCard mCardGroup;
    mCardGroup.setWindowTitle(tr("Добавление подкатегории"));
    DataProductsGroup *item =  dynamic_cast <DataProductsGroup *> (mModel->dataItem(currentIndex));

    mCardGroup.setIdParentGroup(item->getId(), item->getName());
    if (mCardGroup.exec() == QDialog::Accepted) {
        disconnect(mModel, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(onMoved(QModelIndex,int,int)));
        ui->treeViewGroups->selectionModel()->clearSelection();
        mModel->addItem(new DataProductsGroup(mCardGroup.group()));
        goToRowById(mCardGroup.group()->getId());        
        connect(mModel, SIGNAL(rowsInserted(QModelIndex,int,int)), SLOT(onMoved(QModelIndex,int,int)));
        ui->lineEditCount->setText(QString::number(mGroups->size()));
    }
}

void FormProductsGroups::onSortGroups()
{
    if (!ui->treeViewGroups->currentIndex().isValid())
        return;

    if (mIdTimer)
        killTimer(mIdTimer);

    bool isMoveUp = sender() == ui->actionUp;

    QString id = mModel->dataItem(ui->treeViewGroups->currentIndex())->getId();
    mModel->beginReset();
    if (isMoveUp)
        mGroups->moveUp(mModel->dataItem(ui->treeViewGroups->selectionModel()->currentIndex()));
    else mGroups->moveDown(mModel->dataItem(ui->treeViewGroups->selectionModel()->currentIndex()));
    mModel->endReset();
    goToRowById(id);
    mIdTimer = startTimer(1000);
}

void FormProductsGroups::onExpanded(const QModelIndex &index)
{
    if (!index.isValid())
        return;

    const QString idParent = mModel->dataItem(index)->getId();
    refreshData(idParent, index);
    disconnect(ui->treeViewGroups, SIGNAL(expanded(QModelIndex)));
    ui->treeViewGroups->expand(index);
    connect(ui->treeViewGroups, SIGNAL(expanded(QModelIndex)), SLOT(onExpanded(QModelIndex)));
}

void FormProductsGroups::onMoved(const QModelIndex &parent, int first, int last)
{
    DataItem *itemParent = mModel->dataItem(parent);
    DataProductsGroup item;
    if (itemParent) {
        item.setIdParent(itemParent->getId());
        item.setSortIndex(itemParent->children().size());
        for (int i = first; i <= last; ++i) {
            if (itemParent->children().size() > i) {
                item.addId(dynamic_cast <DataItem *> (itemParent->children().at(i))->getId());
            }
        }
    }
    else {
        item.setIdParent("0");
        item.setSortIndex(mGroups->root()->children().size());
        for (int i = first; i <= last; ++i) {
            if (mGroups->root()->children().size() > i) {
                item.addId(dynamic_cast <DataItem *> (mGroups->root()->children().at(i))->getId());
            }
        }
    }
    ApiAdapter::getApi()->saveProductGroup(&item);
}

void FormProductsGroups::onSetIncPrices()
{
    if (!ui->treeViewGroups->currentIndex().isValid())
        return;

    QModelIndexList list = ui->treeViewGroups->selectionModel()->selectedRows();
    if (!list.size())
        return;

    DataProductsGroup *group = new DataProductsGroup();
    for (int i = 0; i < list.count(); i++)
    {
        DataProductsGroup *groupI = static_cast <DataProductsGroup *> (list.at(i).internalPointer());
        group->addId(groupI->getId());
    }

    DialogIncPrices dialog(this, group);
    if (dialog.exec() == QDialog::Accepted) {
        refreshData();
    }
}

void FormProductsGroups::onChangeParentGroup()
{
    if (!UserPermission::isEdit(UserPermission::PRODUCTS))
        return;

    QModelIndexList list = ui->treeViewGroups->selectionModel()->selectedRows();
    if (!list.count())
        return;

    FormProductsGroups *form = new FormProductsGroups(this);

    form->setWindowFlags(Qt::Dialog);
    form->setSelectMode(false);
    if (form->exec() == QDialog::Accepted) {
        QListIterator<DataProductsGroup *> it(form->listSelected());
        if (it.hasNext()) {
            DataProductsGroup *group = new DataProductsGroup;
            DataProductsGroup *gr = (it.next());
            for (int i = 0; i < list.count(); i++) {
                QString id = static_cast <DataProductsGroup *> (list.at(i).internalPointer())->getId();
                if (gr->getId() != id)
                    group->addId(id);
            }
            group->setIdParent(gr->getId());
            if (ApiAdapter::getApi()->saveProductGroup(group))
                refreshData();
            delete group;
        }
    }
    delete form;
}

void FormProductsGroups::onClearParentGroup()
{
    if (!UserPermission::isEdit(UserPermission::PRODUCTS))
        return;

    QModelIndexList list = ui->treeViewGroups->selectionModel()->selectedRows();
    if (!list.count())
        return;

    DataProductsGroup *group = new DataProductsGroup;
    for (int i = 0; i < list.count(); i++) {
        QString id = static_cast <DataProductsGroup *> (list.at(i).internalPointer())->getId();
        group->addId(id);
    }
    group->setIdParent("0");
    group->setIdParent(QString());
    if (ApiAdapter::getApi()->saveProductGroup(group))
        refreshData();

    delete group;
}

void FormProductsGroups::importData()
{
    DialogGroupsMasterImport dialogMaster;
    if (dialogMaster.exec() == QDialog::Accepted)
        refreshData();
}

void FormProductsGroups::exportData()
{
    DialogExport dlgExport(this);
    dlgExport.setFormat("xlsx");
    dlgExport.setApiCatalogName(SEConsts::API_PRODUCTS_GROUPS);
    dlgExport.setFileName("groups");
    dlgExport.exec();

}

void FormProductsGroups::setVisibleColumns()
{
    if (!mFormSetColumns)
        mFormSetColumns = new FormSetVisibleHeaderColumns(this, ui->treeViewGroups->header());
    mFormSetColumns->exec();
}

void FormProductsGroups::storeCheckedItems()
{
    idsCheckedsStore.clear();
    for (int i = 0; i < mGroups->size(); ++i)
        if (mGroups->at(i)->getIsChecked())
            idsCheckedsStore.append(mGroups->at(i)->getId());
}

void FormProductsGroups::restoreCheckedItems()
{
    for (int i = 0; i < mGroups->size(); ++i)
        if (idsCheckedsStore.contains(mGroups->at(i)->getId()))
            mGroups->at(i)->setIsChecked(true);
}

void FormProductsGroups::selectAll()
{
    ui->treeViewGroups->expandAll();
    for (int i = 0; i < mGroups->size(); ++i) {
        ui->treeViewGroups->selectionModel()->select(
                    mModel->getModelIndexById(mGroups->at(i)->getId()), QItemSelectionModel::Select |
                    QItemSelectionModel::Rows);
    }
}

void FormProductsGroups::resetRequest(const QString &idParent)
{
    mRequest.clear();    
    if (!idParent.isNull() && !idParent.isEmpty())
        mRequest.setIdParent(idParent);    
    else mRequest.setIdParent("");
    mRequest.setSearchText(ui->widgetSearchGroups->searchText().trimmed());
}

void FormProductsGroups::onEditGroup()
{
    if (!UserPermission::isEdit(UserPermission::PRODUCTS))
        return;

    QModelIndex parentIndex;
    QModelIndex currentIndex = ui->treeViewGroups->currentIndex();    
    if (!currentIndex.isValid()) {
        QMessageBox::warning(this, "Редактирование",
                             "Выберите категорию для редактирования");
        return;
    }

    DataItem *item =  mModel->dataItem(currentIndex);
    if (!item)
        return;

    setCursor(Qt::WaitCursor);
    storeCheckedItems();
    FormProductGroupCard cardGroup(this);
    bool isMultiMode = false;

    QModelIndexList list = ui->treeViewGroups->selectionModel()->selectedRows();
    if (!list.size()) {
        ui->treeViewGroups->selectionModel()->setCurrentIndex(
                    currentIndex, QItemSelectionModel::SelectCurrent | QItemSelectionModel::Rows);
        list = ui->treeViewGroups->selectionModel()->selectedRows();
    }

    if (list.size() == 1) {
        cardGroup.setId(item->getId());
        cardGroup.setWindowTitle(QString("Редактирование: [" + item->getName() + "]"));
        parentIndex = currentIndex.parent();
        if (parentIndex.isValid()) {
            DataItem *itemParent =  mModel->dataItem(parentIndex);
            cardGroup.setIdParentGroup(itemParent->getId(), itemParent->getName());
        }
    }
    else {
        isMultiMode = true;
        QStringList ids;
        for (int i = 0; i < list.size(); i++)
            ids.append(mModel->dataItem(list.at(i))->getId());
        cardGroup.setIds(ids);
        cardGroup.setWindowTitle(tr("Мультиредактирование"));
    }

    if (cardGroup.exec() == QDialog::Accepted) {
        if (!isMultiMode) {
            QString oldIdParent = item->getIdParent();
            item->copy(cardGroup.group());
            if (item->getIdParent() != oldIdParent) {
                mModel->moveItem(currentIndex, item);
                goToRowById(item->getId());
            };
            restoreCheckedItems();
        } else refreshData();
    }
    setCursor(Qt::ArrowCursor);
}

void FormProductsGroups::onDeleteGroups()
{
    if (!UserPermission::isDelete(UserPermission::PRODUCTS))
        return;

    QString msg;
    QModelIndexList list = ui->treeViewGroups->selectionModel()->selectedRows();
    if (!list.size()) {
        QMessageBox::warning(this, tr("Удаление категорий товаров"),
                             tr("Выделите категории для удаления!"));
        return;
    }

    DataProductsGroup *group;
    if (list.size() > 1)
        msg = tr("Удалить выбранные категории вместе с подкатегориями и товарами?");
    else {
        group = static_cast <DataProductsGroup *> (list.at(0).internalPointer());
        msg = tr("Удалить группу ") + " " + group->getName() +
                " вместе с подгруппами и товарами?";
    }

    QMessageBox::StandardButton mesRepl;

    mesRepl = QMessageBox::question(this, tr("Подтверждение удаления"),
                                    msg, QMessageBox::Yes | QMessageBox::No);
    if(mesRepl == QMessageBox::Yes) {
        Request request;    
        for (int i = 0; i < list.count(); i++)
        {
            group = static_cast <DataProductsGroup *> (list.at(i).internalPointer());
            if (!group->getId().isEmpty())
                request.addId(group->getId());
        }        
        if (ApiAdapter::getApi()->deleteProductsGroups(request)) {
            mModel->deleteItems(list);
            emit onItemChecked();
       }
    }
}

void FormProductsGroups::on_treeViewGroups_doubleClicked(const QModelIndex &)
{
    if (ui->toolButtonEditGroup->isVisible())
        onEditGroup();
    else onSelectGroups();
}

void FormProductsGroups::setContextMenu()
{
    ui->treeViewGroups->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(ui->treeViewGroups, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(showContextMenu(QPoint)));
}

void FormProductsGroups::initActions()
{
    ui->toolButtonAddGroup->setDefaultAction(ui->actionAddGroup);    
    ui->toolButtonDeleteGroup->setDefaultAction(ui->actionDeleteGroups);
    ui->toolButtonEditGroup->setDefaultAction(ui->actionEditGroup);
    ui->toolButtonSelect->setDefaultAction(ui->actionSelect);
    ui->toolButtonMoveDown->setDefaultAction(ui->actionDown);
    ui->toolButtonMoveUp->setDefaultAction(ui->actionUp);
    onChangeGroup();
}

void FormProductsGroups::initSignals()
{
    connect(ui->actionAddGroup, SIGNAL(triggered()), SLOT(onAddGroupProduct()));
    connect(ui->actionAddSubGroup, SIGNAL(triggered()), SLOT(onAddSubGroupProduct()));
    connect(ui->actionEditGroup, SIGNAL(triggered()), SLOT(onEditGroup()));
    connect(ui->actionDeleteGroups, SIGNAL(triggered()), SLOT(onDeleteGroups()));
    connect(ui->actionSetParentRoot, SIGNAL(triggered()), SLOT(onClearParentGroup()));
    connect(ui->actionSetParentGroup, SIGNAL(triggered()), SLOT(onChangeParentGroup()));
    connect(ui->toolButtonRefresh, SIGNAL(clicked()), SLOT(refreshData()));
    connect(ui->actionSelect, SIGNAL(triggered()), SLOT(onSelectGroups()));
    connect(ui->actionDown, SIGNAL(triggered()), SLOT(onSortGroups()));
    connect(ui->actionUp, SIGNAL(triggered()), SLOT(onSortGroups()));
    connect(ui->toolButtonImport, SIGNAL(clicked(bool)), SLOT(importData()));
    connect(ui->toolButtonExport, SIGNAL(clicked(bool)), SLOT(exportData()));
    connect(ui->toolButtonSetColumns, &QToolButton::clicked, this, &FormProductsGroups::setVisibleColumns);
    connect(ui->actionSetCodeUrl, &QAction::triggered, this, &FormProductsGroups::setCodeURL);   
    connect(ui->widgetSearchGroups, SIGNAL(runSearch()), SLOT(refreshData()));
    connect(ui->toolButtonExpandAll, &QAbstractButton::clicked, this, &FormProductsGroups::expandCollapseTree);
    connect(ui->toolButtonCollapseAll, &QAbstractButton::clicked, this, &FormProductsGroups::expandCollapseTree);
    connect(ui->toolButtonCheckedAll, &QAbstractButton::clicked, this, &FormProductsGroups::checkedUncheckedTree);
    connect(ui->toolButtonUncheckedAll, &QAbstractButton::clicked, this, &FormProductsGroups::checkedUncheckedTree);
    connect(mApi, SIGNAL(finishedRequest(bool,QString)), SLOT(finishedRefreshData(bool,QString)));
    connect(ui->actionSetIncPrices, SIGNAL(triggered(bool)), SLOT(onSetIncPrices()));
}

void FormProductsGroups::initFields()
{
    mFields.clear();    
    mFields.append(new Field("name", "Наименование"));
    mFields.append(new Field("id", "Ид.", Qt::AlignRight | Qt::AlignVCenter));
    mFields.append(new Field("code", "Код"));
    if (SEConfig::getMainInfo()->isShowIncPrices())
        mFields.append(new Field("sourcePriceTitle", "Источник цен"));
    mFields.append(new Field("sortIndex", "Индекс"));
    mFields.append(new Field("note", "SEO описание"));
}

void FormProductsGroups::onSelectGroups()
{
    QModelIndexList list =
        ui->treeViewGroups->selectionModel()->selectedRows();
    for (int i = 0; i < list.size(); i++) {
        DataProductsGroup *item =
                static_cast <DataProductsGroup *> (list.at(i).internalPointer());
        mListSelected.append(item);
    }
    accept();
}

void FormProductsGroups::onItemChecked()
{
    QModelIndexList expandedList;
    for (int i = 0; i < mModel->indexList().size(); i++) {
        if (ui->treeViewGroups->isExpanded(mModel->indexList().at(i)))
            expandedList.push_back(mModel->indexList().at(i));
    }
    emit onItemTreeChanged();
    ui->treeViewGroups->reset();
    for (int i = 0; i < expandedList.size(); i++)
        ui->treeViewGroups->expand(expandedList.at(i));
}

void FormProductsGroups::onChangeGroup()
{
    QModelIndex currentIndex = ui->treeViewGroups->currentIndex();
    ui->actionAddGroup->setEnabled(UserPermission::isAdd(UserPermission::PRODUCTS));
    ui->actionAddSubGroup->setEnabled(currentIndex.isValid() &&
                                      !mModel->dataItem(currentIndex)->getId().isEmpty() &&
                                      UserPermission::isAdd(UserPermission::PRODUCTS));
    ui->actionEditGroup->setEnabled(currentIndex.isValid() &&
                                    !mModel->dataItem(currentIndex)->getId().isEmpty() &&
                                    UserPermission::isEdit(UserPermission::PRODUCTS));
    ui->actionDeleteGroups->setEnabled(currentIndex.isValid() &&
                                       !mModel->dataItem(currentIndex)->getId().isEmpty() &&
                                       UserPermission::isDelete(UserPermission::PRODUCTS));
    ui->actionSetParentRoot->setEnabled(currentIndex.isValid() &&
                                        !mModel->dataItem(currentIndex)->getId().isEmpty() &&
                                        UserPermission::isEdit(UserPermission::PRODUCTS));
    ui->actionSetParentGroup->setEnabled(currentIndex.isValid() &&
                                         !mModel->dataItem(currentIndex)->getId().isEmpty() &&
                                         UserPermission::isEdit(UserPermission::PRODUCTS));
    ui->actionSelect->setEnabled(currentIndex.isValid() && !mModel->dataItem(currentIndex)->getId().isEmpty());
    ui->actionUp->setEnabled(currentIndex.isValid() &&
                             mModel->dataItem(currentIndex)->row() &&
                             UserPermission::isEdit(UserPermission::PRODUCTS));
    if (currentIndex.isValid() &&
            mModel->dataItem(currentIndex)->getIdParent().isEmpty() &&
           (mModel->dataItem(currentIndex)->row() == 1))
        ui->actionUp->setEnabled(false);
    ui->actionDown->setEnabled(currentIndex.isValid() &&
                               !mModel->dataItem(currentIndex)->getId().isEmpty() &&
                               mModel->dataItem(currentIndex)->parent() &&
                               mModel->dataItem(currentIndex)->parent()->children().count() - 1 !=
            mModel->dataItem(currentIndex)->row() &&
            UserPermission::isEdit(UserPermission::PRODUCTS));
}


