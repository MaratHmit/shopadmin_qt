#include "formfeaturesgroups.h"
#include "ui_formfeaturesgroups.h"
#include "formfeaturegroupcard.h"
#include "apiadapter.h"
#include "seconfig.h"
#include "request.h"
#include "field.h"
#include "userpermission.h"

#include <QKeyEvent>

FormFeaturesGroups::FormFeaturesGroups(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FormFeaturesGroups)
{
    ui->setupUi(this);    

    setWindowFlags(Qt::Widget);

    mGroups = new ListDataItems();
    mGroupsModel = new NameTableModel(this, reinterpret_cast <ListDataItems *> (mGroups));    

    mTypeTimer = 0;
    mIdTimer = 0;
    mSelectMode = false;
    mIsShowed = false;
    ui->toolButtonSelect->setHidden(true);
    initTableGroups();
    initButtonsSignals();
    initPermissions();
}

FormFeaturesGroups::~FormFeaturesGroups()
{
    delete ui;    
}

void FormFeaturesGroups::setSelectMode()
{
    mSelectMode = true;
    mGroupsModel->setCheckedMode(false);

    ui->toolButtonEditGroup->setHidden(true);
    ui->toolButtonDelGroup->setHidden(true);
    ui->toolButtonGroupDown->setHidden(true);
    ui->toolButtonGroupUp->setHidden(true);
    ui->line->setHidden(true);    

    ui->toolButtonSelect->setVisible(true);
    disconnect(ui->treeViewGroups, SIGNAL(doubleClicked(QModelIndex)), 0, 0);
    connect(ui->toolButtonSelect, SIGNAL(clicked()),
            SLOT(accept()));
    connect(ui->treeViewGroups, SIGNAL(doubleClicked(QModelIndex)),
            SLOT(accept()));
}

void FormFeaturesGroups::refreshData()
{
    setCursor(Qt::WaitCursor);
    QString id = currentIdGroup();

    mGroupsModel->beginReset();
    ApiAdapter::getApi()->getListFeaturesGroups(mGroupsModel->getItems());
    mGroupsModel->endReset();

    goToRowByIdGroup(id);
    ui->treeViewGroups->setFocus();
    setCursor(Qt::ArrowCursor);
}

void FormFeaturesGroups::onAddEditFeatureGroup()
{
    int isNew = false;

    QToolButton *button = qobject_cast<QToolButton *> (sender());
    isNew = button == ui->toolButtonAddGroup;

    if ((isNew && !isAddPermission) || (!isNew && !isEditPermission))
        return;

    FormFeatureGroupCard *card = new FormFeatureGroupCard(this);
    QString id = currentIdGroup();
    if (!id.isEmpty() && !isNew)
        card->setId(id);
    card->show();

    connect(card, SIGNAL(accepted()), SLOT(refreshData()));
}

void FormFeaturesGroups::onUpDownFeatureGroup()
{
    if (mIdTimer)
        killTimer(mIdTimer);

    QToolButton *button = qobject_cast<QToolButton *> (sender());
    bool isMoveUp = button == ui->toolButtonGroupUp;

    QString id = currentIdGroup();
    mGroupsModel->beginReset();
    if (isMoveUp)
        mGroups->moveUp(mGroupsModel->dataItem((ui->treeViewGroups->selectionModel()->currentIndex())));
    else mGroups->moveDown(mGroupsModel->dataItem((ui->treeViewGroups->selectionModel()->currentIndex())));
    mGroupsModel->endReset();

    goToRowByIdGroup(id);
    mTypeTimer = 1;
    mIdTimer = startTimer(1000);
}

void FormFeaturesGroups::onChangeGroups()
{
    QString id = currentIdGroup();

    ui->toolButtonEditGroup->setEnabled(!id.isEmpty() && isEditPermission);
    ui->toolButtonDelGroup->setEnabled(!id.isEmpty() && isDeletePermission);

    ui->toolButtonGroupUp->setEnabled(!id.isEmpty() &&
                                      ui->treeViewGroups->
                                      selectionModel()->currentIndex().row() && isEditPermission);
    ui->toolButtonGroupDown->setEnabled(!id.isEmpty() &&
                                        mGroupsModel->rowCount() - 1 -
                                        ui->treeViewGroups->
                                        selectionModel()->currentIndex().row() && isEditPermission);
}

void FormFeaturesGroups::onCheckedGroups()
{
    emit checkedGroups();
}

void FormFeaturesGroups::on_toolButtonDelGroup_clicked()
{
    QString msg;
    if (ui->treeViewGroups->selectionModel()->selectedRows().count() > 1)
        msg = tr("Удалить выбранные группы?");
    else msg = tr("Удалить группу") + ": " +
            mGroupsModel->nameData(ui->treeViewGroups->currentIndex()) + "?";

    QMessageBox::StandardButton mesRepl;

    mesRepl = QMessageBox::question(this, tr("Подтверждение удаления"),
                                    msg, QMessageBox::Yes | QMessageBox::No);
    if(mesRepl == QMessageBox::Yes)    {
        Request request;
        QModelIndexList list = ui->treeViewGroups->selectionModel()->selectedRows(0);

        for (int i = 0; i < list.count(); i++)
            request.addId(mGroupsModel->idData(list.at(i)));
        ApiAdapter::getApi()->deleteFeaturesGroups(request);
        refreshData();
    }
}

void FormFeaturesGroups::showEvent(QShowEvent *)
{
    startTimer(1);
}

void FormFeaturesGroups::timerEvent(QTimerEvent *event)
{
    killTimer(event->timerId());
    switch (mTypeTimer) {
    case 0:
        if (!mIsShowed) {
            refreshData();
            mIsShowed = true;
        }
        break;
    case 1:
        saveSortGroups();
        break;
    default:
        break;
    }
}

bool FormFeaturesGroups::eventFilter(QObject *, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        switch (keyEvent->key()) {
         case Qt::Key_Up:
             if (keyEvent->modifiers() & Qt::ControlModifier &&
                     ui->treeViewGroups->hasFocus()) {
                 emit ui->toolButtonGroupUp->clicked();
                 return true;
             }
             break;
         case Qt::Key_Down:
             if (keyEvent->modifiers() & Qt::ControlModifier &&
                     ui->treeViewGroups->hasFocus()) {
                 emit ui->toolButtonGroupDown->clicked();
                 return true;
             }
             break;
        case Qt::Key_Insert:
            if (ui->treeViewGroups->hasFocus()) {
                emit ui->toolButtonAddGroup->click();
                return true;
            }
            break;
        case Qt::Key_Delete:
            if (keyEvent->modifiers() & Qt::ControlModifier &&
                    ui->toolButtonDelGroup->isEnabled() &&
                    ui->treeViewGroups->hasFocus()) {
                emit ui->toolButtonDelGroup->click();
                return true;
            }
            break;
        }
    }

    return false;
}

QString FormFeaturesGroups::currentIdGroup() const
{
    QModelIndex index = ui->treeViewGroups->currentIndex();
    if (index.isValid())
        return mGroupsModel->idData(index);
    else return 0;
}

QString FormFeaturesGroups::currentNameGroup() const
{
    QModelIndex index = ui->treeViewGroups->currentIndex();
    if (index.isValid())
        return mGroupsModel->nameData(index);
    else return QString();
}

QStringList FormFeaturesGroups::idsChecked() const
{
    return mGroupsModel->idsChecked();
}

void FormFeaturesGroups::setShadowMode()
{
    ui->treeViewGroups->setStyleSheet("background-color: " + SEConsts::SHADOW_COLOR);
    ui->toolButtonDelGroup->hide();
    ui->toolButtonGroupDown->hide();
    ui->toolButtonGroupUp->hide();
    ui->lineRefresh->hide();
}

void FormFeaturesGroups::saveSortGroups()
{    
    ApiAdapter::getApi()->sortFeatureGroups(mGroupsModel->getSortParamsForApi());
}

void FormFeaturesGroups::initTableGroups()
{
    ui->treeViewGroups->setModel(mGroupsModel);
    mGroupsModel->setCheckedMode(true);

    ui->treeViewGroups->header()->setStretchLastSection(true);
    ui->treeViewGroups->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->treeViewGroups->setSelectionMode(QAbstractItemView::ExtendedSelection);

    connect(ui->treeViewGroups->selectionModel(),
            SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            SLOT(onChangeGroups()));
    connect(mGroupsModel, SIGNAL(checked(QString,bool)),
            SLOT(onCheckedGroups()));

    ui->treeViewGroups->installEventFilter(this);
}

void FormFeaturesGroups::initButtonsSignals()
{
    connect(ui->toolButtonAddGroup, SIGNAL(clicked()), SLOT(onAddEditFeatureGroup()));
    connect(ui->toolButtonEditGroup, SIGNAL(clicked()), SLOT(onAddEditFeatureGroup()));
    connect(ui->treeViewGroups, SIGNAL(doubleClicked(QModelIndex)),
            SLOT(onAddEditFeatureGroup()));
    connect(ui->toolButtonGroupDown, SIGNAL(clicked()), SLOT(onUpDownFeatureGroup()));
    connect(ui->toolButtonGroupUp, SIGNAL(clicked()), SLOT(onUpDownFeatureGroup()));
    connect(ui->toolButtonRefresh, SIGNAL(clicked()), SLOT(refreshData()));
}

void FormFeaturesGroups::initPermissions()
{
    isAddPermission = UserPermission::isAdd(UserPermission::PRODUCTS);
    isEditPermission = UserPermission::isEdit(UserPermission::PRODUCTS);
    isDeletePermission = UserPermission::isDelete(UserPermission::PRODUCTS);

    ui->toolButtonAddGroup->setEnabled(isAddPermission);
    ui->toolButtonEditGroup->setEnabled(isEditPermission);
    ui->toolButtonDelGroup->setEnabled(isDeletePermission);
}

void FormFeaturesGroups::goToRowByIdGroup(QString id)
{
    QModelIndex modelIndex = mGroupsModel->getModelIndexById(id);

    if (modelIndex.isValid()) {
        ui->treeViewGroups->selectionModel()->setCurrentIndex(
                    modelIndex, QItemSelectionModel::SelectCurrent | QItemSelectionModel::Rows);
        return;
    }

    if (mGroupsModel->rowCount() > 0)
        ui->treeViewGroups->selectionModel()->
                setCurrentIndex(mGroupsModel->index(0, 0),
                                QItemSelectionModel::SelectCurrent |
                                QItemSelectionModel::Rows);
    else onChangeGroups();
}
