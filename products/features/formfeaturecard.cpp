#include "formfeaturecard.h"
#include "ui_formfeaturecard.h"
#include "formfeaturesgroups.h"
#include "apiadapter.h"
#include "seconfig.h"
#include "requestfeature.h"

FormFeatureCard::FormFeatureCard(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FormFeatureCard)
{
    initialization();
}

FormFeatureCard::FormFeatureCard(QWidget *parent, const bool &isModificationMode) :
    QDialog(parent),
    ui(new Ui::FormFeatureCard)
{
    initialization();
    mIsModificationsMode = isModificationMode;
}

FormFeatureCard::~FormFeatureCard()
{
    mListTypesFeatures->deleteItems();;
    delete mListTypesFeatures;
    delete mFeature;
    delete ui;
}

void FormFeatureCard::initialization()
{
    ui->setupUi(this);
    mFeature = new DataFeature();
    mListTypesFeatures = new ListDataItems();
    mIsModificationsMode = false;

    setWindowFlags(Qt::Window);
    ui->pageImage->hideImageAlt();
    ui->pageImage->setSection(SEConsts::Section_Features);
}

void FormFeatureCard::setId(const QString &id)
{
    mFeature->setId(id);
    if (!id.isEmpty())
        setWindowTitle(tr("Редактировать параметр товара"));
}

void FormFeatureCard::showEvent(QShowEvent *)
{
    ui->lineEditName->setFocus();
    getData();
}

void FormFeatureCard::getData()
{
    initTypesFeatures();
    if (mFeature->getId().isEmpty())
        return;

    if (ApiAdapter::getApi()->getInfoFeature(mFeature))
        fillControls();
}

void FormFeatureCard::setData()
{
    mFeature->setName(ui->lineEditName->text());
    mFeature->setCode(ui->lineEditUrl->text());
    mFeature->setMeasure(ui->lineEditMeasure->text());
    mFeature->setDescription(ui->plainTextEdit->document()->toPlainText());
    mFeature->setImageFile(ui->pageImage->fileImageName());
    mFeature->setValueType(getTypeFeature(QString::number(ui->comboBoxType->currentIndex())));
    mFeature->setProperty("isYAMarket", ui->checkBoxIsYandexMarket->isChecked());
    mFeature->setProperty("placeholder", ui->lineEditPlaceholder->text());
}

void FormFeatureCard::fillControls()
{
    ui->lineEditName->setText(mFeature->getName());
    ui->lineEditUrl->setText(mFeature->getCode());
    ui->lineEditGroup->setText(mFeature->getNameGroup());
    ui->comboBoxType->setCurrentIndex(getIndexTypeFeature(mFeature->getValueType()));
    ui->lineEditMeasure->setText(mFeature->measure());
    ui->pageImage->setImage(mFeature->getImageFile());
    ui->plainTextEdit->document()->setPlainText(mFeature->property("description").toString());
    ui->checkBoxIsYandexMarket->setChecked(mFeature->property("isYAMarket").toBool());
    ui->lineEditPlaceholder->setText(mFeature->property("placeholder").toString());
}

bool FormFeatureCard::save()
{    
    mFeature->setIsSetAllFields(true);
    if (ApiAdapter::getApi()->saveFeature(mFeature)) {
        ui->pageImage->saveImage();
        return true;
    }
    return false;
}

QString FormFeatureCard::getTypeFeature(const QString &id) const
{
    for (int i = 0; i < ui->comboBoxType->count(); i++) {
        DataItem *item = reinterpret_cast <DataItem *> (ui->comboBoxType->itemData(i).toUInt());
        if (item->getId() == id)
            return item->getValueType();
    }
    return QString();
}

int FormFeatureCard::getIndexTypeFeature(const QString &valueType) const
{
    for (int i = 0; i < mListTypesFeatures->size(); i++) {
        if (mListTypesFeatures->at(i)->getValueType() == valueType)
            return i;
    }
    return 0;
}

void FormFeatureCard::initTypesFeatures()
{
    RequestFeature request;
    request.setIsList(mIsModificationsMode);

    ApiAdapter::getApi()->getListFeaturesTypes(request, mListTypesFeatures);
    ui->comboBoxType->clear();
    for (int i = 0; i < mListTypesFeatures->size(); i++) {
        ui->comboBoxType->addItem(mListTypesFeatures->at(i)->getName(),
                                  (qintptr) mListTypesFeatures->at(i));
    }
    if (ui->comboBoxType->count())
        ui->comboBoxType->setCurrentIndex(0);
}

void FormFeatureCard::on_pushButtonMainInfo_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void FormFeatureCard::on_pushButtonNote_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}

void FormFeatureCard::on_buttonSave_clicked()
{
    ui->buttonCancel->setEnabled(false);
    ui->buttonSave->setEnabled(false);

    setData();
    if (save())
        accept();

    ui->buttonCancel->setEnabled(true);
    ui->buttonSave->setEnabled(true);
}

void FormFeatureCard::on_toolButtonClearParentGroup_clicked()
{
    mFeature->setIdGroup(QString());
    mFeature->setNameGroup(QString());
    ui->lineEditGroup->clear();
}

void FormFeatureCard::on_toolButtonParentGroup_clicked()
{
    FormFeaturesGroups *formGroups = new FormFeaturesGroups();
    formGroups->setSelectMode();
    if (formGroups->exec() == QDialog::Accepted) {
        mFeature->setIdGroup(formGroups->currentIdGroup());
        mFeature->setNameGroup(formGroups->currentNameGroup());
        ui->lineEditGroup->setText(formGroups->currentNameGroup());
    }

    delete formGroups;
}

void FormFeatureCard::on_lineEditName_textChanged(const QString &arg1)
{
    ui->buttonSave->setEnabled(!arg1.isEmpty());
}
