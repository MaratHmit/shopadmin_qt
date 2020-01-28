#include "framecardspecifications.h"
#include "ui_framecardspecifications.h"

#include <QMenu>

#include "apiadapter.h"
#include "seconfig.h"
#include "formproducts.h"

FrameCardSpecifications::FrameCardSpecifications(
        QWidget *parent, DataProduct *product,
        const bool &isDialogMode) :
    QDialog(parent), ui(new Ui::FrameCardSpecifications)
{
    ui->setupUi(this);
    mFormFeatures = 0;
    mVerticalSpacer = 0;
    mIsDialogMode = isDialogMode;
    if (isDialogMode) {
        ui->checkBoxIsShowFeatures->hide();
        ui->line_2->hide();
        setWindowFlags(Qt::Window);
        connect(ui->buttonOK, &QPushButton::clicked, this, &FrameCardSpecifications::onSave);
        connect(ui->buttonCancel, &QPushButton::clicked, this,
                &QDialog::reject);
        if (!product->getId().isEmpty())
            ui->comboBoxMethod->hide();
    }
    else {
        setWindowFlags(Qt::Widget);
        ui->buttonOK->hide();
        ui->buttonCancel->hide();
        ui->comboBoxMethod->hide();
    }
    mIsFilled = false;
    mProduct = product;
    mListSpecifications = mProduct->listSpecifications();

    initProductTypes();
    initActions();
    initSignals();    
}

FrameCardSpecifications::~FrameCardSpecifications()
{        
    delete ui;            
    delete mProductTypes;
}

bool FrameCardSpecifications::isAddSpecifications() const
{
    return ui->comboBoxMethod->currentIndex() == 0;
}

void FrameCardSpecifications::fillControls()
{
    if (!mListSpecifications || mIsFilled)
        return;

    setCursor(Qt::WaitCursor);
    int count = mListSpecifications->size();
    for (int i = 0; i < count; i++) {
        ui->actionDelSpecification->setEnabled(true);
        ui->widgetFeatures->addSpecification(mListSpecifications->at(i), false);
    }
    ui->widgetFeatures->resetModel();
    setCursor(Qt::ArrowCursor);
    ui->checkBoxIsShowFeatures->setChecked(mProduct->isShowFeatures());
    mIsFilled = true;
}

void FrameCardSpecifications::correctDataBeforeSave()
{
    if (!mListSpecifications->isModified())
        return;

    mListSpecifications->deleteItems();
    for (int i = 0; i < ui->widgetFeatures->listItems()->size(); ++i)
        mListSpecifications->append(new DataFeature(ui->widgetFeatures->listItems()->at(i)));
    mListSpecifications->setModified();
}

void FrameCardSpecifications::deleteGroupSpecification(GroupSpecifications *)
{
    mListSpecifications->deleteItems();
    ui->widgetFeatures->resetModel();
}

void FrameCardSpecifications::onAddSpecificationClicked()
{
    if (!mFormFeatures) {
        mFormFeatures = new FormFeatures(this);
        mFormFeatures->setSelectMode();
    }
    if (mFormFeatures->exec() == QDialog::Accepted) {
        ListFeatures *list = mFormFeatures->selectedList();
        for (int i = 0; i < list->size(); i++) {
            DataFeature *feature = new DataFeature(list->at(i));
            feature->setIdFeature(feature->getId());
            feature->setId(0);
            ui->widgetFeatures->addSpecification(feature, true);
        }
        onModified();
    }
}

void FrameCardSpecifications::showContextMenu(QPoint point)
{
    QPoint globalPos;

    if (sender()->inherits("QAbstractScrollArea"))
        globalPos = ((QAbstractScrollArea*)sender())->viewport()->mapToGlobal(point);
    else globalPos = ((QWidget*)sender())->mapToGlobal(point);

    QMenu menu;

    menu.addAction(ui->actionAddSpecification);
    menu.addAction(ui->actionDelSpecification);
    menu.exec(globalPos);
}

void FrameCardSpecifications::onCopyFromOtherProduct()
{
    FormProducts *form = new FormProducts(this, false);
    if (form->exec() == QDialog::Accepted && form->getSelectedProducts()->size()) {
        DataProduct *product = new DataProduct(form->getSelectedProducts()->at(0));
        setCursor(Qt::WaitCursor);
        if (ApiAdapter::getApi()->getInfoProduct(product)) {
            for (int i = 0; i < product->listSpecifications()->size(); ++i)
                mProduct->listSpecifications()->append(new DataFeature(product->listSpecifications()->at(i)), false);
            mIsFilled = false;
            fillControls();
            emit modified();
        }
        delete product;
        setCursor(Qt::ArrowCursor);
    }
    delete form;
}

void FrameCardSpecifications::selectTypeProduct()
{
    if (ui->comboBoxProductTypes->currentData().toString().isEmpty())
        return;

    DataItem item;
    item.setId(ui->comboBoxProductTypes->currentData().toString());
    mProduct->setProperty("idType", ui->comboBoxProductTypes->currentData().toString());
    ui->comboBoxProductTypes->setEnabled(false);
    setCursor(Qt::WaitCursor);
    if (ApiAdapter::getApi()->getInfoData(SEConsts::API_PRODUCTS_TYPES, &item)) {
        ui->widgetFeatures->clear();

        if (!item.property("featuresList").isNull()) {
            QList<QVariant> list = item.property("featuresList").toList();
            for (int i = 0; i < list.size(); ++i) {
                DataFeature feature;
                QMap<QString, QVariant> map = list.at(i).toMap();
                feature.setIdFeature(map["id"].toString());
                feature.setName(map["name"].toString());
                feature.setValueType(map["valueType"].toString());
                feature.setSortIndex(map["sortIndex"].toInt());
                feature.setMeasure(map["measure"].toString());
                ui->widgetFeatures->addSpecification(&feature, false);
            }
            ui->widgetFeatures->resetModel();
            onModified();
        }
    }
    ui->comboBoxProductTypes->setEnabled(true);
    setCursor(Qt::ArrowCursor);
}

void FrameCardSpecifications::onDelSpecificationClicked()
{
    ui->widgetFeatures->deleteSpecification();
}

bool FrameCardSpecifications::existSpecification(DataFeature *feature) const
{
    for (int i = 0; i < mListSpecifications->size(); i++) {
        if (mListSpecifications->at(i)->idFeature() == feature->getId())
            return true;
    }
    return false;
}

void FrameCardSpecifications::initActions()
{
    ui->toolButtonAddSpecification->setDefaultAction(ui->actionAddSpecification);
    ui->toolButtonDelSpecification->setDefaultAction(ui->actionDelSpecification);
    ui->actionDelSpecification->setEnabled(false);
}

void FrameCardSpecifications::initSignals()
{
    connect(ui->actionAddSpecification, SIGNAL(triggered()), SLOT(onAddSpecificationClicked()));
    connect(ui->actionDelSpecification, SIGNAL(triggered()), SLOT(onDelSpecificationClicked()));    
    connect(ui->toolButtonCopy, SIGNAL(clicked(bool)), SLOT(onCopyFromOtherProduct()));
    connect(ui->comboBoxProductTypes, SIGNAL(currentIndexChanged(int)), SLOT(selectTypeProduct()));
    connect(ui->widgetFeatures, SIGNAL(modified()), SLOT(onModified()));
    connect(ui->checkBoxIsShowFeatures, SIGNAL(clicked(bool)), mProduct, SLOT(setIsShowFeatures(bool)));
}

void FrameCardSpecifications::initProductTypes()
{
    mProductTypes = new ListDataItems;
    ui->comboBoxProductTypes->setEnabled(false);
    setCursor(Qt::WaitCursor);
    if (ApiAdapter::getApi()->getListData(SEConsts::API_PRODUCTS_TYPES, mProductTypes, Request())) {
        int index = -1;
        for (int i = 0; i < mProductTypes->size(); ++i) {
            ui->comboBoxProductTypes->addItem(mProductTypes->at(i)->getName(), mProductTypes->at(i)->getId());
            if (mProduct->property("idType").toString() == mProductTypes->at(i)->getId())
                index = i;
        }
        ui->comboBoxProductTypes->setCurrentIndex(index);
    }
    ui->comboBoxProductTypes->setEnabled(true);
    setCursor(Qt::ArrowCursor);
}

void FrameCardSpecifications::onSave()
{
    correctDataBeforeSave();    
    accept();
}

void FrameCardSpecifications::onModified()
{
    mListSpecifications->setModified();
    emit modified();
}
