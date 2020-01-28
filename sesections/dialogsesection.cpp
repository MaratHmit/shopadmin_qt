#include "dialogsesection.h"
#include "ui_dialogsesection.h"

DialogSESection::DialogSESection(QWidget *parent, const DataItem *section) :
    QDialog(parent),
    ui(new Ui::DialogSESection)
{
    ui->setupUi(this);

    mSection = new DataItem(section);
    initSignals();
}

DialogSESection::~DialogSESection()
{
    delete ui;
    delete mSection;
}

const DataItem *DialogSESection::section() const
{
    return mSection;
}

void DialogSESection::accept()
{
    fillData();
    if (save())
        QDialog::accept();
}

void DialogSESection::onEditName()
{
    ui->pushButtonOK->setEnabled(true);
}

void DialogSESection::showEvent(QShowEvent *)
{
    fillControls();
}

void DialogSESection::initSignals()
{
    connect(ui->pushButtonOK, SIGNAL(clicked(bool)), SLOT(accept()));
    connect(ui->pushButtonCancel, SIGNAL(clicked(bool)), SLOT(reject()));
    connect(ui->lineEditName, SIGNAL(textChanged(QString)), SLOT(onEditName()));
}

void DialogSESection::fillData()
{
    mSection->setName(ui->lineEditName->text().trimmed());
    mSection->setIsActive(ui->checkBoxIsActive->isChecked());
}

void DialogSESection::fillControls()
{
    ui->lineEditCode->setText(mSection->getCode());
    ui->lineEditPage->setText(mSection->property("note").toString());
    ui->lineEditName->setText(mSection->getName());
    ui->checkBoxIsActive->setChecked(mSection->getIsActive());
}

bool DialogSESection::save()
{    
    return ApiAdapter::getApi()->saveSESection(mSection);
}
