#include "dialogaddgroupletters.h"
#include "ui_dialogaddgroupletters.h"
#include "apiadapter.h"

DialogAddGroupLetters::DialogAddGroupLetters(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogAddGroupLetters)
{
    ui->setupUi(this);
    initVariables();
    initSignals();
}

DialogAddGroupLetters::~DialogAddGroupLetters()
{
    delete ui;
    delete mGroup;
}

const DataLetterGroup *DialogAddGroupLetters::group() const
{
    return mGroup;
}

void DialogAddGroupLetters::onOK()
{
    if (ApiAdapter::getApi()->saveTemplateLetterGroup(mGroup))
        accept();
}

void DialogAddGroupLetters::onChangeData()
{
    ui->pushButtonOK->setEnabled(!ui->lineEditName->text().trimmed().isEmpty());
    mGroup->setName(ui->lineEditName->text().trimmed());
}

void DialogAddGroupLetters::initVariables()
{
    mGroup = new DataLetterGroup();
}

void DialogAddGroupLetters::initSignals()
{
    connect(ui->pushButtonCancel, &QPushButton::clicked, this, &DialogAddGroupLetters::reject);
    connect(ui->pushButtonOK, &QPushButton::clicked, this, &DialogAddGroupLetters::onOK);
    connect(ui->lineEditName, &QLineEdit::textEdited, this, &DialogAddGroupLetters::onChangeData);
}
