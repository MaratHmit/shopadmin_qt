#include "framecustomfields.h"
#include "ui_framecustomfields.h"
#include "dialogdynfielditem.h"
#include "dialogdynfieldsgroup.h"

FrameCustomFields::FrameCustomFields(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FrameCustomFields)
{
    ui->setupUi(this);

    dataType = parentWidget()->objectName().remove("tab").toLower();

    initVariables();
    initActions();
    initSignals();
}

FrameCustomFields::~FrameCustomFields()
{
    delete ui;
}

void FrameCustomFields::onChangeDynFieldsItem()
{
    ui->actionAdd->setEnabled(true);
    ui->actionEdit->setEnabled(ui->treeViewDynFields->currentIndex().isValid());
    ui->actionDel->setEnabled(ui->treeViewDynFields->currentIndex().isValid());
    ui->actionUp->setEnabled(ui->treeViewDynFields->currentIndex().isValid() &&
                                 ui->treeViewDynFields->
                                 selectionModel()->currentIndex().row());
    ui->actionDown->setEnabled(ui->treeViewDynFields->currentIndex().isValid() &&
                                   mModelDynFields->rowCount() - 1 -
                                   ui->treeViewDynFields->
                                   selectionModel()->currentIndex().row());
}

void FrameCustomFields::showEvent(QShowEvent *)
{
    if (!mIsShowed) {
        SEConfig::loadSplitterLayout(ui->splitterDynFields);
        restoreStateTable();
        refreshDataDynFieldsGroups();
        refreshDataDynFields();
        mIsShowed = true;
    }
}

void FrameCustomFields::hideEvent(QHideEvent *)
{
    SEConfig::saveSplitterLayout(ui->splitterDynFields);
    saveStateTable();
}

void FrameCustomFields::timerEvent(QTimerEvent *event)
{
    killTimer(event->timerId());
    if (mIsChangeSortGroup)
        saveSortGroups();
    if (mIsChangeSort)
        saveSortItems();
}

void FrameCustomFields::goToRowById(const QString &id)
{
    QModelIndex modelIndex = mModelDynFields->getModelIndexById(id);

    if (modelIndex.isValid()) {
        ui->treeViewDynFields->selectionModel()->setCurrentIndex(
                    modelIndex, QItemSelectionModel::SelectCurrent | QItemSelectionModel::Rows);
        return;
    }
    if (mModelDynFields->rowCount() > 0)
        ui->treeViewDynFields->selectionModel()->
                setCurrentIndex(mModelDynFields->index(0, 0),
                                QItemSelectionModel::SelectCurrent | QItemSelectionModel::Rows);
    else onChangeDynFieldsItem();
}

void FrameCustomFields::goToRowByIdGroups(const QString &id)
{
    QModelIndex modelIndex = mModelDynFieldsGroups->getModelIndexById(id);

    if (modelIndex.isValid()) {
        ui->treeViewDynFieldsGroups->selectionModel()->setCurrentIndex(
                    modelIndex, QItemSelectionModel::SelectCurrent | QItemSelectionModel::Rows);
        return;
    }
    if (mModelDynFieldsGroups->rowCount() > 0)
        ui->treeViewDynFieldsGroups->selectionModel()->
                setCurrentIndex(mModelDynFieldsGroups->index(0, 0),
                                QItemSelectionModel::SelectCurrent | QItemSelectionModel::Rows);
    else onChangeDynFieldsGroupsItem();
}

void FrameCustomFields::resetRequest()
{
    mRequest.clear();    
    QString idsGroups = mModelDynFieldsGroups->idsChecked().join(",");
    if (!idsGroups.isEmpty())
        mRequest.setFilter("[idGroup] IN (" + idsGroups + ")");
    else mRequest.setFilter("[data] = '" + dataType + "'");
}

void FrameCustomFields::restoreStateTable()
{
    mIsRestoreStates = SEConfig::restoreTableState(ui->treeViewDynFields->header(),
                                                   ui->treeViewDynFields->objectName());
}

void FrameCustomFields::setStateTableDefault()
{
    for (int i = 0; i < mModelDynFields->columnCount() - 1; i++)
        ui->treeViewDynFields->resizeColumnToContents(i);
    mIsRestoreStates = true;
}

void FrameCustomFields::saveStateTable()
{
    SEConfig::saveTableState(ui->treeViewDynFields->header(),
                             ui->treeViewDynFields->objectName());
}

void FrameCustomFields::saveSortGroups()
{
    ApiAdapter::getApi()->sortDynFieldsGroup(mModelDynFieldsGroups->getSortParamsForApi());
    mIsChangeSortGroup = false;
}

void FrameCustomFields::saveSortItems()
{
    ApiAdapter::getApi()->sortDynField(mModelDynFields->getSortParamsForApi());
    mIsChangeSort = false;
}

void FrameCustomFields::initActions()
{
    ui->toolButtonAdd->setDefaultAction(ui->actionAdd);
    ui->toolButtonEdit->setDefaultAction(ui->actionEdit);
    ui->toolButtonDelete->setDefaultAction(ui->actionDel);
    ui->toolButtonRefresh->setDefaultAction(ui->actionRefresh);
    ui->toolButtonUp->setDefaultAction(ui->actionUp);
    ui->toolButtonDown->setDefaultAction(ui->actionDown);
}

void FrameCustomFields::initVariables()
{
    mIdTimer = 0;
    mIsShowed = false;
    mIsChangeSort = false;
    mIsChangeSortGroup = false;
    mListDynFieldGroups = new ListDataItems();
    mListDynFields = new ListDataItems();
    initTreeDynFieldsGroups();
    initTreeDynFields();
}

void FrameCustomFields::initSignals()
{
    connect(mModelDynFieldsGroups, SIGNAL(checked(QString,bool)), SLOT(refreshDataDynFields()));
    connect(ui->toolButtonRefreshGroups, SIGNAL(clicked(bool)), SLOT(refreshDataDynFieldsGroups()));
    connect(ui->actionRefresh, SIGNAL(triggered(bool)), SLOT(refreshDataDynFields()));
    connect(ui->actionAdd, SIGNAL(triggered(bool)), SLOT(onAddEditItem()));
    connect(ui->actionEdit, SIGNAL(triggered(bool)), SLOT(onAddEditItem()));
    connect(ui->actionDel, SIGNAL(triggered(bool)), SLOT(onDeleteItem()));
    connect(ui->actionDown, SIGNAL(triggered(bool)), SLOT(onSortItem()));
    connect(ui->actionUp, SIGNAL(triggered(bool)), SLOT(onSortItem()));
    connect(ui->toolButtonAddGroup, SIGNAL(clicked(bool)), SLOT(onAddEditGroup()));
    connect(ui->toolButtonEditGroup, SIGNAL(clicked(bool)), SLOT(onAddEditGroup()));
    connect(ui->toolButtonDeleteGroup, SIGNAL(clicked(bool)), SLOT(onDeleteGroup()));
    connect(ui->treeViewDynFieldsGroups->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
            SLOT(onChangeDynFieldsGroupsItem()));
    connect(ui->treeViewDynFields->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
            SLOT(onChangeDynFieldsItem()));
    connect(ui->treeViewDynFields, SIGNAL(doubleClicked(QModelIndex)), SLOT(onAddEditItem()));
    connect(ui->treeViewDynFieldsGroups, SIGNAL(doubleClicked(QModelIndex)), SLOT(onAddEditGroup()));
    connect(ui->toolButtonUpGroup, SIGNAL(clicked(bool)), SLOT(onSortGroup()));
    connect(ui->toolButtonDownGroup, SIGNAL(clicked(bool)), SLOT(onSortGroup()));
}

void FrameCustomFields::initTreeDynFieldsGroups()
{
    mModelDynFieldsGroups = new NameTableModel(this, mListDynFieldGroups);
    ui->treeViewDynFieldsGroups->setModel(mModelDynFieldsGroups);
    mModelDynFieldsGroups->setCheckedMode(true);
}

void FrameCustomFields::initTreeDynFields()
{
    mModelDynFields = new NameTableModel(this, reinterpret_cast<ListDataItems *>(mListDynFields));
    mModelDynFields->clearFields();
    mModelDynFields->appendField(new Field("id", "Ид.", Qt::AlignRight | Qt::AlignVCenter));
    mModelDynFields->appendField(new Field("isActiveIco", "Активен", Qt::AlignCenter | Qt::AlignVCenter, Qt::DecorationRole));
    mModelDynFields->appendField(new Field("code", tr("Код")));
    mModelDynFields->appendField(new Field("name", tr("Наименование")));
    mModelDynFields->appendField(new Field("displayType", tr("Тип")));
    mModelDynFields->appendField(new Field("placeholder", tr("Подсказка")));
    mModelDynFields->appendField(new Field("sortIndex", tr("Порядок"), Qt::AlignRight | Qt::AlignVCenter));
    mModelDynFields->appendField(new Field("values", tr("Значения")));
    mModelDynFields->appendField(new Field("nameGroup", tr("Группа")));
    mModelDynFields->appendField(new Field("description", tr("Описание")));

    ui->treeViewDynFields->setModel(mModelDynFields);
}

void FrameCustomFields::refreshDataDynFieldsGroups()
{
    setCursor(QCursor(Qt::WaitCursor));
    QString id = QString();
    QModelIndex index = ui->treeViewDynFieldsGroups->currentIndex();
    if (index.isValid())
        id = mModelDynFieldsGroups->idData(ui->treeViewDynFieldsGroups->currentIndex());
    Request request;
    request.setFilter("[data] = '" + dataType + "'");
    if (ApiAdapter::getApi()->getListDynFieldsGroups(request, mListDynFieldGroups)) {
        mModelDynFieldsGroups->resetModel();
        goToRowByIdGroups(id);
    }
    setCursor(QCursor(Qt::ArrowCursor));
}

void FrameCustomFields::refreshDataDynFields()
{
    setCursor(QCursor(Qt::WaitCursor));
    resetRequest();
    QString id = QString();
    QModelIndex index = ui->treeViewDynFields->currentIndex();
    if (index.isValid())
        id = mModelDynFields->idData(ui->treeViewDynFields->currentIndex());
    if (ApiAdapter::getApi()->getListDynFields(mRequest, mListDynFields)) {
        mModelDynFields->resetModel();
        if (!mIsRestoreStates)
            setStateTableDefault();
        goToRowById(id);
    }
    setCursor(QCursor(Qt::ArrowCursor));
}

void FrameCustomFields::onAddEditGroup()
{
    bool isNew = sender() == ui->toolButtonAddGroup;
    if (!isNew && !ui->treeViewDynFieldsGroups->currentIndex().isValid())
        return;

    ui->toolButtonAddGroup->setEnabled(false);
    ui->toolButtonEditGroup->setEnabled(false);
    ui->toolButtonDeleteGroup->setEnabled(false);

    DialogDynFieldsGroup dialog(this);    
    if (!isNew)
        dialog.setGroup(mModelDynFieldsGroups->dataItem(ui->treeViewDynFieldsGroups->currentIndex()));
    dialog.setDataType(dataType);
    if (dialog.exec() == QDialog::Accepted) {
        refreshDataDynFieldsGroups();
        if (isNew)
            goToRowByIdGroups(dialog.group()->getId());
    } else onChangeDynFieldsGroupsItem();
}

void FrameCustomFields::onDeleteGroup()
{
    if (!ui->treeViewDynFieldsGroups->currentIndex().isValid()) {
        QMessageBox::warning(this, tr("Удаление групп элементов"),
                             tr("Выделите группы для удаления!"));
        return;
    }

    QString msg;
    if (ui->treeViewDynFieldsGroups->selectionModel()->selectedRows().count() > 1)
        msg = tr("Удалить выбранные группы?");
    else msg = tr("Удалить групу") + ": " +
            mModelDynFieldsGroups->dataItem(ui->treeViewDynFieldsGroups->currentIndex())->getName() + "?";

    QMessageBox::StandardButton mesRepl;

    mesRepl = QMessageBox::question(this, tr("Подтверждение удаления"),
                                    msg, QMessageBox::Yes | QMessageBox::No);
    if(mesRepl == QMessageBox::Yes)
    {
        Request request;

        QModelIndexList list = ui->treeViewDynFieldsGroups->selectionModel()->selectedRows();

        for (int i = 0; i < list.count(); i++)
            request.addId(mModelDynFieldsGroups->idData(list.at(i)));

        ApiAdapter::getApi()->deleteDynFieldsGroup(request);
        refreshDataDynFieldsGroups();
        refreshDataDynFields();
    }
}

void FrameCustomFields::onSortGroup()
{
    if (!ui->treeViewDynFieldsGroups->currentIndex().isValid())
        return;
    if (mIdTimer)
        killTimer(mIdTimer);

    bool isMoveUp = sender() == ui->toolButtonUpGroup;

    QString id = mModelDynFieldsGroups->idData(ui->treeViewDynFieldsGroups->currentIndex());
    mModelDynFieldsGroups->beginReset();
    if (isMoveUp)
        mListDynFieldGroups->moveUp(mModelDynFieldsGroups->dataItem(ui->treeViewDynFieldsGroups->selectionModel()->currentIndex()));
    else mListDynFieldGroups->moveDown(mModelDynFieldsGroups->dataItem(ui->treeViewDynFieldsGroups->selectionModel()->currentIndex()));
    mModelDynFieldsGroups->endReset();
    goToRowByIdGroups(id);
    mIsChangeSortGroup = true;
    mIdTimer = startTimer(1000);
}

void FrameCustomFields::onAddEditItem()
{
    bool isNew = sender() == ui->actionAdd;
    if (!isNew && !ui->treeViewDynFields->currentIndex().isValid())
        return;

    ui->actionAdd->setEnabled(false);
    ui->actionEdit->setEnabled(false);
    ui->actionDel->setEnabled(false);

    DialogDynFieldItem dialog(this);
    if (!isNew)
        dialog.setDynFieldItem(mModelDynFields->dataItem(ui->treeViewDynFields->currentIndex()));
    dialog.setDataType(dataType);
    dialog.setGroups(mListDynFieldGroups);
    if (dialog.exec() == QDialog::Accepted) {
        refreshDataDynFields();
        if (isNew)
            goToRowById(dialog.dynFieldItem()->getId());
    } else onChangeDynFieldsItem();
}

void FrameCustomFields::onDeleteItem()
{
    if (!ui->treeViewDynFields->currentIndex().isValid()) {
        QMessageBox::warning(this, tr("Удаление элементов"),
                             tr("Выделите элемент для удаления!"));
        return;
    }

    QString msg;
    if (ui->treeViewDynFields->selectionModel()->selectedRows().count() > 1)
        msg = tr("Удалить выбранные элементы?");
    else msg = tr("Удалить элемент") + ": " +
            mModelDynFields->dataItem(ui->treeViewDynFields->currentIndex())->getName() + "?";

    QMessageBox::StandardButton mesRepl;

    mesRepl = QMessageBox::question(this, tr("Подтверждение удаления"),
                                    msg, QMessageBox::Yes | QMessageBox::No);
    if(mesRepl == QMessageBox::Yes)
    {
        Request request;

        QModelIndexList list = ui->treeViewDynFields->selectionModel()->selectedRows();

        for (int i = 0; i < list.count(); i++)
            request.addId(mModelDynFields->idData(list.at(i)));

        ApiAdapter::getApi()->deleteDynFields(request);
        refreshDataDynFields();
    }
}

void FrameCustomFields::onSortItem()
{
    if (!ui->treeViewDynFields->currentIndex().isValid())
        return;
    if (mIdTimer)
        killTimer(mIdTimer);

    bool isMoveUp = sender() == ui->actionUp;

    QString id = mModelDynFields->idData(ui->treeViewDynFields->currentIndex());
    mModelDynFields->beginReset();
    if (isMoveUp)
        mListDynFields->moveUp(mModelDynFields->dataItem(ui->treeViewDynFields->selectionModel()->currentIndex()));
    else mListDynFields->moveDown(mModelDynFields->dataItem(ui->treeViewDynFields->selectionModel()->currentIndex()));
    mModelDynFields->endReset();
    goToRowById(id);
    mIsChangeSort = true;
    mIdTimer = startTimer(1000);
}

void FrameCustomFields::onChangeDynFieldsGroupsItem()
{
    ui->toolButtonAddGroup->setEnabled(true);
    ui->toolButtonEditGroup->setEnabled(ui->treeViewDynFieldsGroups->currentIndex().isValid());
    ui->toolButtonDeleteGroup->setEnabled(ui->treeViewDynFieldsGroups->currentIndex().isValid());
    ui->toolButtonUpGroup->setEnabled(ui->treeViewDynFieldsGroups->currentIndex().isValid() &&
                                        ui->treeViewDynFieldsGroups->
                                        selectionModel()->currentIndex().row());
    ui->toolButtonDownGroup->setEnabled(ui->treeViewDynFieldsGroups->currentIndex().isValid() &&
                                          mModelDynFieldsGroups->rowCount() - 1 -
                                          ui->treeViewDynFieldsGroups->
                                          selectionModel()->currentIndex().row());
}
