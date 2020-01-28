#include "framecardmodifications.h"
#include "ui_framecardmodifications.h"
#include "apiadapter.h"
#include "widgetpagemodifications.h"
#include "formmodificationsgroups.h"

#include <QTabWidget>
#include <QDebug>
#include <QSettings>

FrameCardModifications::FrameCardModifications(
        QWidget *parent, ListModificationsGroups *modificationsGroups,
        ListDataItems *imagesProduct, const bool &isDialogMode) :
    QDialog(parent),
    ui(new Ui::FrameCardModifications)
{
    ui->setupUi(this);    
    mIsSelectMode = false;
    mIsDialogMode = isDialogMode;
    mImagesProduct = imagesProduct;
    mModificationsGroups = modificationsGroups;
    initVariables();
    initSignals();
}

FrameCardModifications::FrameCardModifications(QWidget *parent, ListModificationsGroups *modificationsGroups,
                                               const bool &isMultiSelect) :
    QDialog(parent),
    ui(new Ui::FrameCardModifications)
{
    ui->setupUi(this);    
    mIsSelectMode = true;
    mIsDialogMode = true;
    mIsMultiSelect = isMultiSelect;
    mImagesProduct = 0;
    mModificationsGroups = modificationsGroups;
    initVariables();
    initSignals();
}

FrameCardModifications::~FrameCardModifications()
{ 
    delete mListPages;
    delete ui;
    delete mSelectedModifications;
}

void FrameCardModifications::setIdModificationGroupDef(const QString &idModificationGroupDef)
{
    mIdModificationGroupDef = idModificationGroupDef;
}

ListModifications *FrameCardModifications::selectedModifications() const
{
    mSelectedModifications->clear();
    for (int i = 0; i < mListPages->size(); i++)
        for (int j = 0; j < mListPages->at(i)->selectedModifications()->size(); j++)
            mSelectedModifications->append(mListPages->at(i)->selectedModifications()->at(j));
    return mSelectedModifications;
}

void FrameCardModifications::initVariables()
{
    QSettings settings;
    ui->checkBoxIsAutoFilledModifications->setChecked(settings.value("IsAutoFilledModifications", true).toBool());
    ui->toolButtonSelect->hide();
    ui->toolButtonSelect->setEnabled(false);
    if (mIsDialogMode) {
        setWindowFlags(Qt::Window);
        connect(ui->buttonOK, &QPushButton::clicked, this, &QDialog::accept);
        connect(ui->buttonCancel, &QPushButton::clicked, this, &QDialog::reject);
    }
    else {
        setWindowFlags(Qt::Widget);
        ui->buttonOK->hide();
        ui->buttonCancel->hide();
    }
    if (mIsSelectMode) {
        ui->buttonOK->hide();
        ui->buttonCancel->hide();
        ui->toolButtonAddTypeGoods->hide();
        ui->toolButtonSelect->show();
    }
    mListPages = new QList<WidgetPageModifications *>();
    mIsFilled = false;
    mSelectedModifications = new ListModifications();
}

void FrameCardModifications::fillControls()
{
    if (mIsFilled)
        return;

    initGroups();
    mModificationsGroups->setUnModified();
    mIsFilled = true;
}

void FrameCardModifications::onAddGroups()
{    
    FormModificationsGroups formGroups(this, true);

    if (mModificationsGroups->size()) {
        ListModificationsGroups list;
        for (int i = 0; i < mModificationsGroups->size(); i++)
            list.append(new DataModificationGroup(mModificationsGroups->at(i)));
        formGroups.setExludingGroups(&list);
    }

    if (formGroups.exec()) {
        for (int i = 0; i < formGroups.selectedGroups()->size(); i++)
            addPage(formGroups.selectedGroups()->at(i), true);
        onModified();
    }

    QSettings settings;
    settings.setValue("IsAutoFilledModifications", ui->checkBoxIsAutoFilledModifications->isChecked());
    settings.sync();
}

void FrameCardModifications::onRemoveGroup(const int &index)
{
    WidgetPageModifications *page = 0;
    for (int i = 0; i < mListPages->size(); i++)
        if (ui->tabWidget->indexOf(mListPages->at(i)) == index) {
            page = mListPages->takeAt(i);
            break;
        }

    if (page) {        
        ui->tabWidget->removeTab(index);
        mModificationsGroups->deleteItem(page->groupInfo());
    }

    emit modified();
}

void FrameCardModifications::onModified()
{
    mModificationsGroups->setModified();
    emit modified();
}

void FrameCardModifications::onChangeItemModification(const QModelIndex &index)
{
    ui->toolButtonSelect->setEnabled(index.isValid());
}

void FrameCardModifications::addPage(DataModificationGroup *group,
                                     const bool &isNew)
{
    if (!group->columns()->size())
        return;

    if (WidgetPageModifications *page = getPageById(group->getId())) {        
        ui->tabWidget->addTab(page, group->getName());
        for (int i = 0; i < mModificationsGroups->size(); i++)
            if (mModificationsGroups->at(i)->getId() == group->getId()) {
                mModificationsGroups->at(i)->setIsChecked(true);
                break;
            }
        return;
    }

    DataModificationGroup *newGroup;
    if (isNew) {
        newGroup = new DataModificationGroup(group);
        mModificationsGroups->append(newGroup);
    } else newGroup = group;

    WidgetPageModifications *page = new WidgetPageModifications(
                this, newGroup, mImagesProduct, mIsSelectMode, mIsMultiSelect);
    if (isNew && ui->checkBoxIsAutoFilledModifications->isChecked())
        page->setIsAutoFilledModifications();

    connect(page, SIGNAL(modified()), SLOT(onModified()));
    connect(page, SIGNAL(changeItem(QModelIndex)), SLOT(onChangeItemModification(QModelIndex)));
    if (mIsSelectMode)
        connect(page, SIGNAL(doubleClicked()), ui->toolButtonSelect, SLOT(click()));

    newGroup->setIsActive(true);
    ui->tabWidget->addTab(page, newGroup->getName());
    mListPages->push_back(page);    
}


WidgetPageModifications *FrameCardModifications::getPageById(const QString &id) const
{
    for (int i = 0; i < mListPages->size(); i++) {        
        if (mListPages->at(i)->groupInfo()->getId() == id)
            return mListPages->at(i);
    }
    return 0;
}

void FrameCardModifications::initSignals()
{
    connect(ui->toolButtonAddTypeGoods, SIGNAL(clicked()), SLOT(onAddGroups()));
    connect(ui->toolButtonSelect, SIGNAL(clicked()), SLOT(accept()));    
    connect(ui->tabWidget, &QTabWidget::tabCloseRequested, this, &FrameCardModifications::onRemoveGroup);
}

void FrameCardModifications::initGroups()
{
    if (!mModificationsGroups)
        return;

    if (!mModificationsGroups->size() && !mIdModificationGroupDef.isEmpty()) {
        DataModificationGroup *group = new DataModificationGroup;
        group->setId(mIdModificationGroupDef);
        if (ApiAdapter::getApi()->getInfoModificationGroup(group)) {
            mModificationsGroups->append(group);
            addPage(group, true);
        }
        return;
    }

    for (int i = 0; i < mModificationsGroups->size(); i++) {
        mModificationsGroups->at(i)->setIsChecked(true);
        mModificationsGroups->at(i)->setUnModified();
        addPage(mModificationsGroups->at(i));
    }
}


