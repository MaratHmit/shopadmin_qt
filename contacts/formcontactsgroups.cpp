#include "formcontactsgroups.h"
#include "ui_formcontactsgroups.h"
#include "formcontactgroupcard.h"
#include "apiadapter.h"
#include "request.h"
#include "userpermission.h"

#include <QClipboard>
#include <QMenu>

FormContactsGroups::FormContactsGroups(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormContactsGroups)
{
    ui->setupUi(this);
    ui->toolButtonSelect->setHidden(true);

    isShown = false;
    groups = new ListDataItems();
    initTreeGroups();
    initActions();
    initSignals();
    initPermission();
}

FormContactsGroups::~FormContactsGroups()
{
    delete ui;
    delete groups;
}

void FormContactsGroups::initTreeGroups()
{
    modelGroups = new NameTableModel(this, dynamic_cast <ListDataItems *> (groups));
    modelGroups->setCheckedMode(true);
    ui->treeView->setModel(modelGroups);
    ui->treeView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->treeView->setSelectionMode(QAbstractItemView::ExtendedSelection);    
    ui->treeView->setAnimated(true);

    ui->treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->treeView, SIGNAL(customContextMenuRequested(QPoint)),
            SLOT(showContextMenu(QPoint)));
}

void FormContactsGroups::initSignals()
{    
    connect(ui->actionAddGroup, SIGNAL(triggered()), SLOT(onAddEditGroup()));
    connect(ui->actionEditGroup, SIGNAL(triggered()), SLOT(onAddEditGroup()));
    connect(ui->actionDeleteGroups, SIGNAL(triggered()), SLOT(onDeleteGroups()));
    connect(ui->actionRefresh, SIGNAL(triggered()), SLOT(refreshData()));
    connect(ui->treeView, SIGNAL(doubleClicked(QModelIndex)), ui->actionEditGroup,
            SLOT(trigger()));
    connect(ui->actionCopy, &QAction::triggered, this, &FormContactsGroups::onCopyCellText);
    connect(modelGroups, SIGNAL(checked(QString,bool)), SLOT(onCheckedGroups()));
}

void FormContactsGroups::initPermission()
{
    ui->actionAddGroup->setEnabled(UserPermission::isAdd(UserPermission::CONTACTS));
    ui->actionEditGroup->setEnabled(UserPermission::isEdit(UserPermission::CONTACTS));
    ui->actionDeleteGroups->setEnabled(UserPermission::isDelete(UserPermission::CONTACTS));
}

void FormContactsGroups::showContextMenu(QPoint point)
{
    onChangeGroup();
    QPoint globalPos;

    if (sender()->inherits("QAbstractScrollArea"))
        globalPos = ((QAbstractScrollArea*)sender())->viewport()->mapToGlobal(point);
    else globalPos = ((QWidget*)sender())->mapToGlobal(point);

    QMenu menu;

    menu.addAction(ui->actionAddGroup);
    menu.addAction(ui->actionEditGroup);
    menu.addAction(ui->actionDeleteGroups);
    menu.addSeparator();
    menu.addAction(ui->actionRefresh);
    menu.addSeparator();
    menu.addAction(ui->actionCopy);

    menu.exec(globalPos);
}

void FormContactsGroups::onSave(const DataItem *group, const bool &isNew)
{
    modelGroups->beginReset();
    if (isNew)
        groups->append(new DataItem(group));
    else {
        DataItem *editGroup = modelGroups->dataItem(ui->treeView->currentIndex());
        if (editGroup)
            editGroup->copy(group);
    }
    modelGroups->endReset();
    goToRowById(group->getId());
}

bool FormContactsGroups::goToRowById(const QString &id)
{
    if (id.isEmpty())
        return false;

    int row = 0;
    for (int i = 0; i < groups->size(); i++)
        if (groups->at(i)->getId() == id) {
            row = i;
            break;
        }

    ui->treeView->setCurrentIndex(modelGroups->index(row, 0));
    return ui->treeView->currentIndex().isValid();
}

void FormContactsGroups::showEvent(QShowEvent *)
{
    refreshData();
}

void FormContactsGroups::onAddEditGroup()
{
    int isNew = false;
    QString id;

    isNew = sender() == ui->actionAddGroup;

    DataItem *group;
    FormContactGroupCard formGroup;
    connect(&formGroup, SIGNAL(saved(const DataItem*,bool)),
            SLOT(onSave(const DataItem*,bool)));
    if (isNew)
        formGroup.setWindowTitle(tr("Новая группа контактов"));
    else {
        formGroup.setWindowTitle(tr("Редактирование группы контактов"));
        group = modelGroups->dataItem(ui->treeView->currentIndex());
        formGroup.setGroup(group);
        id = group->getId();
    }

    formGroup.exec();
}

void FormContactsGroups::onDeleteGroups()
{
    if (!ui->treeView->currentIndex().isValid()) {
        QMessageBox::warning(this, tr("Удаление групп контактов"),
                             tr("Выделите группы для удаления!"));
        return;
    }

    QString msg;
    if (ui->treeView->selectionModel()->selectedRows().count() > 1)
        msg = tr("Удалить выбранные группы?");
    else msg = tr("Удалить групу") + ": " +
            modelGroups->dataItem(ui->treeView->currentIndex())->getName() + "?";

    QMessageBox::StandardButton mesRepl;

    mesRepl = QMessageBox::question(this, tr("Подтверждение удаления"),
                                    msg, QMessageBox::Yes | QMessageBox::No);
    if(mesRepl == QMessageBox::Yes)
    {
        Request request;

        QModelIndexList list = ui->treeView->selectionModel()->selectedRows();

        for (int i = 0; i < list.count(); i++)
            request.addId(modelGroups->idData(list.at(i)));

        ApiAdapter::getApi()->deleteContactGroups(request);
        refreshData();
        emit checkedGroups();
    }
}

void FormContactsGroups::onChangeGroup()
{
    ui->actionEditGroup->setEnabled(
                ui->treeView->currentIndex().isValid() &&
                !modelGroups->idData(ui->treeView->currentIndex()).isEmpty() &&
                UserPermission::isEdit(UserPermission::CONTACTS));
    ui->actionDeleteGroups->setEnabled(
                ui->treeView->currentIndex().isValid() &&
                !modelGroups->idData(ui->treeView->currentIndex()).isEmpty() &&
                UserPermission::isDelete(UserPermission::CONTACTS));
}

void FormContactsGroups::onCheckedGroups()
{
    emit checkedGroups();
}

void FormContactsGroups::onCopyCellText()
{
    if (!ui->treeView->currentIndex().isValid())
        return;

    QApplication::clipboard()->setText(modelGroups->data(ui->treeView->currentIndex(), Qt::DisplayRole).toString());
}

void FormContactsGroups::refreshData(QString id)
{
    setCursor(Qt::WaitCursor);

    QModelIndex index = ui->treeView->currentIndex();
    if (index.isValid() && id.isEmpty())
        id = modelGroups->idData(index);

    disconnect(ui->treeView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)));
    modelGroups->beginReset();
    ApiAdapter::getApi()->getListContactsGroups(groups);
    modelGroups->endReset();
    connect(ui->treeView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
            SLOT(onChangeGroup()));
    checkedGroups(
                );
    setCursor(Qt::ArrowCursor);
}

void FormContactsGroups::initActions()
{
   ui->actionEditGroup->setEnabled(false);
   ui->actionDeleteGroups->setEnabled(false);
   ui->toolButtonAdd->setDefaultAction(ui->actionAddGroup);
   ui->toolButtonEdit->setDefaultAction(ui->actionEditGroup);
   ui->toolButtonDelete->setDefaultAction(ui->actionDeleteGroups);
   ui->toolButtonRefresh->setDefaultAction(ui->actionRefresh);
}

NameTableModel *FormContactsGroups::groupsModel() const
{
    return modelGroups;
}

QStringList FormContactsGroups::idCheckedGroups() const
{
    return modelGroups->idsChecked();
}
