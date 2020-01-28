#include "formoptiongroupcard.h"
#include "ui_formoptiongroupcard.h"

#include "seconfig.h"
#include "seconsts.h"
#include "apiadapter.h"

FormOptionGroupCard::FormOptionGroupCard(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FormOptionGroupCard)
{
    ui->setupUi(this);

    mGroup = new DataItem();
}

FormOptionGroupCard::~FormOptionGroupCard()
{
    delete mGroup;
    delete ui;
}

void FormOptionGroupCard::setId(QString &id)
{
    mGroup->setId(id);
}

void FormOptionGroupCard::showEvent(QShowEvent *)
{
    getData();
    ui->lineEditName->setFocus();
    connect(ui->lineEditName, SIGNAL(textEdited(QString)), SLOT(checkEnabled()));
    connect(ui->plainTextEdit, SIGNAL(textChanged()), SLOT(checkEnabled()));
    connect(ui->checkBoxIsActive, SIGNAL(clicked(bool)), SLOT(checkEnabled()));
}

void FormOptionGroupCard::setData()
{
    mGroup->setName(ui->lineEditName->text());
    mGroup->setDescription(ui->plainTextEdit->document()->toPlainText());
    mGroup->setIsActive(ui->checkBoxIsActive->isChecked());
}

void FormOptionGroupCard::getData()
{
    if (mGroup->getId().isEmpty())
        return;

    if (ApiAdapter::getApi()->getInfoOptionGroup(mGroup))
        fillControls();
}

void FormOptionGroupCard::fillControls()
{
    ui->lineEditName->setText(mGroup->getName());
    ui->plainTextEdit->document()->setPlainText(mGroup->property("description").toString());
    ui->checkBoxIsActive->setChecked(mGroup->getIsActive());
}

bool FormOptionGroupCard::save()
{
    mGroup->setIsSetAllFields(true);
    if (ApiAdapter::getApi()->saveOptionGroup(mGroup))
        return true;

    return false;
}

void FormOptionGroupCard::on_buttonCancel_clicked()
{
    reject();
}

void FormOptionGroupCard::on_buttonSave_clicked()
{
    ui->buttonSave->setEnabled(false);
    ui->buttonCancel->setEnabled(false);

    setData();
    if (save())
        accept();

    ui->buttonSave->setEnabled(true);
    ui->buttonCancel->setEnabled(true);
}

void FormOptionGroupCard::checkEnabled()
{
     ui->buttonSave->setEnabled(!ui->lineEditName->text().isEmpty());
}
