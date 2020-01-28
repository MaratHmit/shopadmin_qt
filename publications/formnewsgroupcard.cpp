#include "formnewsgroupcard.h"
#include "ui_formnewsgroupcard.h"
#include "formnewsgroups.h"
#include "apiadapter.h"

FormNewsGroupCard::FormNewsGroupCard(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FormNewsGroupCard)
{
    ui->setupUi(this);
    mGroup = new DataItem();
    ui->pushButtonOK->setEnabled(false);
}

FormNewsGroupCard::~FormNewsGroupCard()
{
    delete ui;
    delete mGroup;
}

void FormNewsGroupCard::setIdParentGroup(const QString &idParent, const QString &titleParent)
{
    mGroup->setIdParent(idParent);
    ui->lineEditParentGroup->setText(titleParent);
}

void FormNewsGroupCard::setGroup(const DataItem *group)
{
    mGroup->copy(group);
}

const DataItem *FormNewsGroupCard::group() const
{
    return mGroup;
}

void FormNewsGroupCard::showEvent(QShowEvent *)
{
    fillControls();
    initSignals();
    ui->lineEditName->setFocus();
}

void FormNewsGroupCard::onSelectParentGroup()
{
    FormNewsGroups *formGroups = new FormNewsGroups(this);
    formGroups->setSelectMode(false);
    if (formGroups->exec() == QDialog::Accepted) {
        mGroup->setIdParent(formGroups->listSelected().at(0)->getId());
        ui->lineEditParentGroup->setText(formGroups->listSelected().at(0)->getName());
    }
}

void FormNewsGroupCard::onClearParentGroup()
{
    mGroup->setIdParent(QString());
    ui->lineEditParentGroup->clear();
}

void FormNewsGroupCard::onOK()
{
    if (save())
        accept();
}

void FormNewsGroupCard::checkEnabled()
{
    ui->pushButtonOK->setEnabled(!ui->lineEditName->text().trimmed().isEmpty());
}

void FormNewsGroupCard::fillControls()
{
    if (mGroup->getId().isEmpty())
        return;

    ui->lineEditName->setText(mGroup->getName());
    ui->lineEditCode->setText(mGroup->getCode());    
}

void FormNewsGroupCard::initSignals()
{
    connect(ui->toolButtonParentGroup, &QToolButton::clicked, this, &FormNewsGroupCard::onSelectParentGroup);
    connect(ui->toolButtonClearParentGroup, &QToolButton::clicked, this, &FormNewsGroupCard::onClearParentGroup);
    connect(ui->pushButtonCancel, SIGNAL(clicked()), SLOT(reject()));
    connect(ui->pushButtonOK, &QToolButton::clicked, this, &FormNewsGroupCard::onOK);
    connect(ui->lineEditName, SIGNAL(textEdited(QString)), mGroup, SLOT(setName(QString)));
    connect(ui->lineEditCode, SIGNAL(textEdited(QString)), mGroup, SLOT(setCode(QString)));
    connect(mGroup, &DataItem::modified, this, &FormNewsGroupCard::checkEnabled);
}

bool FormNewsGroupCard::save()
{
    return ApiAdapter::getApi()->saveNewsGroup(mGroup);
}
