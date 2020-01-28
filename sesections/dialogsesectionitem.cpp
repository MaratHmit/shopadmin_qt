#include "dialogsesectionitem.h"
#include "ui_dialogsesectionitem.h"

#include "apiadapter.h"

#include <QDebug>

DialogSESectionItem::DialogSESectionItem(QWidget *parent, const DataItem *item) :
    QDialog(parent),
    ui(new Ui::DialogSESectionItem)
{
    ui->setupUi(this);    
    mSectionItem = new DataItem(item);    
    ui->tabWidget->setCurrentIndex(0);

    setWindowFlags(Qt::Window);
    initVariables();    
}

const DataItem *DialogSESectionItem::sectionItem() const
{
    return mSectionItem;
}

DialogSESectionItem::~DialogSESectionItem()
{
    delete ui;
    delete mSectionItem;
}

void DialogSESectionItem::onChangeType()
{
    ui->widgetBindItem->setVisible(ui->comboBoxTypes->currentIndex() < 4);
    ui->widgetURL->setVisible(!ui->widgetBindItem->isVisible());
    if (!ui->widgetBindItem->isVisible()) {
        mSectionItem->setIdValue(QString());
        mSectionItem->setValue(QString());
        ui->lineEditBindItem->clear();
    }
}

void DialogSESectionItem::onSelectItem()
{
    // группа товара
    if (ui->comboBoxTypes->currentIndex() == 0) {
        FormProductsGroups form(this);
        form.setSelectMode(false);
        if (form.exec() == QDialog::Accepted) {
            if (form.listSelected().size()) {
                mSectionItem->setIdValue(form.listSelected().at(0)->getId());
                mSectionItem->setValue("productGroup");
                setImageFromItem(form.listSelected().at(0));
            }
        }
    }
    // товар
    if (ui->comboBoxTypes->currentIndex() == 1) {
        FormProducts form(this, false);
        if (form.exec() == QDialog::Accepted) {
            if (form.getSelectedProducts()->size()) {
                mSectionItem->setIdValue(form.getSelectedProducts()->at(0)->getId());
                mSectionItem->setValue("product");
                DataProduct product(form.getSelectedProducts()->at(0));
                ApiAdapter::getApi()->getInfoProduct(&product);
                setImageFromItem(&product);
            }
        }
    }
    // бренд
    if (ui->comboBoxTypes->currentIndex() == 2) {
        FormBrands form(this);
        form.setSelectMode(false);
        if (form.exec() == QDialog::Accepted) {
            if (form.selectedBrands()->size()) {
                mSectionItem->setIdValue(form.selectedBrands()->at(0)->getId());
                mSectionItem->setValue("brand");
                setImageFromItem(form.selectedBrands()->at(0));
            }
        }
    }
    // новость
    if (ui->comboBoxTypes->currentIndex() == 3) {
        FormPublications form(this);
        form.setSelectMode(false);
        if (form.exec() == QDialog::Accepted) {
            if (form.selectedNews()->size()) {
                mSectionItem->setIdValue(form.selectedNews()->at(0)->getId());
                mSectionItem->setValue("publication");
                setImageFromItem(form.selectedNews()->at(0));
            }
        }
    }
    onChangedData();
}

void DialogSESectionItem::onChangedData()
{
    if (ui->comboBoxTypes->currentIndex() < 4)
        ui->pushButtonOK->setEnabled(!mSectionItem->getIdValue().isEmpty());
    else ui->pushButtonOK->setEnabled(!ui->lineEditName->text().isEmpty());
}

void DialogSESectionItem::onChangedImage()
{
    isModifiedImage = true;
    onChangedData();
}

void DialogSESectionItem::onChangedTab(int index)
{
    if (index == 2 && !isShowedNote) {
        ui->widgetNote->setHTMLText(mSectionItem->property("note").toString());
        isShowedNote = true;
    }
}

void DialogSESectionItem::accept()
{
    fillData();
    if (save())
        QDialog::accept();
}

void DialogSESectionItem::showEvent(QShowEvent *)
{
    startTimer(10);
}

void DialogSESectionItem::timerEvent(QTimerEvent *event)
{
    killTimer(event->timerId());
    fillControls();
    initSignals();
}

void DialogSESectionItem::fillControls()
{
    if (mSectionItem->getId().isEmpty())
        return;

    if (!mSectionItem->getIdValue().isEmpty()) {        
        if (mSectionItem->getValue() == "productGroup") {
            DataProductsGroup group;
            group.setId(mSectionItem->getIdValue());
            ApiAdapter::getApi()->getInfoProductGroup(&group);
            ui->lineEditBindItem->setText(group.getName());
            if (mSectionItem->getImageFile().isEmpty())
                setImageFromItem(group);
        }
        if (mSectionItem->getValue() == "product") {            
            DataProduct product;
            product.setId(mSectionItem->getIdValue());
            ApiAdapter::getApi()->getInfoProduct(&product);
            ui->lineEditBindItem->setText(product.getName());
            if (mSectionItem->getImageFile().isEmpty())
                setImageFromItem(product);
            ui->comboBoxTypes->setCurrentIndex(1);
        }
        if (mSectionItem->getValue() == "brand") {            
            DataItem brand;            
            brand.setId(mSectionItem->getIdValue());            
            ApiAdapter::getApi()->getInfoBrand(&brand);
            ui->lineEditBindItem->setText(brand.getName());
            if (mSectionItem->getImageFile().isEmpty())
                setImageFromItem(brand);            
            ui->comboBoxTypes->setCurrentIndex(2);            
        }
        if (mSectionItem->getValue() == "publication") {            
            DataNew publication;
            publication.setId(mSectionItem->getIdValue());
            ApiAdapter::getApi()->getInfoNews(&publication);
            ui->lineEditBindItem->setText(publication.getName());
            if (mSectionItem->getImageFile().isEmpty())
                setImageFromItem(publication);
            ui->comboBoxTypes->setCurrentIndex(3);
        }
    }
    if (!mSectionItem->getImageFile().isEmpty())
        ui->widgetImage->setImage(mSectionItem->getImageFile());    
    if (mSectionItem->getValue().isEmpty()) {
        ui->comboBoxTypes->setCurrentIndex(4);
    }
    ui->lineEditName->setText(mSectionItem->getName());
    ui->lineEditURL->setText(mSectionItem->getUrl());        
    ui->widgetImage->setImageAlt(mSectionItem->getImageAlt());
    ui->checkBoxIsActive->setChecked(mSectionItem->getIsActive());
}

void DialogSESectionItem::fillData()
{
    ui->widgetNote->refreshSourceText();
    mSectionItem->setName(ui->lineEditName->text().trimmed());    
    mSectionItem->setNote(ui->widgetNote->textHTML());
    mSectionItem->setIsActive(ui->checkBoxIsActive->isChecked());
    mSectionItem->setImageAlt(ui->widgetImage->altImage());
    if (isModifiedImage)
        mSectionItem->setImageFile(ui->widgetImage->fileImageName());
    if (mSectionItem->getIdValue().isEmpty())
        mSectionItem->setUrl(ui->lineEditURL->text().trimmed());
    else mSectionItem->setUrl(QString());
}

void DialogSESectionItem::initVariables()
{
    ui->widgetURL->hide();
    ui->widgetImage->setSection(SEConsts::Section_ShopSections);
    ui->widgetNote->setSection(SEConsts::Section_ShopSections);
    isModifiedImage = false;
    isShowedNote = false;
}

void DialogSESectionItem::initSignals()
{
    connect(ui->pushButtonCancel, SIGNAL(clicked(bool)), SLOT(reject()));
    connect(ui->pushButtonOK, SIGNAL(clicked(bool)), SLOT(accept()));
    connect(ui->comboBoxTypes, SIGNAL(currentIndexChanged(int)), SLOT(onChangeType()));
    connect(ui->toolButtonSelectItem, SIGNAL(clicked(bool)), SLOT(onSelectItem()));
    connect(ui->lineEditName, SIGNAL(textChanged(QString)), SLOT(onChangedData()));
    connect(ui->widgetImage, SIGNAL(modified()), SLOT(onChangedImage()));    
    connect(ui->lineEditURL, SIGNAL(textEdited(QString)), SLOT(onChangedData()));
    connect(ui->widgetNote, SIGNAL(modified()), SLOT(onChangedData()));
    connect(ui->tabWidget, SIGNAL(currentChanged(int)), SLOT(onChangedTab(int)));
}

bool DialogSESectionItem::save()
{
    if (isModifiedImage) {
        ui->widgetImage->setSection(SEConsts::Section_ShopSections);
        ui->widgetImage->saveImage();
    }
    setCursor(Qt::WaitCursor);
    bool result = ApiAdapter::getApi()->saveSESectionItem(mSectionItem);
    setCursor(Qt::ArrowCursor);
    return result;
}

void DialogSESectionItem::setImageFromItem(const DataItem &item)
{
    ui->widgetImage->setSection(SEConsts::Section_ShopSections);
    if (mSectionItem->getValue() == "productGroup")
        ui->widgetImage->setSection(SEConsts::Section_GroupsProducts);
    if (mSectionItem->getValue() == "product")
        ui->widgetImage->setSection(SEConsts::Section_Products);
    if (mSectionItem->getValue() == "brand")
        ui->widgetImage->setSection(SEConsts::Section_Brands);
    if (mSectionItem->getValue() == "publication")
        ui->widgetImage->setSection(SEConsts::Section_News);
    ui->lineEditName->setText(item.getName());
    ui->lineEditBindItem->setText(item.getName());
    ui->widgetImage->setImageAlt(item.getImageAlt());
    ui->widgetImage->setImage(item.getImageFile());     
}
