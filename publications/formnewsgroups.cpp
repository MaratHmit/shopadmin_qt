#include "formnewsgroups.h"
#include "ui_formnewsgroups.h"

#include <QtWidgets>
#include <QMenu>
#include <QPoint>

#include "seconfig.h"
#include "apiadapter.h"
#include "request.h"
#include "formnewsgroupcard.h"
#include "userpermission.h"

FormNewsGroups::FormNewsGroups(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FormNewsGroups)
{
    mSelectMode = false;
    mIsShowed = false;
    ui->setupUi(this);
    ui->toolButtonSelect->setHidden(true);
    mGroups = new ListDataItems();
    ui->treeViewGroupsNews->setSelectionMode(QAbstractItemView::ExtendedSelection);
    setWindowFlags(Qt::Widget);
    mPreloader = new Preloader(this);
    initSignals();
    initActions();
    setContextMenu();
    initPermissions();
}

FormNewsGroups::~FormNewsGroups()
{    
    delete mGroups;
    delete ui;
}

QStringList FormNewsGroups::getIdsGroupsChecked()
{    
    return mModel->idsChecked();
}

void FormNewsGroups::setSelectMode(const bool &isMultiSelect)
{
    mSelectMode = true;
    setWindowFlags(Qt::Window);
    if (isMultiSelect)
        ui->treeViewGroupsNews->setSelectionMode(QAbstractItemView::ExtendedSelection);
    else  ui->treeViewGroupsNews->setSelectionMode(QAbstractItemView::SingleSelection);

    ui->toolButtonAddGroup->setHidden(true);
    ui->toolButtonAddSubGroup->setHidden(true);
    ui->toolButtonEditGroup->setHidden(true);
    ui->toolButtonDeleteGroup->setHidden(true);    
    ui->line->setHidden(true);
    ui->line_2->setHidden(true);
    ui->toolButtonMoveDown->setHidden(true);
    ui->toolButtonMoveUp->setHidden(true);
    ui->toolButtonSelect->setVisible(true);
}

void FormNewsGroups::refreshData()
{
    mPreloader->startAnimation();
    disconnect(ui->treeViewGroupsNews->selectionModel(),
               SIGNAL(currentChanged(QModelIndex,QModelIndex)));    
    mModel->beginReset();
    ApiAdapter::getApi()->getListNewsGroups(mModel->getItems());
    mModel->endReset();
    connect(ui->treeViewGroupsNews->selectionModel(),
            SIGNAL(currentChanged(QModelIndex,QModelIndex)),
            SLOT(onChangeGroup()));
    onChangeGroup();
    mPreloader->stopAnimation();
}

void FormNewsGroups::showEvent(QShowEvent *)
{
    if (!mIsShowed) {
        mModel = new NameTableModel(this, mGroups);
        if (!mSelectMode)
            mModel->setCheckedMode(true);
        ui->treeViewGroupsNews->setModel(mModel);
        SEConfig::restoreTableState(ui->treeViewGroupsNews->header(),
                                        ui->treeViewGroupsNews->objectName());
        connect(mModel, SIGNAL(checked(QString,bool)), SLOT(onItemChecked()));
        refreshData();
        mIsShowed = true;
    }
}

void FormNewsGroups::hideEvent(QHideEvent *)
{    
    SEConfig::saveTableState(ui->treeViewGroupsNews->header(),
                                    ui->treeViewGroupsNews->objectName());
}

void FormNewsGroups::insertRecord(const DataItem *group)
{
    recordAddEdit(group, true);
}

void FormNewsGroups::editRecord(const DataItem *group)
{
    recordAddEdit(group, false);
}

void FormNewsGroups::showContextMenu(QPoint point)
{
    onChangeGroup();

    QPoint globalPos;

    if (sender()->inherits("QAbstractScrollArea"))
        globalPos = ((QAbstractScrollArea*)sender())->viewport()->mapToGlobal(point);
    else globalPos = ((QWidget*)sender())->mapToGlobal(point);

    QMenu menu;

    if (mSelectMode)
        menu.addAction(ui->actionSelect);
    else {
        menu.addAction(ui->actionAddGroup);
        menu.addAction(ui->actionAddSubGroup);
        menu.addAction(ui->actionEditGroup);
        menu.addAction(ui->actionDeleteGroups);
    }
    menu.exec(globalPos);
}

void FormNewsGroups::initWidgetGroups()
{
    ui->treeViewGroupsNews->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->treeViewGroupsNews->setSortingEnabled(true);
}

void FormNewsGroups::initPermissions()
{
    isAddPermission = UserPermission::isAdd(UserPermission::NEWS);
    isEditPermission = UserPermission::isEdit(UserPermission::NEWS);
    isDeletePermission = UserPermission::isDelete(UserPermission::NEWS);

    ui->actionAddGroup->setEnabled(isAddPermission);
}

void FormNewsGroups::recordAddEdit(const DataItem *group, const bool &isNew)
{
    QModelIndex currentIndex = ui->treeViewGroupsNews->currentIndex();
    ui->treeViewGroupsNews->setFocus();
    QModelIndexList expandedList;
    for (int i = 0; i < mModel->indexList().size(); i++) {
        if (ui->treeViewGroupsNews->isExpanded(mModel->indexList().at(i)))
            expandedList.push_back(mModel->indexList().at(i));
    }
    if (isNew)
        mGroups->append(new DataItem(group));
    else mModel->dataItem(currentIndex)->copy(group);
    ui->treeViewGroupsNews->reset();
    for (int i = 0; i < expandedList.size(); i++)
        ui->treeViewGroupsNews->expand(expandedList.at(i));
    ui->treeViewGroupsNews->setCurrentIndex(currentIndex);
}

void FormNewsGroups::setSimpleMode(const bool &isSimpleMode)
{
    if (isSimpleMode)
        for (int i = 1; i < ui->treeViewGroupsNews->header()->count(); i++)
            ui->treeViewGroupsNews->header()->showSection(i);
     else
        for (int i = 1; i < ui->treeViewGroupsNews->header()->count(); i++)
            ui->treeViewGroupsNews->header()->hideSection(i);
}

QList<DataItem *> FormNewsGroups::listSelected()
{
    return mListSelected;
}

QTreeWidgetItem *FormNewsGroups::getTreeItemByIdInChilds(const int &id,
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

void FormNewsGroups::onAddGroup()
{
    QModelIndex currentIndex = ui->treeViewGroupsNews->currentIndex();
    QModelIndex parentIndex;
    if (currentIndex.isValid())
        parentIndex = currentIndex.parent();

    FormNewsGroupCard *cardGroup = new FormNewsGroupCard(this);

    if (parentIndex.isValid()) {
        DataItem *item = mModel->dataItem(parentIndex);
        cardGroup->setIdParentGroup(item->getId(), item->getName());
    }

    if (cardGroup->exec() == QDialog::Accepted)
        insertRecord(cardGroup->group());
    delete cardGroup;
}

void FormNewsGroups::onAddSubGroup()
{
    QModelIndex currentIndex = ui->treeViewGroupsNews->currentIndex();
    if (!currentIndex.isValid()) {
        QMessageBox::warning(this, tr("Добавление подгруппы"),
                             tr("Выберите родительский элемент"));
        return;
    }
    FormNewsGroupCard *cardGroup = new FormNewsGroupCard(this);
    DataItem *item = mModel->dataItem(currentIndex);

    cardGroup->setIdParentGroup(item->getId(), item->getName());
    if (cardGroup->exec() == QDialog::Accepted)
        insertRecord(cardGroup->group());
    delete cardGroup;
}

void FormNewsGroups::onEditGroup()
{
    QModelIndex parentIndex;
    QModelIndex currentIndex = ui->treeViewGroupsNews->currentIndex();
    FormNewsGroupCard *cardGroup = new FormNewsGroupCard(this);
    if (!currentIndex.isValid()) {
        QMessageBox::warning(this, tr("Редактирование группы"),
                             tr("Выберите группу для редактирования"));
        return;
    }

    DataItem *item = mModel->dataItem(currentIndex);
    cardGroup->setGroup(item);
    parentIndex = currentIndex.parent();
    if (parentIndex.isValid()) {
        item = mModel->dataItem(parentIndex);
        cardGroup->setIdParentGroup(item->getId(), item->getName());
    }
    if (cardGroup->exec() == QDialog::Accepted)
        editRecord(cardGroup->group());
    delete cardGroup;
}

void FormNewsGroups::onDeleteGroups()
{
    QString msg;
    QModelIndexList list = ui->treeViewGroupsNews->selectionModel()->selectedRows();

    DataItem *group;
    if (list.size() > 1)
        msg = tr("Удалить выбранные группы вместе с подгруппами и новостями?");
    else {
        group = static_cast <DataItem *> (list.at(0).internalPointer());
        msg = tr("Удалить группу ") + " " + group->getName() +
                " вместе с подгруппами и новостями?";
    }

    QMessageBox::StandardButton mesRepl;

    mesRepl = QMessageBox::question(this, tr("Подтверждение удаления"),
                                    msg, QMessageBox::Yes | QMessageBox::No);
    if(mesRepl == QMessageBox::Yes) {
        Request request;
        for (int i = 0; i < list.count(); i++)
        {
            group = static_cast <DataItem *>
                    (list.at(i).internalPointer());
            request.addId(group->getId());
        }        
        if (ApiAdapter::getApi()->deleteNewsGroups(request))
            refreshData();
    }
}

void FormNewsGroups::on_treeViewGroupsNews_doubleClicked(const QModelIndex &)
{   
    if (ui->toolButtonEditGroup->isVisible())
        onEditGroup();
    else onSelectGroups();
}

void FormNewsGroups::setContextMenu()
{
    ui->treeViewGroupsNews->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(ui->treeViewGroupsNews, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(showContextMenu(QPoint)));
}

void FormNewsGroups::initActions()
{
    ui->toolButtonAddGroup->setDefaultAction(ui->actionAddGroup);
    ui->toolButtonAddSubGroup->setDefaultAction(ui->actionAddSubGroup);
    ui->toolButtonDeleteGroup->setDefaultAction(ui->actionDeleteGroups);
    ui->toolButtonEditGroup->setDefaultAction(ui->actionEditGroup);
    ui->toolButtonSelect->setDefaultAction(ui->actionSelect);
    onChangeGroup();
}

void FormNewsGroups::initSignals()
{
    connect(ui->actionAddGroup, SIGNAL(triggered()),
           SLOT(onAddGroup()));
    connect(ui->actionAddSubGroup, SIGNAL(triggered()),
            SLOT(onAddSubGroup()));
    connect(ui->actionEditGroup, SIGNAL(triggered()),
            SLOT(onEditGroup()));
    connect(ui->actionDeleteGroups, SIGNAL(triggered()),
            SLOT(onDeleteGroups()));
    connect(ui->toolButtonRefresh, SIGNAL(clicked()),
            SLOT(refreshData()));
    connect(ui->actionSelect, SIGNAL(triggered()),
            SLOT(onSelectGroups()));
}

void FormNewsGroups::onSelectGroups()
{
    QModelIndexList list =
        ui->treeViewGroupsNews->selectionModel()->selectedRows();
    for (int i = 0; i < list.size(); i++) {
        DataItem *item =
                static_cast <DataItem *> (list.at(i).internalPointer());
        mListSelected.append(item);
    }
    accept();
}

void FormNewsGroups::onItemChecked()
{
    QModelIndexList expandedList;
    for (int i = 0; i < mModel->indexList().size(); i++) {
        if (ui->treeViewGroupsNews->isExpanded(mModel->indexList().at(i)))
            expandedList.push_back(mModel->indexList().at(i));
    }
    emit onItemTreeChanged();
    ui->treeViewGroupsNews->reset();
    for (int i = 0; i < expandedList.size(); i++)
        ui->treeViewGroupsNews->expand(expandedList.at(i));
}

void FormNewsGroups::onChangeGroup()
{
    QModelIndex currentIndex = ui->treeViewGroupsNews->currentIndex();
    ui->actionAddSubGroup->setEnabled(currentIndex.isValid() && isAddPermission);
    ui->actionEditGroup->setEnabled(currentIndex.isValid() && isEditPermission);
    ui->actionDeleteGroups->setEnabled(currentIndex.isValid() && isDeletePermission);
    ui->actionSelect->setEnabled(currentIndex.isValid());
}


