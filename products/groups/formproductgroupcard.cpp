#include "formproductgroupcard.h"
#include "ui_formproductgroupcard.h"
#include "apiadapter.h"
#include "formproductsgroups.h"
#include "formdiscounts.h"
#include "formparametersfilters.h"
#include "formproducts.h"

#include <QStyle>
#include <QTableWidgetItem>
#include <QToolButton>
#include <QDir>
#include <QDebug>

FormProductGroupCard::FormProductGroupCard(QWidget *parent) :
    QDialog(parent), ui(new Ui::FormProductGroupCard)
{
    ui->setupUi(this);    
    setWindowFlags(Qt::Window);    
    mGroup = new DataProductsGroup();     
    mIsMultiMode = 0;
    mDescriptionEditor = 0;
    mFrameListImages = new FrameListImages(this, SEConsts::Section_GroupsProducts, mGroup->listImages());
    mFrameListImages->hideCheckedsColumn();
    ui->labelParentGroup->hide();
    ui->lineEditParentGroup->hide();
    ui->toolButtonParentGroup->hide();
    ui->toolButtonClearParentGroup->hide();
    ui->groupBoxIncPrices->hide();
    ui->stackedWidget->addWidget(mFrameListImages);
    ui->widgetSEO->setSEOType(FrameSEO::SEOGoodsGroups);
    if (SEConfig::coreVersion != "5.2")
        ui->pushButtonLinksGroups->setText("Связанные товары");
    if (SEConfig::getMainInfo()->isShowIncPrices())
        ui->groupBoxIncPrices->show();

    ui->stackedWidget->setCurrentIndex(0);
    initLineEditParentGroup();
    initDiscounstModel();
    initLinksModel();
    initSimilarsModel();
    initSubgroupsModel();
    initFiltersModel();    

    ui->widgetImage->setSection(SEConsts::Section_GroupsProducts);
    ui->widgetNote->setSection(SEConsts::Section_GroupsProducts);

    initSignals();
}

FormProductGroupCard::~FormProductGroupCard()
{    
    delete ui;
    delete mDiscountsModel;
    delete mLinksModel;
    delete mGroup;
}

void FormProductGroupCard::setId(const QString &idGroup)
{
    mGroup->setId(idGroup);
    ui->labelParentGroup->show();
    ui->lineEditParentGroup->show();
    ui->toolButtonParentGroup->show();
    ui->toolButtonClearParentGroup->show();
}

void FormProductGroupCard::setIds(const QStringList &ids)
{
    foreach (QString id, ids)
        mGroup->addId(id);
    mIsMultiMode = true;
    ui->pushButtonMainInfo->hide();
    ui->pushButtonDescription->hide();
    ui->pushButtonPicture->hide();
    ui->stackedWidget->removeWidget(ui->pageMain);
    ui->stackedWidget->removeWidget(ui->pageDescription);
    ui->stackedWidget->removeWidget(ui->pageImage);
    ui->pushButtonDiscounts->setChecked(1);
    ui->buttonSave->setEnabled(1);
}

void FormProductGroupCard::setIdParentGroup(const QString &idParent,
                                            const QString &titleParent)
{
    mGroup->setIdParent(idParent);
    ui->lineEditParentGroup->setText(titleParent);
}

DataProductsGroup *FormProductGroupCard::group() const
{
    return mGroup;
}

void FormProductGroupCard::showEvent(QShowEvent *)
{
    initGoodsTypes();
    fillControls();
    ui->lineEditName->setFocus();
}

void FormProductGroupCard::fillControls()
{
    if (mGroup->getId().isEmpty())
        return;

    QStringList ids = mGroup->getIds();
    ApiAdapter::getApi()->getInfoProductGroup(mGroup);
    foreach (QString id, ids)
        mGroup->addId(id);

    ui->checkBox->setChecked(mGroup->getIsActive());
    ui->lineEditName->setText(mGroup->getName());    
    ui->lineEditCode->setText(mGroup->getCode());
    if (!mGroup->getId().isEmpty())
        ui->widgetNote->setHTMLText(mGroup->property("description").toString());

    ui->widgetImage->setImage(mGroup->getImageFile());
    ui->widgetImage->setImageAlt(mGroup->getImageAlt());

    ui->widgetSEO->setSEOHeader(mGroup->property("seoHeader").toString());
    ui->widgetSEO->setSEOKeywords(mGroup->property("seoKeywords").toString());
    ui->widgetSEO->setSEODescription(mGroup->property("seoDescription").toString());
    ui->widgetSEO->setPageTitle(mGroup->property("pageTitle").toString());

    if (mGroup->property("sourcePrice").toString() == "OFFICE_ZAKAZ")
        ui->comboBoxSourcePrices->setCurrentIndex(1);

    ui->spinBoxIncPrice->setValue(mGroup->property("incPrice").toInt());
    ui->spinBoxIncPriceOpt->setValue(mGroup->property("incPriceOpt").toInt());
    ui->spinBoxIncPriceCorp->setValue(mGroup->property("incPriceCorp").toInt());

    for (int i = 0; i < mGroup->filters()->size(); i++)
        mGroup->filters()->at(i)->setIsChecked(mGroup->filters()->at(i)->getIsActive());

    if (!mGroup->idModificationGroupDef().isEmpty())
        for (int i = 0; i < ui->comboBoxGoodsTypesDef->count(); ++i)
            if (ui->comboBoxGoodsTypesDef->itemData(i).toString() == mGroup->idModificationGroupDef()) {
                ui->comboBoxGoodsTypesDef->setCurrentIndex(i);
                break;
            }
    mGroup->setUnModifiedAll();        
}

void FormProductGroupCard::on_buttonSave_clicked()
{
    ui->buttonCancel->setEnabled(false);
    ui->buttonSave->setEnabled(false);

    setData();            
    if (save())
        accept();

    ui->buttonCancel->setEnabled(true);
    ui->buttonSave->setEnabled(true);
}

void FormProductGroupCard::setData()
{    
    if (!mIsMultiMode) {
        if (mDescriptionEditor) {
            mDescriptionEditor->refreshSourceText();
            mGroup->setFullDescription(mDescriptionEditor->textHTML());
        }
        ui->widgetNote->refreshSourceText();
        mGroup->setName(ui->lineEditName->text());
        mGroup->setCode(ui->lineEditCode->text());
        mGroup->setDescription(ui->widgetNote->textHTML());        
        mGroup->setProperty("note", ui->widgetNote->textHTML());
        mGroup->setImageFile(ui->widgetImage->fileImageName());
        mGroup->setImageAlt(ui->widgetImage->altImage());
    }
    mGroup->setSEOHeader(ui->widgetSEO->seoHeader());
    mGroup->setSEOKeywords(ui->widgetSEO->seoKeywords());
    mGroup->setSEODescription(ui->widgetSEO->seoDescription());
    mGroup->setPageTitle(ui->widgetSEO->pageTitle());
    mGroup->setIsActive(ui->checkBox->isChecked());    
    mGroup->setIdModificationGroupDef(ui->comboBoxGoodsTypesDef->currentData().toString());
    mGroup->setProperty("incPrice", ui->spinBoxIncPrice->value());
    mGroup->setProperty("incPriceOpt", ui->spinBoxIncPriceOpt->value());
    mGroup->setProperty("incPriceCorp", ui->spinBoxIncPriceCorp->value());
    if (!ui->comboBoxSourcePrices->currentIndex())
        mGroup->setProperty("sourcePrice", "FILE");
    else mGroup->setProperty("sourcePrice", "OFFICE_ZAKAZ");

    for (int i = 0; i < mGroup->filters()->size(); i++)
        mGroup->filters()->at(i)->setIsActive(mGroup->filters()->at(i)->getIsChecked());
}

void FormProductGroupCard::checkEnabled()
{
    ui->buttonSave->setEnabled(mIsMultiMode || !ui->lineEditName->text().trimmed().isEmpty());
}

void FormProductGroupCard::on_pushButtonMainInfo_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->pageMain);
    ui->lineEditName->setFocus();
}

void FormProductGroupCard::on_pushButtonDescription_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->pageDescription);
    if (mDescriptionEditor)
        return;

    mDescriptionEditor = new FrameWYSIWYG(this);
    mDescriptionEditor->setSection(SEConsts::Section_GroupsProducts);
    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(mDescriptionEditor);
    layout->setMargin(0);
    ui->widgetFullDescription->setLayout(layout);
    if (!mGroup->getId().isEmpty())
        mDescriptionEditor->setHTMLText(mGroup->property("fullDescription").toString());
}

void FormProductGroupCard::on_pushButtonPicture_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->pageImage);
}

void FormProductGroupCard::on_pushButtonDiscounts_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->pageDiscounts);
    mDiscountsModel->resetModel();
    for (int i = 0; i < mDiscountsModel->columnCount() - 1; i++)
        ui->treeViewDiscounts->resizeColumnToContents(i);
}

void FormProductGroupCard::on_pushButtonSEO_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->pageSEO);
    ui->widgetSEO->fillControls();
}

void FormProductGroupCard::on_pushButtonLinksGroups_clicked()
{    
    ui->stackedWidget->setCurrentWidget(ui->pageLinksGroups);
    mLinksModel->resetModel();
    for (int i = 0; i < mLinksModel->columnCount() - 1; i++)
        ui->treeViewLinksGroups->resizeColumnToContents(i);
    if (mLinksModel->rowCount())
        ui->treeViewLinksGroups->selectionModel()->setCurrentIndex(mLinksModel->index(0, 0),
                                                                   QItemSelectionModel::SelectCurrent | QItemSelectionModel::Rows);
}

void FormProductGroupCard::on_pushButtonFilters_clicked()
{    
    ui->stackedWidget->setCurrentWidget(ui->pageParametersFilters);
    mFiltersModel->resetModel();
}

void FormProductGroupCard::initLineEditParentGroup()
{
    QPalette pal;
    SEConfig::setPaletteReadOnly(&pal);
    ui->lineEditParentGroup->setPalette(pal);
    ui->lineEditParentGroup->setReadOnly(true);
}

void FormProductGroupCard::initDiscounstModel()
{
    mDiscountsModel = new NameTableModel(this, reinterpret_cast <ListDataItems *> (mGroup->discounts()));
    mDiscountsModel->clearFields();
    mDiscountsModel->appendField(new Field("id", "Ид."));
    mDiscountsModel->appendField(new Field("name", "Наименование"));
    mDiscountsModel->appendField(new Field("dateTimeFrom", "Старт"));
    mDiscountsModel->appendField(new Field("dateTimeTo", "Завершение"));
    mDiscountsModel->appendField(new Field("stepTime", "Шаг времени", Qt::AlignRight | Qt::AlignVCenter));
    mDiscountsModel->appendField(new Field("stepDiscount", "Шаг скидки", Qt::AlignRight | Qt::AlignVCenter));
    mDiscountsModel->appendField(new Field("sumFrom", "От суммы", Qt::AlignRight | Qt::AlignVCenter));
    mDiscountsModel->appendField(new Field("sumTo", "До суммы", Qt::AlignRight | Qt::AlignVCenter));
    mDiscountsModel->appendField(new Field("countFrom", "От кол-во", Qt::AlignRight | Qt::AlignVCenter));
    mDiscountsModel->appendField(new Field("countTo", "До кол-во", Qt::AlignRight | Qt::AlignVCenter));
    mDiscountsModel->appendField(new Field("discount", "Скидка", Qt::AlignRight | Qt::AlignVCenter));
    mDiscountsModel->appendField(new Field("week", "Дни недели"));
    ui->treeViewDiscounts->setModel(mDiscountsModel);
}

void FormProductGroupCard::initLinksModel()
{
    mLinksModel = new NameTableModel(this, reinterpret_cast <ListDataItems *> (mGroup->linksGroups()));
    mLinksModel->clearFields();
    mLinksModel->appendField(new Field("id", "Ид."));    
    mLinksModel->appendField(new Field("name", "Наименование"));
    ui->treeViewLinksGroups->setModel(mLinksModel);
}

void FormProductGroupCard::initSimilarsModel()
{
    mSimilarsModel = new NameTableModel(this, reinterpret_cast <ListDataItems *> (mGroup->similarGroups()));
    mSimilarsModel->clearFields();
    mSimilarsModel->appendField(new Field("id", "Ид."));
    mSimilarsModel->appendField(new Field("name", "Наименование"));
    ui->treeViewSimilarsGroups->setModel(mSimilarsModel);
}

void FormProductGroupCard::initSubgroupsModel()
{
    ui->pushButtonAdditionalSubgroups->hide();

    mSubgroupsModel = new NameTableModel(this, reinterpret_cast <ListDataItems *> (mGroup->additionalSubgroups()));
    mSubgroupsModel->clearFields();
    mSubgroupsModel->appendField(new Field("id", "Ид."));
    mSubgroupsModel->appendField(new Field("name", "Наименование"));
    ui->treeViewSubgroups->setModel(mSubgroupsModel);
}

void FormProductGroupCard::initFiltersModel()
{
    mFiltersModel = new NameTableModel(this, mGroup->filters());
    mFiltersModel->setCheckedMode(true);
    ui->treeViewFilters->setModel(mFiltersModel);
}

void FormProductGroupCard::initSignals()
{
    connect(ui->toolButtonAddDiscount, &QAbstractButton::clicked, this,
            &FormProductGroupCard::addDiscount);
    connect(ui->toolButtonDelDiscount, &QAbstractButton::clicked, this,
            &FormProductGroupCard::deleteDiscounts);
    connect(ui->treeViewFilters->selectionModel(), &QItemSelectionModel::currentChanged,
            this, &FormProductGroupCard::onChangeFilterItem);
    connect(ui->toolButtonSortDown, SIGNAL(clicked()), SLOT(onSort()));
    connect(ui->toolButtonSortUp, SIGNAL(clicked()), SLOT(onSort()));
    connect(mFiltersModel, &NameTableModel::checked, this, &FormProductGroupCard::onCheckedFilter);
    connect(mGroup, &DataProductsGroup::modified, this, &FormProductGroupCard::checkEnabled);
    connect(ui->widgetSEO, &FrameSEO::modified, this, &FormProductGroupCard::checkEnabled);
    connect(ui->pushButtonPictures, SIGNAL(clicked(bool)), SLOT(onShowListPictures()));
    connect(ui->comboBoxSourcePrices, SIGNAL(currentIndexChanged(int)), SLOT(changeSourcePrices(int)));
}

void FormProductGroupCard::initGoodsTypes()
{
    ui->comboBoxGoodsTypesDef->addItem(tr("Без типа"), QString());
    ListModificationsGroups groups;
    if (ApiAdapter::getApi()->getListModificationsGroups(&groups)) {
        for (int i = 0; i < groups.size(); ++i)
            ui->comboBoxGoodsTypesDef->addItem(groups.at(i)->getName(), groups.at(i)->getId());
        ui->comboBoxGoodsTypesDef->setCurrentIndex(0);
    }
}

void FormProductGroupCard::addDiscount()
{
    FormDiscounts form(this);
    form.setSelectMode(true);
    if (form.exec() ==  QDialog::Accepted) {
        const ListDiscounts *list = form.selectedDiscounts();
        for (int i = 0; i < list->size(); i++)
            if (!mGroup->discounts()->existById(list->at(i)))
                mGroup->discounts()->append(new DataDiscount(list->at(i)));
        mDiscountsModel->resetModel();
        checkEnabled();
    } 
}

void FormProductGroupCard::deleteDiscounts()
{
    QModelIndexList list =
            ui->treeViewDiscounts->selectionModel()->selectedRows();
    if (!list.size())
        return;

    mDiscountsModel->beginReset();
    QStringList idsDelete;
    for (int i = 0; i < list.size(); i++)
        idsDelete.append(mGroup->discounts()->at(list.at(i).row())->getId());
    for (int i = mGroup->discounts()->size() - 1; i >= 0; i--)
        if (idsDelete.contains(mGroup->discounts()->at(i)->getId()))
            delete mGroup->discounts()->takeAt(i);
    mDiscountsModel->endReset();
    checkEnabled();
}

void FormProductGroupCard::onChangeFilterItem()
{
    ui->toolButtonDelFilter->setEnabled(ui->treeViewFilters->currentIndex().isValid());
    ui->toolButtonSortDown->setEnabled(ui->treeViewFilters->currentIndex().isValid());
    ui->toolButtonSortUp->setEnabled(ui->treeViewFilters->currentIndex().isValid());
}

void FormProductGroupCard::onCheckedFilter()
{
    mGroup->filters()->setModified();
    checkEnabled();
}

void FormProductGroupCard::on_toolButtonAddGroupd_clicked()
{
    if (SEConfig::coreVersion != "5.2") {
        FormProducts form(this, true);
        if (form.exec() == QDialog::Accepted) {
            for (int i = 0; i < form.getSelectedProducts()->size(); ++i)
                mGroup->addLinkGroup(form.getSelectedProducts()->at(i));
            mLinksModel->resetModel();
            for (int i = 0; i < mLinksModel->columnCount() - 1; i++)
                ui->treeViewLinksGroups->resizeColumnToContents(i);
            ui->treeViewLinksGroups->selectionModel()->setCurrentIndex(mLinksModel->index(mLinksModel->rowCount() - 1, 0),
                                                                       QItemSelectionModel::SelectCurrent | QItemSelectionModel::Rows);
        }
    } else {
        FormProductsGroups form(this);

        form.setWindowFlags(Qt::Dialog);
        form.setSelectMode(true);
        if (form.exec() == QDialog::Accepted) {
            QListIterator<DataProductsGroup *> it(form.listSelected());
            while (it.hasNext()) {
                DataProductsGroup *gr = (it.next());
                mGroup->addLinkGroup(gr);
            }
            mLinksModel->resetModel();
            for (int i = 0; i < mLinksModel->columnCount() - 1; i++)
                ui->treeViewLinksGroups->resizeColumnToContents(i);
            ui->treeViewLinksGroups->selectionModel()->setCurrentIndex(mLinksModel->index(mLinksModel->rowCount() - 1, 0),
                                                                       QItemSelectionModel::SelectCurrent | QItemSelectionModel::Rows);
        }
    }
}


void FormProductGroupCard::on_toolButtonParentGroup_clicked()
{
    FormProductsGroups *form = new FormProductsGroups(this);

    form->setWindowFlags(Qt::Dialog);
    form->setSelectMode(false);
    if (!mGroup->getId().isEmpty()) {
        ListProductsGroups groups;
        groups.append(new DataProductsGroup(mGroup));
        form->setExludingGroups(&groups);
    }
    if (form->exec() == QDialog::Accepted) {
        QListIterator<DataProductsGroup *> it(form->listSelected());
        if (it.hasNext()) {
            DataProductsGroup *gr = (it.next());
            setIdParentGroup(gr->getId(), gr->getName());
        }
    }
    delete form;
}

void FormProductGroupCard::on_toolButtonClearParentGroup_clicked()
{
    setIdParentGroup(0, "");
    checkEnabled();
}

void FormProductGroupCard::on_lineEditName_textChanged(const QString &)
{    
    checkEnabled();
}

bool FormProductGroupCard::save()
{    
    if (ApiAdapter::getApi()->saveProductGroup(mGroup)) {
        ui->widgetImage->saveImage();
        return ApiAdapter::getApi()->getInfoProductGroup(mGroup);
    }
    return false;
}

void FormProductGroupCard::on_toolButtonDelGroup_clicked()
{
    QModelIndexList list =
            ui->treeViewLinksGroups->selectionModel()->selectedRows();
    if (!list.size())
        return;

    mLinksModel->beginReset();
    QStringList idsDelete;
    for (int i = 0; i < list.size(); i++)
        idsDelete.append(mGroup->linksGroups()->at(list.at(i).row())->getId());
    for (int i = mGroup->linksGroups()->size() - 1; i >= 0; i--)
        if (idsDelete.contains(mGroup->linksGroups()->at(i)->getId()))
            delete mGroup->linksGroups()->takeAt(i);
    mGroup->linksGroups()->setModified();
    mLinksModel->endReset();
    checkEnabled();
}

void FormProductGroupCard::on_toolButtonAddFilter_clicked()
{
    FormParametersFilters form(this);
    form.setExistingFilters(mGroup->filters());

    if (form.exec() ==  QDialog::Accepted) {
        for (int i = 0; i < form.selectedItems()->size(); i++)
            mGroup->filters()->append(new DataItem(form.selectedItems()->at(i)));
        mFiltersModel->resetModel();
        mGroup->filters()->setModified();
        checkEnabled();
    }
}

void FormProductGroupCard::on_toolButtonDelFilter_clicked()
{
    QModelIndexList list =
            ui->treeViewFilters->selectionModel()->selectedRows();
    if (!list.size())
        return;

    mLinksModel->beginReset();
    for (int i = 0; i < list.size(); i++)
        mGroup->filters()->at(list.at(i).row())->setIsSelected(true);
    for (int i = mGroup->filters()->size() - 1; i >= 0; --i)
        if (mGroup->filters()->at(i)->getIsSelected())
            delete mGroup->filters()->takeAt(i);

    mFiltersModel->endReset();
    mGroup->filters()->setModified();
    onChangeFilterItem();
    checkEnabled();
}

void FormProductGroupCard::onSort()
{    
    if (!ui->treeViewFilters->currentIndex().isValid())
        return;

    bool isUp = ui->toolButtonSortUp == sender();

    int index = ui->treeViewFilters->currentIndex().row();
    int newIndex = index;
    if (isUp)
        newIndex--;
    else newIndex++;
    if (newIndex < 0)
        newIndex = 0;
    if (newIndex == mGroup->filters()->size())
        newIndex = mGroup->filters()->size() - 1;

    mFiltersModel->beginReset();
    if (isUp)
        mGroup->filters()->moveUp(mFiltersModel->dataItem(ui->treeViewFilters->currentIndex()));
    else mGroup->filters()->moveDown(mFiltersModel->dataItem(ui->treeViewFilters->currentIndex()));    
    mFiltersModel->endReset();
    ui->treeViewFilters->selectionModel()->setCurrentIndex(
                mFiltersModel->index(newIndex, 0), QItemSelectionModel::SelectCurrent |
                QItemSelectionModel::Rows);
}

void FormProductGroupCard::onShowListPictures()
{
    ui->stackedWidget->setCurrentWidget(mFrameListImages);
}

void FormProductGroupCard::on_pushButtonSimilarGroups_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->pageSimilarGroups);
    mSimilarsModel->resetModel();
    for (int i = 0; i < mSimilarsModel->columnCount() - 1; i++)
        ui->treeViewSimilarsGroups->resizeColumnToContents(i);
    if (mSimilarsModel->rowCount())
        ui->treeViewSimilarsGroups->selectionModel()->setCurrentIndex(mSimilarsModel->index(0, 0),
                                                                   QItemSelectionModel::SelectCurrent | QItemSelectionModel::Rows);
}

void FormProductGroupCard::on_toolButtonAddSimilar_clicked()
{
    FormProductsGroups form(this);

    form.setWindowFlags(Qt::Dialog);
    form.setSelectMode(true);
    if (form.exec() == QDialog::Accepted) {
        QListIterator<DataProductsGroup *> it(form.listSelected());
        while (it.hasNext()) {
            DataProductsGroup *gr = (it.next());
            mGroup->addSimilarGroup(gr);
        }
        mSimilarsModel->resetModel();
        for (int i = 0; i < mSimilarsModel->columnCount() - 1; i++)
            ui->treeViewSimilarsGroups->resizeColumnToContents(i);
        ui->treeViewSimilarsGroups->selectionModel()->setCurrentIndex(mSimilarsModel->index(mSimilarsModel->rowCount() - 1, 0),
                                                                   QItemSelectionModel::SelectCurrent | QItemSelectionModel::Rows);
    }
}

void FormProductGroupCard::on_toolButtonDelSimilar_clicked()
{
    QModelIndexList list =
            ui->treeViewSimilarsGroups->selectionModel()->selectedRows();
    if (!list.size())
        return;

    mSimilarsModel->beginReset();
    QStringList idsDelete;
    for (int i = 0; i < list.size(); i++)
        idsDelete.append(mGroup->similarGroups()->at(list.at(i).row())->getId());
    for (int i = mGroup->similarGroups()->size() - 1; i >= 0; i--)
        if (idsDelete.contains(mGroup->similarGroups()->at(i)->getId()))
            delete mGroup->similarGroups()->takeAt(i);
    mGroup->similarGroups()->setModified();
    mSimilarsModel->endReset();
    checkEnabled();
}

void FormProductGroupCard::on_pushButtonAdditionalSubgroups_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->pageAdditionalSubgroups);
    mSubgroupsModel->resetModel();
    for (int i = 0; i < mSubgroupsModel->columnCount() - 1; i++)
        ui->treeViewSubgroups->resizeColumnToContents(i);
    if (mSubgroupsModel->rowCount())
        ui->treeViewSubgroups->selectionModel()->setCurrentIndex(mSubgroupsModel->index(0, 0),
                                                                   QItemSelectionModel::SelectCurrent | QItemSelectionModel::Rows);
}

void FormProductGroupCard::on_toolButtonAddSubgroup_clicked()
{
    FormProductsGroups form(this);

    form.setWindowFlags(Qt::Dialog);
    form.setSelectMode(true);

    ListProductsGroups groups;
    if (!mGroup->getId().isEmpty()) {
        DataProductsGroup *group = new DataProductsGroup;
        group->setId(mGroup->getId());
        groups.append(group);
    }
    for (int i = 0; i < mGroup->additionalSubgroups()->size(); i++) {
        DataProductsGroup *group = new DataProductsGroup;
        group->setId(mGroup->additionalSubgroups()->at(i)->getId());
        groups.append(group);
    }
    form.setExludingGroups(&groups);

    if (form.exec() == QDialog::Accepted) {
        QListIterator<DataProductsGroup *> it(form.listSelected());
        while (it.hasNext()) {
            DataProductsGroup *gr = (it.next());
            mGroup->addAdditionalSubgroup(gr);
        }
        mSubgroupsModel->resetModel();
        for (int i = 0; i < mSubgroupsModel->columnCount() - 1; i++)
            ui->treeViewSubgroups->resizeColumnToContents(i);
        ui->treeViewSubgroups->selectionModel()->setCurrentIndex(mSubgroupsModel->index(mSubgroupsModel->rowCount() - 1, 0),
                                                                   QItemSelectionModel::SelectCurrent | QItemSelectionModel::Rows);
    }

}

void FormProductGroupCard::on_toolButtonDelSubgroup_clicked()
{
    QModelIndexList list =
            ui->treeViewSubgroups->selectionModel()->selectedRows();
    if (!list.size())
        return;

    mSubgroupsModel->beginReset();
    QStringList idsDelete;
    for (int i = 0; i < list.size(); i++)
        idsDelete.append(mGroup->additionalSubgroups()->at(list.at(i).row())->getId());
    for (int i = mGroup->additionalSubgroups()->size() - 1; i >= 0; i--)
        if (idsDelete.contains(mGroup->additionalSubgroups()->at(i)->getId()))
            delete mGroup->additionalSubgroups()->takeAt(i);
    mGroup->additionalSubgroups()->setModified();
    mSubgroupsModel->endReset();
    checkEnabled();
}

void FormProductGroupCard::changeSourcePrices(int index)
{
    ui->labelIncPrice->setVisible(!index);
    ui->labelIncPriceCorp->setVisible(!index);
    ui->labelIncPriceOpt->setVisible(!index);
    ui->spinBoxIncPrice->setVisible(!index);
    ui->spinBoxIncPriceCorp->setVisible(!index);
    ui->spinBoxIncPriceOpt->setVisible(!index);
}
