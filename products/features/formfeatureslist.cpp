#include "formfeatureslist.h"
#include "ui_formfeatureslist.h"
#include "formfeaturevaluecard.h"
#include "apiadapter.h"
#include "seconfig.h"
#include "userpermission.h"

#include <QKeyEvent>

FormFeaturesList::FormFeaturesList(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FormFeaturesList)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Widget);

    initialization();
}

FormFeaturesList::FormFeaturesList(QWidget *parent, DataFeature *feature) :
    QDialog(parent),
    ui(new Ui::FormFeaturesList)
{
    ui->setupUi(this);
    initialization();
    setFeature(feature);
    setSelectMode();
}

void FormFeaturesList::initialization()
{
    mIsSelectMode = 0;
    ui->toolButtonSelect->setHidden(true);
    mValues = new ListFeatures();
    mModel = new NameTableModel(this, reinterpret_cast<ListDataItems *>(mValues));    
    mModel->prependField(new Field("id", "Ид.", Qt::AlignRight | Qt::AlignVCenter));
    mIdTimer = 0;
    mTypeTimer = 0;
    mIsNotDeleteSelectedList = false;
    isModifiedSort = false;
    mFeature = 0;
    mSelectedValues = new ListFeatures();
    ui->widget->setIsHotSearchMode();
    connect(ui->widget, SIGNAL(runSearch()), SLOT(onSearch()));

    initTable();
    initSignals();
    initPermissions();
}

FormFeaturesList::~FormFeaturesList(){  

    delete ui;    
    if (!mIsNotDeleteSelectedList)
        delete mSelectedValues;
}

void FormFeaturesList::setFeature(DataFeature *feature)
{    
    mFeature = feature;    
    if (feature)
        mModel->setTitleHeader(1, feature->getName());
    else mModel->setTitleHeader(1, QString());
}

void FormFeaturesList::setSelectMode()
{
    mIsSelectMode = 1;
    ui->toolButtonSelect->show();
    ui->toolButtonSelect->setDisabled(true);
    connect(ui->toolButtonSelect, SIGNAL(clicked()), SLOT(onSelect()));
    disconnect(ui->treeView, SIGNAL(doubleClicked(QModelIndex)), 0, 0);
    connect(ui->treeView, SIGNAL(doubleClicked(QModelIndex)),
            SLOT(onSelect()));
}

void FormFeaturesList::setSelectModificationMode()
{
    mIsSelectMode = 2;
    ui->toolButtonDelete->hide();
    disconnect(ui->treeView, SIGNAL(doubleClicked(QModelIndex)), 0, 0);
    ui->treeView->setStyleSheet("QTreeView::item:selected:!active {background: #EEE8AA;}");
}

void FormFeaturesList::setSelectedValues(const QStringList &listId)
{
    mListIdSelected.clear();
    for (int i = 0; i < listId.size(); i++)
        mListIdSelected.push_back(listId.at(i));
}

const ListFeatures *FormFeaturesList::selectedValues()
{
    if (mSelectedValues->size())
        return mSelectedValues;

    QModelIndexList list = ui->treeView->selectionModel()->selectedRows();    
    for (int i = 0; i < list.size(); i++)
        mSelectedValues->append(mValues->at(list.at(i).row()));
    return mSelectedValues;
}

DataFeature *FormFeaturesList::selectedFeatureValue() const
{
    if (ui->treeView->currentIndex().isValid()) {
        DataFeature *value = dynamic_cast <DataFeature *>
                (mModel->dataItem(ui->treeView->currentIndex()));
        if (value)
            return value;
    }
    return 0;
}

void FormFeaturesList::onAddEdit()
{
    int isNew = false;

    isNew = sender() == ui->toolButtonAdd;

    if ((isNew && !isAddPermission) || (!isNew && !isEditPermission))
        return;

    FormFeatureValueCard *card = new FormFeatureValueCard(this, mFeature->isColorType());
    card->setIdFeature(mFeature->getId());
    QString id = currentId();
    if (!id.isEmpty() && !isNew)
        card->setId(id);
    if (card->exec() == QDialog::Accepted) {
        refreshData();
        goToRowById(card->idValue());
    }
    delete card;
}

void FormFeaturesList::onDelete()
{
    QString msg;
    if (ui->treeView->selectionModel()->selectedRows().count() > 1)
        msg = tr("Удалить выбранные значения?");
    else msg = tr("Удалить значение") + ": " +
            mModel->nameData(ui->treeView->currentIndex()) + "?";

    QMessageBox::StandardButton mesRepl;

    mesRepl = QMessageBox::question(this, tr("Подтверждение удаления"),
                                    msg, QMessageBox::Yes | QMessageBox::No);
    if(mesRepl == QMessageBox::Yes)    {
        Request request;
        QModelIndexList list = ui->treeView->selectionModel()->selectedRows(0);

        for (int i = 0; i < list.count(); i++)
            request.addId(mModel->idData(list.at(i)));
        ApiAdapter::getApi()->deleteFeaturesValues(request);
        refreshData();
    }
}

void FormFeaturesList::onChange()
{
    QString id = currentId();

    ui->toolButtonEdit->setEnabled(!id.isEmpty() && isEditPermission);
    ui->toolButtonDelete->setEnabled(!id.isEmpty() && isDeletePermission);

    ui->toolButtonMoveUp->setEnabled(!id.isEmpty() &&
                                     ui->treeView->
                                     selectionModel()->currentIndex().row() && isEditPermission);
    ui->toolButtonMoveDown->setEnabled(!id.isEmpty() &&
                                       mModel->rowCount() - 1 -
                                       ui->treeView->
                                       selectionModel()->currentIndex().row() && isEditPermission);
    ui->toolButtonSelect->setEnabled(!id.isEmpty());
}

void FormFeaturesList::onUpDown()
{
    if (mIdTimer)
        killTimer(mIdTimer);

    bool isMoveUp = sender() == ui->toolButtonMoveUp;

    QString id = currentId();
    mModel->beginReset();
    if (isMoveUp)
        mValues->moveUp(mModel->dataItem((ui->treeView->selectionModel()->currentIndex())));
    else mValues->moveDown(mModel->dataItem((ui->treeView->selectionModel()->currentIndex())));
    mModel->endReset();
    goToRowById(id);
    mTypeTimer = 1;
    isModifiedSort = true;
    mIdTimer = startTimer(1000);
}

void FormFeaturesList::onSelect()
{
    accept();
}

void FormFeaturesList::onSearch()
{
    for (int i = 0; i < mValues->size(); ++i) {
         QString s = ui->widget->searchText().toLower();
         ui->treeView->setRowHidden(i, QModelIndex(), !mValues->at(i)->getName().toLower().contains(s));
    }
}

void FormFeaturesList::showEvent(QShowEvent *)
{
    startTimer(1);
}

void FormFeaturesList::hideEvent(QHideEvent *)
{
    saveSort();
}

void FormFeaturesList::timerEvent(QTimerEvent *event)
{
    killTimer(event->timerId());
    switch (mTypeTimer) {
    case 0:
        refreshData();
        break;
    case 1:
        saveSort();
        break;
    default:
        break;
    }
}

bool FormFeaturesList::eventFilter(QObject *, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        switch (keyEvent->key()) {
         case Qt::Key_Up:
             if (keyEvent->modifiers() & Qt::ControlModifier &&
                     ui->treeView->hasFocus()) {
                 emit ui->toolButtonMoveUp->clicked();
                 return true;
             }
             break;
         case Qt::Key_Down:
             if (keyEvent->modifiers() & Qt::ControlModifier &&
                     ui->treeView->hasFocus()) {
                 emit ui->toolButtonMoveDown->clicked();
                 return true;
             }
             break;
        case Qt::Key_Insert:
            if (ui->treeView->hasFocus()) {
                emit ui->toolButtonAdd->click();
                return true;
            }
            break;
        case Qt::Key_Delete:
            if (keyEvent->modifiers() & Qt::ControlModifier &&
                    ui->toolButtonDelete->isEnabled() &&
                    ui->treeView->hasFocus()) {
                emit ui->toolButtonDelete->click();
                return true;
            }
            break;
        }
    }

    return false;
}

QString FormFeaturesList::currentId()
{
    return mModel->idData(ui->treeView->currentIndex());
}

void FormFeaturesList::saveSort()
{
    if (isModifiedSort) {
        ApiAdapter::getApi()->sortFeaturesValues(mModel->getSortParamsForApi());
        isModifiedSort = false;
    }
}

void FormFeaturesList::refreshData()
{
    if (mFeature && mFeature->isListType()) {
        mRequest.setIdFeature(mFeature->getId());

        mModel->beginReset();
        ApiAdapter::getApi()->getListFeaturesValues(mRequest, mValues);
        mModel->endReset();
        setEnabled(true);

        ui->treeView->resizeColumnToContents(0);
        if (mIsSelectMode == 2)
            selectSelectedItems();
    } else {
        mModel->beginReset();
        mValues->deleteItems();
        mModel->endReset();
        setDisabled(true);
    }
}

void FormFeaturesList::setShadowMode()
{
     ui->treeView->setStyleSheet("background-color: " + SEConsts::SHADOW_COLOR);
     ui->toolButtonDelete->hide();
}

void FormFeaturesList::selectAll()
{
    ui->treeView->selectAll();
}

void FormFeaturesList::setIsNotDeleteSelectedList()
{
    mIsNotDeleteSelectedList = true;
}

void FormFeaturesList::initTable()
{
    ui->treeView->setModel(mModel);
    ui->treeView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->treeView->setSelectionMode(QAbstractItemView::ExtendedSelection);
}

void FormFeaturesList::initSignals()
{
    connect(ui->toolButtonAdd, SIGNAL(clicked()), SLOT(onAddEdit()));
    connect(ui->toolButtonEdit, SIGNAL(clicked()), SLOT(onAddEdit()));
    connect(ui->toolButtonDelete, SIGNAL(clicked()), SLOT(onDelete()));
    connect(ui->toolButtonMoveDown, SIGNAL(clicked()), SLOT(onUpDown()));
    connect(ui->toolButtonMoveUp, SIGNAL(clicked()), SLOT(onUpDown()));
    connect(ui->treeView->selectionModel(),
            SIGNAL(currentChanged(QModelIndex, QModelIndex)),
            SLOT(onChange()));
    switch (mIsSelectMode) {
    case 0:
        connect(ui->treeView, SIGNAL(doubleClicked(QModelIndex)),
                ui->toolButtonEdit, SLOT(click()));
        break;
    case 1:
        connect(ui->treeView, SIGNAL(doubleClicked(QModelIndex)),
                ui->toolButtonSelect, SLOT(click()));
        break;
    default:
        break;
    }

    ui->treeView->installEventFilter(this);
}

void FormFeaturesList::initPermissions()
{
    isAddPermission = UserPermission::isAdd(UserPermission::PRODUCTS);
    isEditPermission = UserPermission::isEdit(UserPermission::PRODUCTS);
    isDeletePermission = UserPermission::isDelete(UserPermission::PRODUCTS);

    ui->toolButtonAdd->setEnabled(isAddPermission);
    ui->toolButtonEdit->setEnabled(isEditPermission);
    ui->toolButtonDelete->setEnabled(isDeletePermission);
}

void FormFeaturesList::goToRowById(const QString &id)
{
    QModelIndex modelIndex = mModel->getModelIndexById(id);

    if (modelIndex.isValid()) {
        ui->treeView->selectionModel()->setCurrentIndex(
                    modelIndex, QItemSelectionModel::SelectCurrent | QItemSelectionModel::Rows);
        return;
    }
}

void FormFeaturesList::selectSelectedItems()
{
    for (int j = 0; j < mValues->size(); j++) {
        for (int i = 0; i < mListIdSelected.size(); i++) {
            if (mValues->at(j)->getId() == mListIdSelected.at(i))
            {
                ui->treeView->selectionModel()->select(
                            mModel->index(j, 0), QItemSelectionModel::Select);
                break;
            }
        }
    }
}
