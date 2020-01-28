
#include "framecardmaininfo.h"
#include "ui_framecardmaininfo.h"

#include "apiadapter.h"
#include "seconfig.h"
#include "formbrands.h"
#include "formproductsgroups.h"
#include "dialogyacategories.h"


#include <QTextEdit>
#include <QDateEdit>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QHBoxLayout>

FrameCardMainInfo::FrameCardMainInfo(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FrameCardMainInfo)
{
    ui->setupUi(this);
    mIsShowed = false;
    mIsFilled = false;    
    mProduct = 0;
    mIsMultiMode = 0;    
    mListCurrencies = new ListCurrencies();
    QPalette pal;
    SEConfig::setPaletteReadOnly(&pal);
    ui->lineEditGroup->setPalette(pal);
    ui->widgetDescription->setSection(SEConsts::Section_Products);

    QPalette palBackground(palette());
    palBackground.setColor(QPalette::Background, QColor(240, 240, 240, 255));

    for (int i = 0; i < ui->tabWidget->count(); i++) {
        ui->tabWidget->widget(i)->setAutoFillBackground(true);
        ui->tabWidget->widget(i)->setPalette(palette());
    }
    ui->tabWidget->setCurrentIndex(0);
    initSignals();
}

FrameCardMainInfo::~FrameCardMainInfo()
{
    delete ui;
    delete mListCurrencies;
}

void FrameCardMainInfo::setProduct(DataProduct *product)
{
    mProduct = product;
}

void FrameCardMainInfo::showEvent(QShowEvent *)
{
    if (!mIsShowed) {        
        setValidators();        
        mIsShowed = true;
    }
    disconnectSignalsChanged();
    initCurrencies();
    initMeasures();
    initLabels();
    ui->lineEditName->setFocus();
}

void FrameCardMainInfo::setMultiMode(const bool &isMultiMode)
{
    mIsMultiMode = isMultiMode;
    ui->lineEditName->setHidden(isMultiMode);
    ui->lineEditCode->setHidden(isMultiMode);
    ui->lineEditArticle->setHidden(isMultiMode);
    ui->labelName->setHidden(isMultiMode);
    ui->labelCode->setHidden(isMultiMode);
    ui->labelArticul->setHidden(isMultiMode);
    if (mIsMultiMode)
        ui->toolButtonTranslite->hide();
    if (mIsMultiMode)
        ui->comboBoxCurrencies->setCurrentIndex(-1);
}

void FrameCardMainInfo::onSetInfinitely(const bool &value)
{
    mProduct->setIsInfinitely(value);
    double count = mProduct->count();
    if (count < 0)
        count = 0;
    ui->lineEditCount->setEnabled(!value);    
    if (value)
        ui->lineEditCount->clear();
    else {
        ui->lineEditCount->setText(QString::number(count, 'f', 2));
        mProduct->setCount(count);
    }
}

void FrameCardMainInfo::onSelectBrand()
{
    FormBrands *form = new FormBrands(this);
    form->setSelectMode(false);
    if (form->exec() == QDialog::Accepted) {
        if (form->selectedBrands()->size()) {
            ui->lineEditBrand->setText(form->selectedBrands()->at(0)->getName());
            mProduct->brand()->setId(form->selectedBrands()->at(0)->getId());
            mProduct->brand()->setName(form->selectedBrands()->at(0)->getName());
            emit modified();
        }
    }
}

void FrameCardMainInfo::onDeleteBrand()
{
    ui->lineEditBrand->clear();
    mProduct->brand()->clearData();
    mProduct->setModified("brand");
    emit modified();
}

void FrameCardMainInfo::onSelectYAMarketCategories()
{
    DialogYACategories dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        ui->lineEditYandexMarketCategories->setText(dialog.selectedItem()->getCode());
        mProduct->setProperty("idYAMarketCategory", dialog.selectedItem()->getId());
    }
}

void FrameCardMainInfo::onDeleteYAMarketCategories()
{
    mProduct->setProperty("idYAMarketCategory", QString());
    ui->lineEditYandexMarketCategories->clear();
}

void FrameCardMainInfo::transliteCode()
{
    if (!ui->lineEditName->text().isEmpty()) {
        QStringList list;
        list.append(ui->lineEditName->text().trimmed());
        list = ApiAdapter::getApi()->translit(list);
        if (list.count()) {
            ui->lineEditCode->setText(list.at(0).toLower());
            mProduct->setCode(list.at(0).toLower());
        }
    }
}

void FrameCardMainInfo::setValidators()
{
    QRegExp regExprFloat("\\d*\\.{1}\\d*$|^\\d*$");
    QRegExpValidator *validCurrency = new QRegExpValidator(regExprFloat, this);
    ui->lineEditPrice->setValidator(validCurrency);
    ui->lineEditPricePurchase->setValidator(validCurrency);
    ui->lineEditPriceWholesale->setValidator(validCurrency);
    ui->lineEditPriceMiniWholesale->setValidator(validCurrency);
    ui->lineEditRate->setValidator(validCurrency);
    ui->lineEditTax->setValidator(validCurrency);
    ui->lineEditCount->setValidator(validCurrency);
    ui->lineEditMinCount->setValidator(validCurrency);
}

void FrameCardMainInfo::initCurrencies()
{
    if (ui->comboBoxCurrencies->count())
        return;

    mListCurrencies = new ListCurrencies();
    if (ApiAdapter::getApi()->getListCurrencies(mListCurrencies)) {
        ui->comboBoxCurrencies->clear();
        for (int i = 0; i < mListCurrencies->size(); i++) {
            ui->comboBoxCurrencies->addItem(mListCurrencies->at(i)->getName(),
                                            mListCurrencies->at(i)->getCode());
            if (mListCurrencies->at(i)->getCode() == SEConfig::getMainInfo()->baseCurrency())
                ui->comboBoxCurrencies->setCurrentIndex(i);
        }        
    }
}

void FrameCardMainInfo::initMeasures()
{
    if (SEConfig::isExtHosting) {
        ui->comboBoxVolumeEdit->hide();
        ui->comboBoxVolumeView->hide();
        ui->comboBoxWeightEdit->hide();
        ui->comboBoxWeightView->hide();
        ui->labelVolumeEdit->hide();
        ui->labelVolumeView->hide();
        ui->labelWeightEdit->hide();
        ui->labelWeightView->hide();
    }

    if (ui->comboBoxWeightEdit->count() || SEConfig::isExtHosting)
        return;

    mListMeasuresWeight = new ListDataItems();
    if (ApiAdapter::getApi()->getListData(SEConsts::API_MEASURES_WEIGHT, mListMeasuresWeight)) {
        ui->comboBoxWeightView->clear();
        ui->comboBoxWeightEdit->clear();
        for (int i = 0; i < mListMeasuresWeight->size(); i++) {
            ui->comboBoxWeightView->addItem(mListMeasuresWeight->at(i)->getName(),
                                            mListMeasuresWeight->at(i)->getId());
            if (mListMeasuresWeight->at(i)->property("isView").toBool())
                ui->comboBoxWeightView->setCurrentIndex(i);
            ui->comboBoxWeightEdit->addItem(mListMeasuresWeight->at(i)->getName(),
                                            mListMeasuresWeight->at(i)->getId());
            if (mListMeasuresWeight->at(i)->property("isEdit").toBool()) {
                ui->comboBoxWeightEdit->setCurrentIndex(i);
                onChangeMeasureWeght();
            }
        }
    }

    mListMeasuresVolume = new ListDataItems();
    if (ApiAdapter::getApi()->getListData(SEConsts::API_MEASURES_VOLUME, mListMeasuresVolume)) {
        ui->comboBoxVolumeView->clear();
        ui->comboBoxVolumeEdit->clear();
        for (int i = 0; i < mListMeasuresVolume->size(); i++) {
            ui->comboBoxVolumeView->addItem(mListMeasuresVolume->at(i)->getName(),
                                            mListMeasuresVolume->at(i)->getId());
            if (mListMeasuresVolume->at(i)->property("isView").toBool())
                ui->comboBoxVolumeView->setCurrentIndex(i);
            ui->comboBoxVolumeEdit->addItem(mListMeasuresVolume->at(i)->getName(),
                                            mListMeasuresVolume->at(i)->getId());
            if (mListMeasuresVolume->at(i)->property("isEdit").toBool()) {
                ui->comboBoxVolumeView->setCurrentIndex(i);
                onChangeMeasureVolume();
            }
        }
    }
}

void FrameCardMainInfo::initLabels()
{
    if (mProduct->getId().isEmpty()) {
        ListDataItems mListLabels;
        if (ApiAdapter::getApi()->getListData(SEConsts::API_PRODUCTS_LABELS, &mListLabels)) {
            QHBoxLayout*layout = new QHBoxLayout;
            for (int i = 0; i < mListLabels.size(); i++) {
                QCheckBox *check = new QCheckBox(ui->widgetLabels);
                layout->addWidget(check);
                check->setText(mListLabels.at(i)->getName());
                check->setProperty("idLabel", mListLabels.at(i)->getId());
                connect(check, &QCheckBox::clicked, this, &FrameCardMainInfo::onChangeData);
                DataItem *item = new DataItem;
                item->setId(mListLabels.at(i)->getId());
                mProduct->listLabels()->append(item);
            }
            ui->widgetLabels->setLayout(layout);
        }
    } else {
        QHBoxLayout*layout = new QHBoxLayout;
        for (int i = 0; i < mProduct->listLabels()->size(); i++) {
            QCheckBox *check = new QCheckBox(ui->widgetLabels);
            layout->addWidget(check);
            check->setText(mProduct->listLabels()->at(i)->getName());
            check->setProperty("idLabel", mProduct->listLabels()->at(i)->getId());
            check->setChecked(mProduct->listLabels()->at(i)->getIsChecked());
            connect(check, &QCheckBox::clicked, this, &FrameCardMainInfo::onChangeData);
        }
        ui->widgetLabels->setLayout(layout);
    }
}

void FrameCardMainInfo::connectSignalsChanged()
{
    connect(ui->lineEditPrice, &QLineEdit::textEdited, this, &FrameCardMainInfo::onChangeData);
    connect(ui->lineEditPricePurchase, &QLineEdit::textEdited, this, &FrameCardMainInfo::onChangeData);
    connect(ui->lineEditPriceWholesale, &QLineEdit::textEdited, this, &FrameCardMainInfo::onChangeData);
    connect(ui->lineEditPriceMiniWholesale, &QLineEdit::textEdited, this, &FrameCardMainInfo::onChangeData);
    connect(ui->lineEditRate, &QLineEdit::textEdited, this, &FrameCardMainInfo::onChangeData);
    connect(ui->lineEditTax, &QLineEdit::textEdited, this, &FrameCardMainInfo::onChangeData);
    connect(ui->lineEditName, &QLineEdit::textEdited, this, &FrameCardMainInfo::onChangeData);
    connect(ui->lineEditGroup, &QLineEdit::textEdited, this, &FrameCardMainInfo::onChangeData);
    connect(ui->lineEditArticle, &QLineEdit::textEdited, this, &FrameCardMainInfo::onChangeData);
    connect(ui->lineEditCode, &QLineEdit::textEdited, this, &FrameCardMainInfo::onChangeData);
    connect(ui->lineEditPresence, &QLineEdit::textEdited, this, &FrameCardMainInfo::onChangeData);
    connect(ui->lineEditStepCount, &QLineEdit::textEdited, this, &FrameCardMainInfo::onChangeData);
    connect(ui->lineEditMeasure, &QLineEdit::textEdited, this, &FrameCardMainInfo::onChangeData);
    connect(ui->comboBoxCurrencies, &QComboBox::currentTextChanged, this, &FrameCardMainInfo::onChangeData);
    connect(ui->checkBoxInfinity, &QCheckBox::clicked, this, &FrameCardMainInfo::onChangeData);
    connect(ui->checkBoxIsActive, &QCheckBox::clicked, this, &FrameCardMainInfo::onChangeData);
    connect(ui->lineEditCount, &QLineEdit::textEdited, this, &FrameCardMainInfo::onChangeData);
    connect(ui->lineEditMinCount, &QLineEdit::textEdited, this, &FrameCardMainInfo::onChangeData);
    connect(ui->checkBoxActIsHit, &QCheckBox::clicked, this, &FrameCardMainInfo::onChangeData);
    connect(ui->checkBoxActIsNew, &QCheckBox::clicked, this, &FrameCardMainInfo::onChangeData);
    connect(ui->checkBoxActIsSpec, &QCheckBox::clicked, this, &FrameCardMainInfo::onChangeData);
    connect(ui->checkBoxIsYandexMarket, &QCheckBox::clicked, this, &FrameCardMainInfo::onChangeData);
    connect(ui->checkBoxIsMarketAvailable, &QCheckBox::clicked, this, &FrameCardMainInfo::onChangeData);
    connect(ui->doubleSpinBoxVolume, SIGNAL(valueChanged(double)), SLOT(onChangeData()));
    connect(ui->doubleSpinBoxWeight, SIGNAL(valueChanged(double)), SLOT(onChangeData()));
    connect(ui->widgetDescription, SIGNAL(modified()), SLOT(onChangeData()));
    connect(ui->spinBoxSortIndex, SIGNAL(valueChanged(int)), SLOT(onChangeData()));
    connect(ui->checkBoxInfinity, SIGNAL(clicked(bool)), SLOT(onSetInfinitely(bool)));    
    connect(ui->comboBoxWeightEdit, &QComboBox::currentTextChanged, this, &FrameCardMainInfo::onChangeData);
    connect(ui->comboBoxWeightView, &QComboBox::currentTextChanged, this, &FrameCardMainInfo::onChangeData);
    connect(ui->comboBoxVolumeEdit, &QComboBox::currentTextChanged, this, &FrameCardMainInfo::onChangeData);
    connect(ui->comboBoxVolumeView, &QComboBox::currentTextChanged, this, &FrameCardMainInfo::onChangeData);
}

void FrameCardMainInfo::disconnectSignalsChanged()
{
    ui->lineEditPrice->disconnect();
    ui->lineEditPricePurchase->disconnect();
    ui->lineEditPriceWholesale->disconnect();
    ui->lineEditPriceMiniWholesale->disconnect();
    ui->lineEditRate->disconnect();
    ui->lineEditTax->disconnect();
    ui->lineEditName->disconnect();
    ui->lineEditGroup->disconnect();
    ui->lineEditArticle->disconnect();
    ui->lineEditCode->disconnect();
    ui->lineEditPresence->disconnect();
    ui->lineEditStepCount->disconnect();
    ui->lineEditMinCount->disconnect();
    ui->lineEditMeasure->disconnect();
    ui->comboBoxCurrencies->disconnect();    
    ui->checkBoxIsActive->disconnect();    
    ui->checkBoxActIsNew->disconnect();
    ui->checkBoxActIsHit->disconnect();
    ui->doubleSpinBoxWeight->disconnect();
    ui->doubleSpinBoxVolume->disconnect();
    ui->widgetDescription->disconnect();
    ui->spinBoxSortIndex->disconnect();
    ui->checkBoxInfinity->disconnect();
    ui->checkBoxIsYandexMarket->disconnect();
    ui->checkBoxIsMarketAvailable->disconnect();
    ui->comboBoxVolumeEdit->disconnect();
    ui->comboBoxVolumeView->disconnect();
    ui->comboBoxWeightEdit->disconnect();
    ui->comboBoxWeightView->disconnect();
}

void FrameCardMainInfo::onChangeData()
{
    if (sender() == ui->lineEditPrice)
        mProduct->setPrice(ui->lineEditPrice->text().toFloat());
    if (sender() == ui->lineEditPricePurchase)
        mProduct->setPricePurchase(ui->lineEditPricePurchase->text().toFloat());
    if (sender() == ui->lineEditPriceWholesale)
        mProduct->setPriceWholesale(ui->lineEditPriceWholesale->text().toFloat());
    if (sender() == ui->lineEditPriceMiniWholesale)
        mProduct->setPriceMiniWholesale(ui->lineEditPriceMiniWholesale->text().toFloat());
    if (sender() == ui->lineEditRate)
        mProduct->setRate(ui->lineEditRate->text().toFloat());
    if (sender() == ui->lineEditTax)
        mProduct->setTax(ui->lineEditTax->text().toFloat());
    if (sender() == ui->lineEditCount) {
        mProduct->setIsInfinitely(ui->checkBoxInfinity->isChecked());
        if (!mProduct->isInfinitely())
            mProduct->setCount(ui->lineEditCount->text().toFloat());     
    }
    if (sender() == ui->lineEditStepCount)
        mProduct->setStepCount(ui->lineEditStepCount->text().toFloat());
    if (sender() == ui->lineEditMinCount)
        mProduct->setMinCount(ui->lineEditMinCount->text().toFloat());
    if (sender() == ui->lineEditGroup)
        mProduct->setNameGroup(ui->lineEditGroup->text().trimmed());
    if (sender() == ui->lineEditName)
        mProduct->setName(ui->lineEditName->text().trimmed());
    if (sender() == ui->lineEditArticle)
        mProduct->setArticle(ui->lineEditArticle->text().trimmed());
    if (sender() == ui->lineEditCode)
        mProduct->setCode(ui->lineEditCode->text().trimmed());
    if (sender() == ui->lineEditStepCount)
        mProduct->setStepCount(ui->lineEditStepCount->text().toFloat());
    if (sender() == ui->lineEditMeasure)
        mProduct->setMeasurement(ui->lineEditMeasure->text().trimmed());
    if (sender() == ui->lineEditPresence)
        mProduct->setPrecense(ui->lineEditPresence->text());    
    if (sender() == ui->comboBoxCurrencies)
        mProduct->setCurrency(ui->comboBoxCurrencies->currentData().toString());
    if (sender() == ui->checkBoxInfinity)
        mProduct->setIsInfinitely(ui->checkBoxInfinity->isChecked());
    if (sender() == ui->checkBoxIsActive)
        mProduct->setIsActive(ui->checkBoxIsActive->isChecked());
    if (sender() == ui->doubleSpinBoxVolume)
        mProduct->setVolume(ui->doubleSpinBoxVolume->value());
    if (sender() == ui->doubleSpinBoxWeight)
        mProduct->setWeight(ui->doubleSpinBoxWeight->value());    
    if (sender() == ui->checkBoxActIsHit)
        mProduct->setIsHit(ui->checkBoxActIsHit->isChecked());
    if (sender() == ui->checkBoxActIsNew)
        mProduct->setIsNew(ui->checkBoxActIsNew->isChecked());
    if (sender() == ui->checkBoxActIsSpec)
        mProduct->setIsSpec(ui->checkBoxActIsSpec->isChecked());
    if (sender() == ui->spinBoxSortIndex)
        mProduct->setSortIndex(ui->spinBoxSortIndex->value());
    if (sender() == ui->checkBoxIsYandexMarket)
        mProduct->setIsYAMarket(ui->checkBoxIsYandexMarket->isChecked());
    if (sender() == ui->checkBoxIsMarketAvailable)
        mProduct->setIsYAMarketAvailable(ui->checkBoxIsMarketAvailable->isChecked());
    if (sender() == ui->comboBoxWeightView) {
        int id = ui->comboBoxWeightView->currentData().toInt();
        if (id > 0)
            mProduct->setProperty("idWeightView", id);
    }
    if (sender() == ui->comboBoxWeightEdit) {
        int id = ui->comboBoxWeightEdit->currentData().toInt();
        if (id > 0) {
            mProduct->setProperty("idWeightEdit", id);
            onChangeMeasureWeght();
        }
    }
    if (sender() == ui->comboBoxVolumeView) {
        int id = ui->comboBoxVolumeView->currentData().toInt();
        if (id > 0)
            mProduct->setProperty("idVolumeView", id);
    }
    if (sender() == ui->comboBoxVolumeEdit) {
        int id = ui->comboBoxVolumeEdit->currentData().toInt();
        if (id > 0) {
            mProduct->setProperty("idVolumeEdit", id);
            onChangeMeasureVolume();
        }
    }
    if ((!sender()->property("idLabel").isNull())) {
        QCheckBox *check = reinterpret_cast<QCheckBox *>(sender());
        QString idLabel = check->property("idLabel").toString();
        for (int i = 0; i < mProduct->listLabels()->size(); i++) {
            if (idLabel == mProduct->listLabels()->at(i)->getId()) {
                mProduct->listLabels()->at(i)->setIsChecked(check->isChecked());
                break;
            }
        }
    }
    emit modified();
}

void FrameCardMainInfo::onChangeMeasureWeght()
{
    int id = ui->comboBoxWeightEdit->currentData().toInt();
    if (id > 0) {
        for (int i = 0; i < mListMeasuresWeight->size(); ++i)
            if (id == mListMeasuresWeight->at(i)->getId().toInt())
                ui->doubleSpinBoxWeight->setSuffix(" " + mListMeasuresWeight->at(i)->property("designation").toString());
    }
}

void FrameCardMainInfo::onChangeMeasureVolume()
{
    int id = ui->comboBoxVolumeEdit->currentData().toInt();
    if (id > 0) {
        for (int i = 0; i < mListMeasuresVolume->size(); ++i)
            if (id == mListMeasuresVolume->at(i)->getId().toInt())
                ui->doubleSpinBoxVolume->setSuffix(" " + mListMeasuresVolume->at(i)->property("designation").toString());
    }
}

void FrameCardMainInfo::initSignals()
{
    connect(ui->toolButtonTranslite, &QAbstractButton::clicked, this, &FrameCardMainInfo::transliteCode);
    connect(ui->toolButtonSelectBrand, SIGNAL(clicked()), SLOT(onSelectBrand()));
    connect(ui->toolButtonClearParentBrand, SIGNAL(clicked()), SLOT(onDeleteBrand()));
    connect(ui->toolButtonSelectYandexMarketCategory, SIGNAL(clicked(bool)), SLOT(onSelectYAMarketCategories()));
    connect(ui->toolButtonClearSelectYandexMarketCategory, SIGNAL(clicked(bool)), SLOT(onDeleteYAMarketCategories()));    
    connect(ui->toolButtonParentGroup, SIGNAL(clicked()), SLOT(onSelectGroup()));
    connect(ui->toolButtonClearParentGroup, SIGNAL(clicked()), SLOT(onDeleteGroup()));
}

void FrameCardMainInfo::fillControls()
{
    if (ui->lineEditGroup->text().isEmpty()) {
        if (mProduct->getPathGroup().isEmpty())
            ui->lineEditGroup->setText(mProduct->getNameGroup());
        else ui->lineEditGroup->setText(mProduct->getPathGroup());
    }
    if (!mProduct->getId().isEmpty() && mProduct->getIds().size() == 1 && !mIsFilled) {
        ui->checkBoxIsActive->setChecked(mProduct->getIsActive());
        ui->lineEditName->setText(mProduct->getName());
        ui->lineEditArticle->setText(mProduct->getArticle());
        ui->lineEditCode->setText(mProduct->getCode());
        ui->lineEditPrice->setText(QString::number(mProduct->price(), 'f', 2));
        ui->lineEditPricePurchase->setText(QString::number(mProduct->pricePurchase(), 'f', 2));
        ui->lineEditPriceWholesale->setText(QString::number(mProduct->priceWholesale(), 'f', 2));
        ui->lineEditPriceMiniWholesale->setText(QString::number(mProduct->priceMiniWholesale(), 'f', 2));
        ui->lineEditRate->setText(QString::number(mProduct->rate(), 'f', 2));
        ui->lineEditTax->setText(QString::number(mProduct->tax(), 'f', 2));
        ui->lineEditMeasure->setText(mProduct->measurement());
        ui->checkBoxIsYandexMarket->setChecked(mProduct->isYAMarket());
        ui->checkBoxIsMarketAvailable->setChecked(mProduct->isYAMarketAvailable());

        for (int i = 0; i < ui->comboBoxCurrencies->count(); i++) {
            QString code = ui->comboBoxCurrencies->itemData(i).toString();
            if (code == mProduct->getCurrency()) {
                ui->comboBoxCurrencies->setCurrentIndex(i);
                break;
            }
        }
        if (!mProduct->isInfinitely()) {
            ui->lineEditCount->setText(QString::number(mProduct->count(), 'f', 2));
            ui->lineEditCount->setEnabled(true);
            ui->checkBoxInfinity->setChecked(false);
        }

        ui->lineEditStepCount->setText(QString::number(mProduct->stepCount(), 'f', 2));
        ui->lineEditMinCount->setText(QString::number(mProduct->minCount(), 'f', 2));
        ui->lineEditPresence->setText(mProduct->precense());
        ui->checkBoxActIsNew->setChecked(mProduct->isNew());
        ui->checkBoxActIsHit->setChecked(mProduct->isHit());
        ui->checkBoxActIsSpec->setChecked(mProduct->isSpec());
        ui->checkBoxIsYandexMarket->setChecked(mProduct->isYAMarket());
        ui->lineEditBrand->setText(mProduct->brand()->getName());
        ui->doubleSpinBoxVolume->setValue(mProduct->volume());
        ui->doubleSpinBoxWeight->setValue(mProduct->weight());
        ui->spinBoxSortIndex->setValue(mProduct->getSortIndex());
        if (!mProduct->property("idYAMarketCategory").toString().isEmpty()) {
            QVariantMap map = SEConfig::getMainInfo()->property("yaMarketCategories").toMap();
            if (map.keys().contains(mProduct->property("idYAMarketCategory").toString()))
                ui->lineEditYandexMarketCategories->setText(map[mProduct->property("idYAMarketCategory").toString()].toString());
        }

        ui->widgetDescription->setHTMLText(mProduct->property("description").toString());

        if (mProduct->property("idWeightView").toInt()) {
            for (int i = 0; i < ui->comboBoxWeightView->count(); ++i) {
                if (mProduct->property("idWeightView").toInt() == ui->comboBoxWeightView->itemData(i).toInt()) {
                    ui->comboBoxWeightView->setCurrentIndex(i);
                    break;
                }
            }
        }
        if (mProduct->property("idWeightEdit").toInt()) {
            for (int i = 0; i < ui->comboBoxWeightEdit->count(); ++i) {
                if (mProduct->property("idWeightEdit").toInt() == ui->comboBoxWeightEdit->itemData(i).toInt()) {
                    ui->comboBoxWeightEdit->setCurrentIndex(i);
                    onChangeMeasureWeght();
                    break;
                }
            }
        }
        if (mProduct->property("idVolumeView").toInt()) {
            for (int i = 0; i < ui->comboBoxVolumeView->count(); ++i) {
                if (mProduct->property("idVolumeView").toInt() == ui->comboBoxVolumeView->itemData(i).toInt()) {
                    ui->comboBoxVolumeView->setCurrentIndex(i);
                    break;
                }
            }
        }
        if (mProduct->property("idVolumeEdit").toInt()) {
            for (int i = 0; i < ui->comboBoxVolumeEdit->count(); ++i) {
                if (mProduct->property("idVolumeEdit").toInt() == ui->comboBoxVolumeEdit->itemData(i).toInt()) {
                    ui->comboBoxVolumeEdit->setCurrentIndex(i);
                    onChangeMeasureVolume();
                    break;
                }
            }
        }

        mIsFilled = true;
    }
    connectSignalsChanged();
}

void FrameCardMainInfo::refreshData()
{
    mIsFilled = false;   
    fillControls();
}

void FrameCardMainInfo::correctDataBeforeSave()
{
    if (mProduct->getIds().isEmpty())
        mProduct->setCurrency(ui->comboBoxCurrencies->currentData().toString());
    ui->widgetDescription->refreshSourceText();
    mProduct->setDescription(ui->widgetDescription->textHTML());
}

void FrameCardMainInfo::onSelectGroup()
{
    FormProductsGroups *form = new FormProductsGroups(this);

    form->setWindowFlags(Qt::Dialog);
    form->setSelectMode(false);
    if (form->exec() == QDialog::Accepted) {
        QListIterator<DataProductsGroup *> it(form->listSelected());
        if (it.hasNext()) {
            DataProductsGroup *gr = (it.next());
            mProduct->setIdGroup(gr->getId());            
            mProduct->setIdModificationGroupDef(gr->idModificationGroupDef());
            if (gr->getPathGroup().isEmpty())
                ui->lineEditGroup->setText(gr->getName());
            else ui->lineEditGroup->setText(gr->getPathGroup());
        }        
        emit modified();
    }
    delete form;
}

void FrameCardMainInfo::onDeleteGroup()
{
    mProduct->setIdGroup(QString());
    mProduct->setIdModificationGroupDef(QString());
    ui->lineEditGroup->clear();
    emit modified();    
}
