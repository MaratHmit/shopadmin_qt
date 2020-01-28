#include "formoptioncart.h"
#include "ui_formoptioncart.h"
#include "apiadapter.h"

FormOptionCart::FormOptionCart(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FormOptionCart)
{
    ui->setupUi(this);

    initVariables();    
}

FormOptionCart::~FormOptionCart()
{
    delete ui;
    delete mOption;
}

void FormOptionCart::setId(QString &id)
{
    mOption->setId(id);
}

void FormOptionCart::showEvent(QShowEvent *)
{
    getData();
    ui->lineEditName->setFocus();
    initSignals();
}

void FormOptionCart::initVariables()
{
    mOption = new DataItem;
    ui->pageImage->hideImageAlt();
    ui->pageImage->setSection(SEConsts::Section_Features);
    ui->stackedWidget->setCurrentIndex(0);
}

void FormOptionCart::initSignals()
{
    connect(ui->lineEditName, SIGNAL(textEdited(QString)), SLOT(checkEnabled()));
    connect(ui->lineEditCode, SIGNAL(textEdited(QString)), SLOT(checkEnabled()));
    connect(ui->lineEditNote, SIGNAL(textEdited(QString)), SLOT(checkEnabled()));
    connect(ui->plainTextEdit, SIGNAL(textChanged()), SLOT(checkEnabled()));
    connect(ui->checkBoxIsActive, SIGNAL(clicked(bool)), SLOT(checkEnabled()));
    connect(ui->checkBoxIsCounted, SIGNAL(clicked(bool)), SLOT(checkEnabled()));
    connect(ui->comboBoxGroups, SIGNAL(currentIndexChanged(int)), SLOT(checkEnabled()));
    connect(ui->comboBoxType, SIGNAL(currentIndexChanged(int)), SLOT(checkEnabled()));
    connect(ui->comboBoxTypePrice, SIGNAL(currentIndexChanged(int)), SLOT(checkEnabled()));
    connect(ui->pageImage, SIGNAL(imageChanged(QString)), SLOT(checkEnabled()));
}

void FormOptionCart::loadGroups()
{
    ListDataItems groups;
    ui->comboBoxGroups->addItem("Без группы", QString());
    if (ApiAdapter::getApi()->getListOptionsGroups(&groups)) {
        for (int i = 0; i < groups.size(); i++)
            ui->comboBoxGroups->addItem(groups.at(i)->getName(), groups.at(i)->getId());
    }
}

void FormOptionCart::setData()
{
    mOption->setName(ui->lineEditName->text().trimmed());
    mOption->setCode(ui->lineEditCode->text().trimmed());
    mOption->setNote(ui->lineEditNote->text().trimmed());
    mOption->setDescription(ui->plainTextEdit->document()->toPlainText());
    mOption->setIsActive(ui->checkBoxIsActive->isChecked());
    mOption->setProperty("isCounted", ui->checkBoxIsCounted->isChecked());
    mOption->setProperty("type", ui->comboBoxType->currentIndex());
    mOption->setProperty("typePrice", ui->comboBoxTypePrice->currentIndex());
    mOption->setImageFile(ui->pageImage->fileImageName());
    if (ui->comboBoxGroups->currentIndex() >= 0)
        mOption->setIdGroup(ui->comboBoxGroups->currentData().toString());
}

void FormOptionCart::getData()
{
    loadGroups();
    if (mOption->getId().isEmpty())
        return;

    if (ApiAdapter::getApi()->getInfoOption(mOption))
        fillControls();
}

void FormOptionCart::fillControls()
{
    ui->lineEditName->setText(mOption->getName());
    ui->lineEditCode->setText(mOption->getCode());
    ui->lineEditNote->setText(mOption->property("note").toString());
    ui->plainTextEdit->document()->setPlainText(mOption->property("description").toString());
    ui->checkBoxIsCounted->setChecked(mOption->property("isCounted").toBool());
    ui->checkBoxIsActive->setChecked(mOption->getIsActive());
    ui->comboBoxType->setCurrentIndex(mOption->property("type").toInt());
    ui->comboBoxTypePrice->setCurrentIndex(mOption->property("typePrice").toInt());
    ui->pageImage->setImage(mOption->getImageFile());
    if (!mOption->getIdGroup().isEmpty()) {
        for (int i = 0; i < ui->comboBoxGroups->count(); i++) {
            if (ui->comboBoxGroups->itemData(i).toString() == mOption->getIdGroup()) {
                ui->comboBoxGroups->setCurrentIndex(i);
                break;
            }
        }
    }

}

bool FormOptionCart::save()
{
    mOption->setIsSetAllFields(true);
    if (ApiAdapter::getApi()->saveOption(mOption)) {
        if (!mOption->getId().isEmpty())
            ui->pageImage->saveImage();
        return true;
    }

    return false;
}

void FormOptionCart::on_buttonSave_clicked()
{
    ui->buttonSave->setEnabled(false);
    ui->buttonCancel->setEnabled(false);

    setData();
    if (save())
        accept();

    ui->buttonSave->setEnabled(true);
    ui->buttonCancel->setEnabled(true);
}

void FormOptionCart::on_buttonCancel_clicked()
{
    reject();
}

void FormOptionCart::checkEnabled()
{
    ui->buttonSave->setEnabled(!ui->lineEditName->text().isEmpty());
}

void FormOptionCart::on_pushButtonMainInfo_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void FormOptionCart::on_pushButtonNote_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}
