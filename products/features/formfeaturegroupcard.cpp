#include "formfeaturegroupcard.h"
#include "ui_formfeaturegroupcard.h"
#include "seconfig.h"
#include "seconsts.h"
#include "apiadapter.h"

FormFeatureGroupCard::FormFeatureGroupCard(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FormFeatureGroupCard)
{
    ui->setupUi(this);    

    setWindowFlags(Qt::Window);

    mGroup = new DataItem();

    ui->pageImage->hideImageAlt();
    ui->pageImage->setSection(SEConsts::Section_Features);
    ui->stackedWidget->setCurrentIndex(0);

    connect(ui->lineEditName, SIGNAL(textEdited(QString)), SLOT(checkEnabled()));
}

FormFeatureGroupCard::~FormFeatureGroupCard()
{
    delete mGroup;
    delete ui;
}

void FormFeatureGroupCard::setId(QString &id)
{
    mGroup->setId(id);
}

void FormFeatureGroupCard::showEvent(QShowEvent *)
{    
    getData();
    ui->lineEditName->setFocus();
}

void FormFeatureGroupCard::on_pushButtonMainInfo_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void FormFeatureGroupCard::on_pushButtonNote_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}

void FormFeatureGroupCard::on_buttonCancel_clicked()
{
    reject();
}

void FormFeatureGroupCard::checkEnabled()
{
    ui->buttonSave->setEnabled(!ui->lineEditName->text().isEmpty());
}

void FormFeatureGroupCard::setData()
{
    mGroup->setName(ui->lineEditName->text());
    mGroup->setDescription(ui->plainTextEdit->document()->toPlainText());
    mGroup->setImageFile(ui->pageImage->fileImageName());
}

void FormFeatureGroupCard::getData()
{
    if (mGroup->getId().isEmpty())
        return;

    if (ApiAdapter::getApi()->getInfoFeatureGroup(mGroup))
        fillControls();
}

void FormFeatureGroupCard::fillControls()
{
    ui->lineEditName->setText(mGroup->getName());
    ui->plainTextEdit->document()->setPlainText(mGroup->property("description").toString());
    ui->pageImage->setImage(mGroup->getImageFile());
}

bool FormFeatureGroupCard::save()
{    
    mGroup->setIsSetAllFields(true);
    if (ApiAdapter::getApi()->saveFeatureGroup(mGroup)) {
        if (!mGroup->getId().isEmpty())
            ui->pageImage->saveImage();
        return true;
    }
    return false;
}

void FormFeatureGroupCard::on_buttonSave_clicked()
{
    ui->buttonSave->setEnabled(false);
    ui->buttonCancel->setEnabled(false);

    setData();
    if (save())
        accept();

    ui->buttonSave->setEnabled(true);
    ui->buttonCancel->setEnabled(true);
}
