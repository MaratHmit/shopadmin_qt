#include "formmodificationsgroupsvalues.h"
#include "ui_formmodificationsgroupsvalues.h"
#include "seconfig.h"
#include "seconsts.h"
#include "field.h"
#include "userpermission.h"

#include <QDebug>
#include <QMessageBox>

FormModificationsGroupsValues::FormModificationsGroupsValues(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormModificationsGroupsValues)
{
    initialization();
}

FormModificationsGroupsValues::FormModificationsGroupsValues(QWidget *parent,
                                                             const bool &isDialog) :
    QWidget(parent),
    ui(new Ui::FormModificationsGroupsValues)
{
    initialization();
    mIsDialogMode = isDialog;
}

void FormModificationsGroupsValues::initialization()
{
    ui->setupUi(this);
    mIsDialogMode = false;
    mModel = new NameTableModel(this);
    mModel->clearFields();
    mModel->appendField(new Field("name", tr("Значение")));
    ui->treeViewValues->setModel(mModel);
    mFormFeatures = 0;    
    initSignals();
    initActions();
    initPermissions();
}

void FormModificationsGroupsValues::initSignals()
{
    connect(ui->actionAddValue, SIGNAL(triggered()),
            SLOT(onAddValue()));
    connect(ui->actionRemoveValue, SIGNAL(triggered()),
            SLOT(onDeleteValue()));
}

void FormModificationsGroupsValues::initActions()
{
    ui->toolButtonAddValue->setDefaultAction(ui->actionAddValue);
    ui->toolButtonDelValue->setDefaultAction(ui->actionRemoveValue);
}

void FormModificationsGroupsValues::initPermissions()
{
    isAddPermission = UserPermission::isAdd(UserPermission::PRODUCTS);
    isEditPermission = UserPermission::isEdit(UserPermission::PRODUCTS);
    isDeletePermission = UserPermission::isDelete(UserPermission::PRODUCTS);

    ui->actionAddValue->setEnabled(isAddPermission);
    ui->actionRemoveValue->setEnabled(isDeletePermission);
}

FormModificationsGroupsValues::~FormModificationsGroupsValues()
{    
    delete ui;
}

void FormModificationsGroupsValues::setGroupName(const QString &groupName)
{
    ui->groupBox->setTitle(tr("Состав группы: ") + groupName);
}

void FormModificationsGroupsValues::setShadowMode()
{
    ui->treeViewValues->setStyleSheet("background-color: " + SEConsts::SHADOW_COLOR);
    ui->actionRemoveValue->setVisible(false);
    ui->toolButtonDelValue->hide();
}

void FormModificationsGroupsValues::refreshData(ListFeatures *listValues)
{       
    setCursor(Qt::WaitCursor);
    mModel->beginReset();
    mModel->unBindItems();
    if (listValues)
        mModel->setListData(reinterpret_cast <ListDataItems *>(listValues));
    mModel->endReset();
    setCursor(Qt::ArrowCursor);
}

void FormModificationsGroupsValues::onAddValue()
{
    if (!mFormFeatures) {
        mFormFeatures = new FormFeatures(this, true, true);
    }
    if (mFormFeatures->exec() == QDialog::Accepted) {
        for (int i = 0; i < mFormFeatures->selectedList()->size(); i++) {
            bool isExist = false;
            for (int j = 0; j < mModel->getItems()->size(); j++) {
                isExist = (mModel->getItems()->at(j)->getId() ==
                           mFormFeatures->selectedList()->at(i)->getId());
                if (isExist)
                    break;
            }
            if (isExist)
                continue;
            mModel->getItems()->append(new DataFeature(mFormFeatures->selectedList()->at(i)));
        }
        mModel->resetModel();
        emit modified();
    }
}

void FormModificationsGroupsValues::onDeleteValue()
{
    if (!ui->treeViewValues->currentIndex().isValid())
        return;

    QString msg;
    if (ui->treeViewValues->selectionModel()->selectedRows().count() > 1)
        msg = tr("Удалить выбранные значения?");
    else msg = tr("Удалить значение") + ": " +
            mModel->nameData(ui->treeViewValues->currentIndex()) + "?";

    QMessageBox::StandardButton mesRepl;

    mesRepl = QMessageBox::question(this, tr("Подтверждение удаления"),
                                    msg, QMessageBox::Yes | QMessageBox::No);
    if(mesRepl == QMessageBox::Yes)    {
        QModelIndexList list = ui->treeViewValues->selectionModel()->selectedRows(0);
        for (int i = 0; i < list.size(); i++)
            mModel->dataItem(list.at(i))->setIsChecked(true);
        for (int i = mModel->getItems()->size() - 1; i >= 0; i--)
            if (mModel->getItems()->at(i)->getIsChecked())
                delete mModel->getItems()->takeAt(i);
        mModel->getItems()->setModified();
        mModel->resetModel();
        emit modified();
    }
}

void FormModificationsGroupsValues::showEvent(QShowEvent *)
{    
    SEConfig::restoreTableState(ui->treeViewValues->header(),
                                    ui->treeViewValues->objectName(), mIsDialogMode);

}

void FormModificationsGroupsValues::hideEvent(QHideEvent *)
{    
    SEConfig::saveTableState(ui->treeViewValues->header(),
                                    ui->treeViewValues->objectName(), mIsDialogMode);
}

