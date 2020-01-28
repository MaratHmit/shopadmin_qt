#include "formmodificationgroupcard.h"
#include "ui_formmodificationgroupcard.h"
#include "apiadapter.h"
#include "seconfig.h"
#include "userpermission.h"

FormModificationGroupCard::FormModificationGroupCard(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FormModificationGroupCard)
{
    ui->setupUi(this);
    mGroup = 0;
    initSignals();
}

FormModificationGroupCard::~FormModificationGroupCard()
{
    if (mGroup)
        delete mGroup;
    delete ui;
}

void FormModificationGroupCard::setGroup(DataModificationGroup *group)
{
    mGroup = new DataModificationGroup(group);
}

void FormModificationGroupCard::onSaveClicked()
{
    ui->buttonCancel->setEnabled(false);
    ui->buttonSave->setEnabled(false);

    if (save())
        accept();

    ui->buttonCancel->setEnabled(true);
    ui->buttonSave->setEnabled(true);
}

void FormModificationGroupCard::onNameChanged()
{
    ui->buttonSave->setEnabled(!ui->lineEditName->text().trimmed().isEmpty());
}

void FormModificationGroupCard::onModifiedListValues()
{

}

void FormModificationGroupCard::showEvent(QShowEvent *)
{
    ui->lineEditName->setFocus();
    getData();
}

void FormModificationGroupCard::getData()
{
    if (!mGroup)
        mGroup = new DataModificationGroup();

    ui->lineEditName->clear();
    ui->comboBoxTypes->clear();    
    initBoxPricesTypes();


    if (mGroup->getId().isEmpty())
        return;

    ui->lineEditName->setText(mGroup->getName());   
    for (int i = 0; i < ui->comboBoxTypes->count(); i++) {
        if (ui->comboBoxTypes->itemData(i).toString() == mGroup->type()) {
            ui->comboBoxTypes->setCurrentIndex(i);
            break;
        }
    }
}

void FormModificationGroupCard::setData()
{
    mGroup->setName(ui->lineEditName->text().trimmed());
    mGroup->setType(ui->comboBoxTypes->currentData().toString());
}

void FormModificationGroupCard::initSignals()
{
    connect(ui->buttonCancel, SIGNAL(clicked()), SLOT(reject()));
    connect(ui->buttonSave, SIGNAL(clicked()), SLOT(onSaveClicked()));
    connect(ui->lineEditName, SIGNAL(textChanged(QString)),
            SLOT(onNameChanged()));

}

void FormModificationGroupCard::initBoxPricesTypes()
{    
    for (int i = 0; i < SEConfig::getModificationsGroupsPricesTypes()->size(); i++)
        ui->comboBoxTypes->addItem(SEConfig::getModificationsGroupsPricesTypes()->at(i)->getName(),
                                   SEConfig::getModificationsGroupsPricesTypes()->at(i)->getId());
    if (ui->comboBoxTypes->count())
        ui->comboBoxTypes->setCurrentIndex(0);
}

bool FormModificationGroupCard::save()
{
    setData();    
    return ApiAdapter::getApi()->saveModificationsGroup(mGroup);
}
