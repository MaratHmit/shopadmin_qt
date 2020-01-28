#include "formmodificationsgroups.h"
#include "ui_formmodificationsgroups.h"
#include "seconfig.h"
#include "apiadapter.h"
#include "request.h"
#include "csvreader.h"
#include "userpermission.h"

#include <QDebug>
#include <QFileDialog>
#include <QFileInfo>
#include <QFile>
#include <QSettings>

FormModificationsGroups::FormModificationsGroups(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FormModificationsGroups)
{
    initialization();
    ui->toolButtonSelect->hide();
    ui->labelHelp->hide();
}

FormModificationsGroups::FormModificationsGroups(QWidget *parent, const bool &isSelect) :
    QDialog(parent),
    ui(new Ui::FormModificationsGroups)
{
    initialization();
    setWindowFlags(Qt::Dialog);
    mIsSelectMode = isSelect;
    if (mIsSelectMode) {
        ui->actionRemoveGroup->setVisible(false);
        ui->toolButtonDelGroup->hide();
        ui->widgetValues->setShadowMode();
    }
}

void FormModificationsGroups::initialization()
{
    ui->setupUi(this);
    mIsSelectMode = false;
    mIsLoadedLayouts = false;
    mModel = 0;
    mListPricesTypes = 0;
    mIdTimer = 0;
    mTypeTimer = 0;
    mTypePriceGroupDef = "1";
    mNameColumnDef = "Размеры";
    mGroups = new ListModificationsGroups();
    mSelectedGroups = new ListModificationsGroups();
    mExludingGroups = new ListModificationsGroups();
    initSignals();
    initActions();
    initPermissions();
}

FormModificationsGroups::~FormModificationsGroups()
{
    if (mListPricesTypes)
        delete mListPricesTypes;;
    delete ui;
    delete mSelectedGroups;
    delete mExludingGroups;
}

void FormModificationsGroups::setExludingGroups(const ListModificationsGroups *groups)
{
    for (int i = 0; i < groups->size(); i++)
        mExludingGroups->append(new DataModificationGroup(groups->at(i)));
}

const ListModificationsGroups *FormModificationsGroups::selectedGroups()
{
    if (mSelectedGroups->size())
        return mSelectedGroups;

    QModelIndexList list = ui->treeViewGroupsM->selectionModel()->selectedRows();

    for (int i = 0; i < list.size(); i++)
        mSelectedGroups->append(dynamic_cast <DataModificationGroup *> (mModel->dataItem(list.at(i))));

    return mSelectedGroups;
}

void FormModificationsGroups::refreshData()
{
    ui->actionEditGroup->setEnabled(false);
    ui->actionRemoveGroup->setEnabled(false);

    if (!mModel) {
        mModel = new NameTableModel(this, reinterpret_cast<ListDataItems *>(mGroups));        
        initTreeViewGroups();
    }

    setCursor(Qt::WaitCursor);
    refreshValues();
    QString id;

    disconnect(ui->treeViewGroupsM->selectionModel(),
               SIGNAL(selectionChanged(QItemSelection,QItemSelection)));

    QModelIndex index = ui->treeViewGroupsM->currentIndex();
    if (index.isValid())
        id = mModel->idData(ui->treeViewGroupsM->currentIndex());

    mModel->beginReset();
    ApiAdapter::getApi()->getListModificationsGroups(mGroups);
    if (mGroups->size()) {
        mTypePriceGroupDef = mGroups->at(mGroups->size() - 1)->type();
        if (mGroups->at(mGroups->size() - 1)->columns()->size())
            mNameColumnDef = mGroups->at(mGroups->size() - 1)->columns()->at(0)->getName();
    }
    for (int i = 0; i < mExludingGroups->size(); i++)
        for (int j = 0; j < mGroups->size(); j++)
            if (mGroups->at(j)->getId() == mExludingGroups->at(i)->getId()) {
                mGroups->deleteItem(mGroups->at(j));
                break;
            }
    mModel->endReset();
    for (int i = 0; i < mModel->columnCount(); i++)
        ui->treeViewGroupsM->resizeColumnToContents(i);

    connect(ui->treeViewGroupsM->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
            SLOT(onChangeGroups()));
    goToRowById(id);
    setCursor(Qt::ArrowCursor);
}

void FormModificationsGroups::showEvent(QShowEvent *)
{    
    SEConfig::loadSplitterLayout(ui->splitterModificationsGroups, mIsSelectMode);

    startTimer(1);
}

void FormModificationsGroups::hideEvent(QHideEvent *)
{    
    SEConfig::saveSplitterLayout(ui->splitterModificationsGroups, mIsSelectMode);
    SEConfig::saveTableState(ui->treeViewGroupsM->header(),
                             ui->treeViewGroupsM->objectName(), mIsSelectMode);
}

void FormModificationsGroups::timerEvent(QTimerEvent *event)
{
    killTimer(event->timerId());

    switch (mTypeTimer) {
    case 0:
        refreshData();
        break;
    case 1:
        saveSortGroups();
        break;
    default:
        break;
    }
}

void FormModificationsGroups::onAddEditGroup()
{
    FormModificationGroupCard groupCard;

    if (sender() == ui->actionEditGroup) {
        if (!isEditPermission)
            return;

        QModelIndex index = ui->treeViewGroupsM->currentIndex();
        if (index.isValid())
            groupCard.setGroup(dynamic_cast <DataModificationGroup *> (
                        mModel->dataItem(ui->treeViewGroupsM->currentIndex())));
    }
    if (groupCard.exec() == QDialog::Accepted)
        refreshData();
}

void FormModificationsGroups::onDeleteGroup()
{
    QString msg;
    if (ui->treeViewGroupsM->selectionModel()->selectedRows().count() > 1)
        msg = tr("Удалить выбранные группы?");
    else msg = tr("Удалить группу") + ": " +
            mModel->nameData(ui->treeViewGroupsM->currentIndex()) + "?";

    QMessageBox::StandardButton mesRepl;

    mesRepl = QMessageBox::question(this, tr("Подтверждение удаления"),
                                    msg, QMessageBox::Yes | QMessageBox::No);
    if(mesRepl == QMessageBox::Yes) {
        QModelIndexList list =
                ui->treeViewGroupsM->selectionModel()->selectedRows();
        Request request;
        QModelIndexList::iterator i;
        for (i = list.begin(); i != list.end(); ++i)
            request.addId(mModel->idData((*i)));
        if (ApiAdapter::getApi()->deleteModificationsGroups(request))
            refreshData();
    }
}

void FormModificationsGroups::onUpDownGroup()
{
    if (mIdTimer)
        killTimer(mIdTimer);

    bool isMoveUp = sender() == ui->toolButtonGroupUp;

    QString id;
    QModelIndex index = ui->treeViewGroupsM->currentIndex();
    if (index.isValid())
        id = mModel->idData(ui->treeViewGroupsM->currentIndex());

    mModel->beginReset();
    if (isMoveUp)
        mGroups->moveUp(mModel->dataItem(ui->treeViewGroupsM->selectionModel()->currentIndex()));
    else mGroups->moveUp(mModel->dataItem(ui->treeViewGroupsM->selectionModel()->currentIndex()));
    mModel->endReset();

    goToRowById(id);
    mTypeTimer = 1;
    mIdTimer = startTimer(500);
}

void FormModificationsGroups::onChangeGroups()
{
    QModelIndex index = ui->treeViewGroupsM->currentIndex();

    ui->actionEditGroup->setEnabled(index.isValid() && isEditPermission);
    ui->actionRemoveGroup->setEnabled(index.isValid() && isDeletePermission);

    ui->toolButtonGroupUp->setEnabled(index.isValid() &&
                                      ui->treeViewGroupsM->selectionModel()->currentIndex().row() &&
                                      isEditPermission);
    ui->toolButtonGroupDown->setEnabled(index.isValid() && mModel->rowCount() - 1 -
                                        ui->treeViewGroupsM->selectionModel()->currentIndex().row() &&
                                        isEditPermission);
    ui->toolButtonSelect->setEnabled(index.isValid());
    refreshValues();
}

void FormModificationsGroups::onLoadFromCsv()
{
    QSettings settings;
    QString csvFolder = settings.value("csvFolder",  SEConfig::getFolderDocs()).toString();
    QStringList files =
           QFileDialog::getOpenFileNames(this, tr("Добавить группы модификация"), csvFolder,
                                         tr("CSV файлы") + "(*.csv)");
    if (files.size()) {
        settings.setValue("csvFolder", QFileInfo(files.at(0)).absolutePath());
        settings.sync();
    }
    setCursor(Qt::WaitCursor);
    for (int i = 0; i < files.size(); i++)
        importFromCsv(files.at(i));
    setCursor(Qt::ArrowCursor);
    if (files.size())
        refreshData();
}

void FormModificationsGroups::saveCurrentGroup()
{
    QModelIndex index = ui->treeViewGroupsM->currentIndex();
    if (!index.isValid())
        return;

    DataModificationGroup *group = dynamic_cast <DataModificationGroup *>
            (mModel->dataItem(ui->treeViewGroupsM->currentIndex()));

    group->setIsSetAllFields(true);
    ApiAdapter::getApi()->saveModificationsGroup(group);
}

void FormModificationsGroups::initSignals()
{
    connect(ui->actionAddGroup, SIGNAL(triggered()), SLOT(onAddEditGroup()));
    connect(ui->actionEditGroup, SIGNAL(triggered()), SLOT(onAddEditGroup()));
    connect(ui->actionRemoveGroup, SIGNAL(triggered()), SLOT(onDeleteGroup()));
    connect(ui->actionRefresh, SIGNAL(triggered()), SLOT(refreshData()));
    connect(ui->toolButtonGroupUp, SIGNAL(clicked()), SLOT(onUpDownGroup()));
    connect(ui->toolButtonGroupDown, SIGNAL(clicked()), SLOT(onUpDownGroup()));
    connect(ui->widgetValues, SIGNAL(modified()), SLOT(saveCurrentGroup()));
    connect(ui->toolButtonSelect, SIGNAL(clicked()), SLOT(accept()));
    connect(ui->toolButtonLoadFromCsv, SIGNAL(clicked(bool)), SLOT(onLoadFromCsv()));
}

void FormModificationsGroups::initActions()
{
    ui->toolButtonAddGroup->setDefaultAction(ui->actionAddGroup);
    ui->toolButtonEditGroup->setDefaultAction(ui->actionEditGroup);
    ui->toolButtonDelGroup->setDefaultAction(ui->actionRemoveGroup); 
    ui->toolButtonRefresh->setDefaultAction(ui->actionRefresh);
}

void FormModificationsGroups::initPermissions()
{
    isAddPermission = UserPermission::isAdd(UserPermission::PRODUCTS);
    isEditPermission = UserPermission::isEdit(UserPermission::PRODUCTS);
    isDeletePermission = UserPermission::isDelete(UserPermission::PRODUCTS);

    ui->toolButtonLoadFromCsv->setEnabled(isAddPermission);
    ui->actionAddGroup->setEnabled(isAddPermission);
    ui->actionEditGroup->setEnabled(isEditPermission);
    ui->actionRemoveGroup->setEnabled(isDeletePermission);    
}

void FormModificationsGroups::goToRowById(QString id)
{
    for (int i = 0; i < mModel->rowCount(); i ++)
        if (mModel->getItems()->at(i)->getId() == id) {
            ui->treeViewGroupsM->selectionModel()->
                    setCurrentIndex(mModel->index(i, 0),
                                    QItemSelectionModel::SelectCurrent |
                                    QItemSelectionModel::Rows);
            return;
        }
    if (mModel->rowCount() > 0)
        ui->treeViewGroupsM->selectionModel()->
                setCurrentIndex(mModel->index(0, 0),
                                QItemSelectionModel::SelectCurrent |
                                QItemSelectionModel::Rows);
}

void FormModificationsGroups::initTreeViewGroups()
{
    mModel->appendField(new Field("type", tr("Тип ценообразования")));
    ui->treeViewGroupsM->setModel(mModel);    

    SEConfig::restoreTableState(ui->treeViewGroupsM->header(),
                                    ui->treeViewGroupsM->objectName(), mIsSelectMode);
    ui->treeViewGroupsM->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->treeViewGroupsM->setSelectionMode(QAbstractItemView::ExtendedSelection);
    if (mIsSelectMode)
        connect(ui->treeViewGroupsM, SIGNAL(doubleClicked(QModelIndex)), ui->toolButtonSelect, SLOT(click()));
    else connect(ui->treeViewGroupsM, SIGNAL(doubleClicked(QModelIndex)), ui->actionEditGroup, SLOT(trigger()));
}

void FormModificationsGroups::saveSortGroups()
{
    ApiAdapter::getApi()->sortModificationsGroups(mModel->getSortParamsForApi());
}

void FormModificationsGroups::refreshValues()
{    
    QModelIndex index = ui->treeViewGroupsM->currentIndex();
    if (!index.isValid()) {
            ui->widgetValues->setGroupName(QString());
            ui->widgetValues->refreshData(0);
            return;
    }

    DataModificationGroup *currentGroup = dynamic_cast <DataModificationGroup *>
            (mModel->dataItem(index));

    if (!currentGroup)
        return;

    ui->widgetValues->setGroupName(currentGroup->getName());
    ui->widgetValues->refreshData(currentGroup->columns());
}

void FormModificationsGroups::importFromCsv(const QString &fileName)
{
    QFile file(fileName);
    if (!file.exists())
        return;

    CsvReader *csv = new CsvReader(0, fileName);
    if (csv->open()){
        QList<QStringList> params = csv->CSVRead();
        if (params.size() > 1) {
            DataModificationGroup group;
            QFileInfo fileInfo(file);
            group.setName(fileInfo.baseName());
            group.setType(mTypePriceGroupDef);
            int countColumns = params.at(1).size();
            for (int i = 0; i < countColumns; ++i) {
                DataFeature *feature = new DataFeature;
                feature->setValueType("L");
                if (i < params.at(0).size() && !params.at(0).at(i).isEmpty())
                    feature->setName(params.at(0).at(i));
                else feature->setName(mNameColumnDef);
                if (ApiAdapter::getApi()->saveFeature(feature))
                    group.columns()->append(feature);
            }
            for (int i = 1; i < params.size(); ++i) {
                for (int j = 0; j < params.at(i).size(); ++j) {
                    if (j < group.columns()->size() && !group.columns()->at(j)->getId().isEmpty()) {
                        DataFeature feature;
                        feature.setIdFeature(group.columns()->at(j)->getId());
                        feature.setName(params.at(i).at(j));
                        ApiAdapter::getApi()->saveFeatureValue(&feature);
                    }
                }
            }
            ApiAdapter::getApi()->saveModificationsGroup(&group);
        }
        csv->close();
    }
}
