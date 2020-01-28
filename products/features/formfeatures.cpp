#include "formfeatures.h"
#include "ui_formfeatures.h"
#include "seconfig.h"
#include "formfeaturecard.h"
#include "formfeatureslist.h"
#include "apiadapter.h"
#include "field.h"
#include "userpermission.h"

#include <QMessageBox>
#include <QMenu>

FormFeatures::FormFeatures(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FormFeatures)
{
    mIsSelectMode = false;
    initialization();
}

FormFeatures::FormFeatures(QWidget *parent,
                           const bool &isDialog,
                           const bool &isModificationsMode) :
    QDialog(parent),
    ui(new Ui::FormFeatures){   

    mIsModificationsMode = isModificationsMode;
    mIsSelectMode = isDialog;
    initialization();    
    if (mIsSelectMode)
        setSelectMode();
}

void FormFeatures::initialization()
{
    ui->setupUi(this);
    ui->toolButtonSelect->setHidden(true);

    mRequest.setLimit(ui->widgetPaginator->getLimit());
    mRequest.setOffset(mNumPage * ui->widgetPaginator->getLimit());

    mFeatures = new ListFeatures();
    mFeaturesModel = new NameTableModel(this, reinterpret_cast <ListDataItems *> (mFeatures));    
    mTypeTimer = 0;
    mIdTimer = 0;
    mDeletedList = 0;
    mListTypesFeatures = 0;
    mNumPage = 0;
    mListSelected = new ListFeatures;    
    mIsModificationsMode = false;
    mIsShowed = false;    
    mIsNotDeleteSelectedList = false;
    ui->widgetSearch->setIsHotSearchMode();

    initTreeFeatures();
    initActions();
    initSignals();
    initNavigator();
    initPermissions();
}

void FormFeatures::initNavigator()
{
    connect(ui->widgetPaginator, SIGNAL(goNextPage()), this, SLOT(goNextPage()));
    connect(ui->widgetPaginator, SIGNAL(goPreviosPage()), this, SLOT(goPreviosPage()));
    connect(ui->widgetPaginator, SIGNAL(goLastPage()), this, SLOT(goLastPage()));
    connect(ui->widgetPaginator, SIGNAL(goFirstPage()), this, SLOT(goFirstPage()));
    connect(ui->widgetPaginator, SIGNAL(refreshDataStart()), this, SLOT(goFirstPage()));
}

FormFeatures::~FormFeatures()
{
    saveState();
    if (!mIsNotDeleteSelectedList)
        delete mListSelected;
    delete ui;
}

void FormFeatures::setSelectMode(const ListFeatures *deletedList)
{
    mDeletedList = deletedList;
    mIsSelectMode = true;
    ui->toolButtonSelect->show();
    ui->toolButtonSelect->setEnabled(false);
    disconnect(ui->treeViewFeatures, SIGNAL(doubleClicked(QModelIndex)), 0, 0);
    connect(ui->treeViewFeatures, SIGNAL(doubleClicked(QModelIndex)), ui->toolButtonSelect, SLOT(click()));
    connect(ui->toolButtonSelect, SIGNAL(clicked()), SLOT(accept()));
    ui->toolButtonDel->hide();
    ui->toolButtonSEO->hide();
    ui->widgetFrameGroups->hide();
    ui->widgetFeatureList->hide();
    ui->groupBoxFeaturesList->hide();
}

void FormFeatures::setIsNotDeleteSelectedList()
{
    mIsNotDeleteSelectedList = true;
}

ListFeatures *FormFeatures::selectedList() const
{
    mListSelected->clear();
    QModelIndexList list =
            ui->treeViewFeatures->selectionModel()->selectedRows();
    for (int i = 0; i < list.size(); i++) {
        DataFeature *item = dynamic_cast <DataFeature *> (mFeaturesModel->dataItem(list.at(i)));
        if (item)
            mListSelected->append(item);
    }
    return mListSelected;
}

void FormFeatures::showEvent(QShowEvent *)
{
    loadState();
    startTimer(1);
    ui->widgetSearch->setFocus();
}

void FormFeatures::hideEvent(QShowEvent *)
{
    saveState();
}

void FormFeatures::timerEvent(QTimerEvent *event)
{
    killTimer(event->timerId());

    switch (mTypeTimer) {
    case 0:
        refreshData();        
        break;
    case 1:
        saveSortGroups();
        break;
    case 2:
        saveSEOCheckeds();
        break;
    case 3:
       searchData() ;
       break;
    default:
        break;
    }
}

void FormFeatures::refreshData()
{
    if (!UserPermission::isRead(UserPermission::PRODUCTS))
        return;

    refreshFeatures();
    mIsShowed = true;
    if (mDeletedList) {
        mFeaturesModel->beginReset();
        for (int i = mFeatures->size() - 1; i >= 0; i--) {
            bool isExist = false;
            for (int j = 0; j < mDeletedList->size(); j++)
                if (mDeletedList->at(j)->idFeature() == mFeatures->at(i)->getId()) {
                    isExist = true;
                    break;
                }
            if (isExist)
                delete mFeatures->takeAt(i);
        }
        mFeaturesModel->endReset();
    }
}

void FormFeatures::refreshGroups()
{
    ui->widgetFrameGroups->refreshData();
}

void FormFeatures::refreshFeatures()
{    
    ui->widgetPaginator->setNumRecord(0, 0);
    resetRequest();
    QString id = currentIdFeature();

    mFeaturesModel->beginReset();
    setCursor(Qt::WaitCursor);
    if (ApiAdapter::getApi()->getListFeatures(mRequest, mFeatures))
        mCountFeatures = mFeatures->countInBase();
    setCursor(Qt::ArrowCursor);
    if (mDeletedList) {
        for (int i = mFeatures->size() - 1; i >= 0; i--) {
            bool isExist = false;
            for (int j = 0; j < mDeletedList->size(); j++)
                if (mDeletedList->at(j)->idFeature() == mFeatures->at(i)->getId()) {
                    isExist = true;
                    break;
                }
            if (isExist)
                delete mFeatures->takeAt(i);
        }
    }
    mFeaturesModel->endReset();
    for (int i = 0; i < mFeaturesModel->columnCount() - 1; ++i)
        ui->treeViewFeatures->resizeColumnToContents(i);

    goToRowByIdFeature(id);
}

void FormFeatures::goNextPage()
{
    mNumPage++;
    if (mNumPage > int(mCountFeatures / ui->widgetPaginator->getLimit()))
        mNumPage = int(mCountFeatures / ui->widgetPaginator->getLimit());
    refreshFeatures();
}

void FormFeatures::goPreviosPage()
{
    mNumPage--;
    if (mNumPage < 0)
        mNumPage = 0;
    refreshFeatures();
}

void FormFeatures::goLastPage()
{
    mNumPage = int(mCountFeatures / ui->widgetPaginator->getLimit());
    refreshFeatures();
}

void FormFeatures::goFirstPage()
{
    mNumPage = 0;
    refreshFeatures();
}

void FormFeatures::onAddEditFeature()
{
    int isNew = false;

    isNew = sender() == ui->actionAdd;
    if ((isNew && !isAddPermission) || (!isNew && !isEditPermission))
        return;

    FormFeatureCard *card = new FormFeatureCard(this);
    QString id = currentIdFeature();
    if (!id.isEmpty() && !isNew)
        card->setId(id);
    card->show();

    connect(card, SIGNAL(accepted()), SLOT(refreshFeatures()));
}

void FormFeatures::onDeleteFeature()
{
    QString msg;
    if (ui->treeViewFeatures->selectionModel()->selectedRows().count() > 1)
        msg = tr("Удалить выбранные параметры?");
    else msg = tr("Удалить параметр") + ": " +
            mFeaturesModel->nameData(ui->treeViewFeatures->currentIndex()) + "?";

    QMessageBox::StandardButton mesRepl;

    mesRepl = QMessageBox::question(this, tr("Подтверждение удаления"),
                                    msg, QMessageBox::Yes | QMessageBox::No);
    if(mesRepl == QMessageBox::Yes)    {
        Request request;
        QModelIndexList list = ui->treeViewFeatures->selectionModel()->selectedRows(0);

        for (int i = 0; i < list.count(); i++)
            request.addId(mFeaturesModel->idData(list.at(i)));
        setCursor(Qt::WaitCursor);
        ApiAdapter::getApi()->deleteFeatures(request);
        setCursor(Qt::ArrowCursor);
        refreshFeatures();
    }
}

QString FormFeatures::currentIdFeature()
{
    QModelIndex index = ui->treeViewFeatures->currentIndex();
    if (index.isValid())
        return mFeaturesModel->idData(index);
    else return QString();
}

DataFeature *FormFeatures::currentFeature()
{
    QModelIndex index = ui->treeViewFeatures->currentIndex();
    if (index.isValid())
        return dynamic_cast <DataFeature *> (mFeaturesModel->dataItem(index));
    else return 0;
}

void FormFeatures::saveSortGroups()
{
    ApiAdapter::getApi()->sortFeatures(mFeaturesModel->getSortParamsForApi());
}

void FormFeatures::saveSEOCheckeds()
{
    RequestFeature request;
    for (int i = 0; i < mFeatures->size(); i++) {
        DataFeature *data = reinterpret_cast<DataFeature *> (mFeatures->at(i));
        request.setIsSEO(data->getId(), data->getIsChecked());
    }

    ApiAdapter::getApi()->setSEOFeatures(request);
}

void FormFeatures::initSignals()
{
    connect(ui->toolButtonRefresh, SIGNAL(clicked()), SLOT(goFirstPage()));
    connect(ui->toolButtonSEO, SIGNAL(clicked(bool)), SLOT(onSetSEOMode(bool)));    
    connect(ui->toolButtonFeatureDown, SIGNAL(clicked()), SLOT(onSortFeatures()));
    connect(ui->toolButtonFeatureUp, SIGNAL(clicked()), SLOT(onSortFeatures()));
    connect(ui->toolButtonSelect, SIGNAL(clicked()), SLOT(onSelectFeatures()));
    connect(ui->actionAdd, SIGNAL(triggered(bool)), SLOT(onAddEditFeature()));
    connect(ui->actionEdit, SIGNAL(triggered(bool)), SLOT(onAddEditFeature()));
    connect(ui->actionDel, SIGNAL(triggered(bool)), SLOT(onDeleteFeature()));
    connect(ui->actionSetGroup, SIGNAL(triggered(bool)), SLOT(onSetGroup()));
    connect(ui->actionUnbindGroup, SIGNAL(triggered(bool)), SLOT(onUnbindGroup()));
    connect(ui->widgetSearch, SIGNAL(runSearch()), SLOT(onRunSearch()));
}

void FormFeatures::initTreeFeatures()
{    
    mFeaturesModel->prependField(new Field("icoType", QString(), 0, Qt::DecorationRole));
    mFeaturesModel->prependField(new Field("id", "Ид.", Qt::AlignRight | Qt::AlignVCenter));    
    mFeaturesModel->appendField(new Field("isYAMarket", "Я.М.", Qt::AlignCenter | Qt::AlignVCenter, Qt::DecorationRole));
    mFeaturesModel->appendField(new Field("measure", tr("Ед.изм.")));
    mFeaturesModel->appendField(new Field("type", tr("Тип параметра")));
    mFeaturesModel->appendField(new Field("nameGroup", tr("Группа параметра")));
    mFeaturesModel->appendField(new Field("code", "Код (URL)"));
    mFeaturesModel->appendField(new Field("namesTypesGoods", tr("Типы товаров")));
    ui->treeViewFeatures->setModel(mFeaturesModel);
    ui->treeViewFeatures->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->treeViewFeatures->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->treeViewFeatures->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(ui->treeViewFeatures->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), SLOT(onChangeFeatures()));
    connect(ui->treeViewFeatures, SIGNAL(doubleClicked(QModelIndex)),
            SLOT(onAddEditFeature()));
    connect(mFeaturesModel, SIGNAL(checked(QString,bool)), SLOT(onCheckedSEO()));
    connect(ui->widgetFrameGroups, SIGNAL(checkedGroups()), SLOT(refreshFeatures()));    
    connect(ui->treeViewFeatures, &QWidget::customContextMenuRequested,
            this, &FormFeatures::showContextMenu);

    ui->treeViewFeatures->installEventFilter(this);
}

void FormFeatures::initActions()
{
    ui->toolButtonAdd->setDefaultAction(ui->actionAdd);
    ui->toolButtonEdit->setDefaultAction(ui->actionEdit);
    ui->toolButtonDel->setDefaultAction(ui->actionDel);
}

void FormFeatures::initPermissions()
{
    isAddPermission = UserPermission::isAdd(UserPermission::PRODUCTS);
    isEditPermission = UserPermission::isEdit(UserPermission::PRODUCTS);
    isDeletePermission = UserPermission::isDelete(UserPermission::PRODUCTS);

    ui->actionAdd->setEnabled(isAddPermission);
    ui->actionEdit->setEnabled(isEditPermission);
    ui->actionDel->setEnabled(isDeletePermission);
    ui->actionSetGroup->setEnabled(isEditPermission);
    ui->actionUnbindGroup->setEnabled(isEditPermission);
    ui->toolButtonFeatureDown->setEnabled(isEditPermission);
    ui->toolButtonFeatureUp->setEnabled(isEditPermission);
    ui->toolButtonSEO->setEnabled(isEditPermission);
}

void FormFeatures::saveState()
{
    int tag = 0;
    if (mIsSelectMode)
        tag = 1;
    if (mIsModificationsMode)
        tag = 2;
    else SEConfig::saveSplitterLayout(ui->splitterFeatures, tag);
    SEConfig::saveTableState(ui->treeViewFeatures->header(),
                                    ui->treeViewFeatures->objectName(),
                                    tag);
}

void FormFeatures::loadState()
{
    int tag = 0;
    if (mIsSelectMode)
        tag = 1;
    if (mIsModificationsMode) {
        ui->widgetFrameGroups->hide();
        tag = 2;
    } else SEConfig::loadSplitterLayout(ui->splitterFeatures, tag);
    SEConfig::restoreTableState(ui->treeViewFeatures->header(),
                                    ui->treeViewFeatures->objectName(),
                                tag);
}

void FormFeatures::searchData()
{
    resetRequest();
    if (ApiAdapter::getApi()->getListFeatures(mRequest, mFeatures)) {
        mCountFeatures = mFeatures->countInBase();
        mFeaturesModel->resetModel();
    }
}

void FormFeatures::goToRowByIdFeature(QString id)
{
    QModelIndex modelIndex = mFeaturesModel->getModelIndexById(id);

    if (modelIndex.isValid()) {
        ui->treeViewFeatures->selectionModel()->setCurrentIndex(
                    modelIndex, QItemSelectionModel::SelectCurrent | QItemSelectionModel::Rows);
        return;
    }

    if (mFeaturesModel->rowCount() > 0)
        ui->treeViewFeatures->selectionModel()->
                setCurrentIndex(mFeaturesModel->index(0, 0),
                                QItemSelectionModel::SelectCurrent |
                                QItemSelectionModel::Rows);
    else onChangeFeatures();
}

void FormFeatures::onChangeFeatures()
{
    QString id = currentIdFeature();

    ui->actionEdit->setEnabled(!id.isEmpty() && isEditPermission);
    ui->actionDel->setEnabled(!id.isEmpty() && isDeletePermission);
    ui->actionSetGroup->setEnabled(!id.isEmpty() && isEditPermission);
    ui->toolButtonFeatureUp->setEnabled(!id.isEmpty() &&
                                        ui->treeViewFeatures->
                                        selectionModel()->currentIndex().row() && isEditPermission);
    ui->toolButtonFeatureDown->setEnabled(!id.isEmpty() &&
                                          mFeaturesModel->rowCount() - 1 -
                                          ui->treeViewFeatures->
                                          selectionModel()->currentIndex().row() && isEditPermission);
    ui->toolButtonSelect->setEnabled(!id.isEmpty());

    if (mFeaturesModel->rowCount() > 0 &&
            ui->treeViewFeatures->currentIndex().isValid())
        ui->widgetPaginator->setNumRecord(mNumPage * ui->widgetPaginator->getLimit() +
                                         ui->treeViewFeatures->currentIndex().row() + 1,
                                         mCountFeatures);
    else ui->widgetPaginator->setNumRecord(0, 0);

    if (!mIsSelectMode)
        refreshListFeature();
}

void FormFeatures::onSortFeatures()
{
    if (mIdTimer)
        killTimer(mIdTimer);

    QToolButton *button = qobject_cast<QToolButton *> (sender());
    bool isMoveUp = button == ui->toolButtonFeatureUp;

    QString id = currentIdFeature();
    mFeaturesModel->beginReset();
    if (isMoveUp)
        mFeatures->moveUp(mFeaturesModel->dataItem(ui->treeViewFeatures->selectionModel()->currentIndex()));
    else mFeatures->moveDown(mFeaturesModel->dataItem(ui->treeViewFeatures->selectionModel()->currentIndex()));
    mFeaturesModel->endReset();
    goToRowByIdFeature(id);
    mTypeTimer = 1;
    mIdTimer = startTimer(1000);
}

void FormFeatures::onCheckedSEO()
{
    if (mIdTimer)
        killTimer(mIdTimer);

    mTypeTimer = 2;
    mIdTimer = startTimer(1000);
}

void FormFeatures::refreshListFeature()
{
    if (mFeatures->size() && ui->treeViewFeatures->currentIndex().isValid()) {
        DataFeature *feature = currentFeature();
        if (!feature)
            return;

        ui->widgetFeatureList->setFeature(feature);
        ui->widgetFeatureList->refreshData();
    } else {
        ui->widgetFeatureList->setFeature(0);
        ui->widgetFeatureList->refreshData();
    }
}

void FormFeatures::onSetSEOMode(const bool &isActive)
{
    mFeaturesModel->beginReset();
    mFeaturesModel->setCheckedMode(isActive);
    mFeaturesModel->endReset();
}

void FormFeatures::onSelectFeatures()
{
    if (selectedList()->size())
        accept();
}

void FormFeatures::onSetGroup()
{
    if (!ui->treeViewFeatures->currentIndex().isValid())
        return;

    FormFeaturesGroups *formGroups = new FormFeaturesGroups;
    formGroups->setSelectMode();
    if (formGroups->exec() == QDialog::Accepted) {
        QModelIndexList list = ui->treeViewFeatures->selectionModel()->selectedRows();
        DataFeature feature;
        feature.setIdGroup(formGroups->currentIdGroup());
        for (int i = 0; i < list.size(); i++)
            feature.addId(mFeaturesModel->dataItem(list.at(i))->getId());
        if (ApiAdapter::getApi()->saveFeature(&feature))
            refreshFeatures();
    }
    delete formGroups;
}

void FormFeatures::onUnbindGroup()
{
    QString msg;
    if (ui->treeViewFeatures->selectionModel()->selectedRows().count() > 1)
        msg = tr("Отвязать группу для выбранных параметров?");
    else msg = tr("Отвязать группу для параметра: ") + ": " +
            mFeaturesModel->nameData(ui->treeViewFeatures->currentIndex()) + "?";

    QMessageBox::StandardButton mesRepl;

    mesRepl = QMessageBox::question(this, tr("Подтверждение"),
                                    msg, QMessageBox::Yes | QMessageBox::No);
    if(mesRepl == QMessageBox::Yes)    {
        QModelIndexList list = ui->treeViewFeatures->selectionModel()->selectedRows();
        DataFeature feature;
        feature.setIdGroup("0");
        feature.setIdGroup(QString());
        for (int i = 0; i < list.size(); i++)
            feature.addId(mFeaturesModel->dataItem(list.at(i))->getId());
        if (ApiAdapter::getApi()->saveFeature(&feature))
            refreshFeatures();
    }
}

void FormFeatures::onRunSearch()
{
    if (mIdTimer)
        killTimer(mIdTimer);

    mTypeTimer = 0;
    mIdTimer = startTimer(250);
}

void FormFeatures::showContextMenu(QPoint point)
{
    QPoint globalPos;

    if (sender()->inherits("QAbstractScrollArea"))
        globalPos = ((QAbstractScrollArea*)sender())->viewport()->mapToGlobal(point);
    else globalPos = ((QWidget*)sender())->mapToGlobal(point);

    QMenu menu;

    menu.addAction(ui->actionAdd);
    menu.addAction(ui->actionEdit);
    menu.addAction(ui->actionDel);
    menu.addSeparator();
    menu.addAction(ui->actionSetGroup);
    menu.addAction(ui->actionUnbindGroup);
    menu.exec(globalPos);
}

bool FormFeatures::eventFilter(QObject *, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        switch (keyEvent->key()) {
         case Qt::Key_Up:
             if (keyEvent->modifiers() & Qt::ControlModifier &&
                     ui->treeViewFeatures->hasFocus()) {
                 emit ui->toolButtonFeatureUp->clicked();
                 return true;
             }
             break;
         case Qt::Key_Down:
             if (keyEvent->modifiers() & Qt::ControlModifier &&
                     ui->treeViewFeatures->hasFocus()) {
                 emit ui->toolButtonFeatureDown->clicked();
                 return true;
             }
             break;
        case Qt::Key_Insert:
            if (ui->treeViewFeatures->hasFocus()) {
                emit ui->toolButtonAdd->click();
                return true;
            }
            break;
        case Qt::Key_Delete:
            if (keyEvent->modifiers() & Qt::ControlModifier &&
                    ui->toolButtonDel->isEnabled() &&
                    ui->treeViewFeatures->hasFocus()) {
                emit ui->toolButtonDel->click();
                return true;
            }
            break;
        }
    }

    return false;
}

void FormFeatures::resetRequest()
{
    mRequest.clear();

    mRequest.setLimit(ui->widgetPaginator->getLimit());
    mRequest.setOffset(mNumPage * ui->widgetPaginator->getLimit());

    mRequest.clearFilter();
    QString idsGroups = ui->widgetFrameGroups->idsChecked().join(",");
    if (!idsGroups.isEmpty())
        mRequest.setFilter("idGroup IN (" + idsGroups + ")");
    mRequest.setIsList(mIsModificationsMode);

    mRequest.setSearchText(ui->widgetSearch->searchText());

}


