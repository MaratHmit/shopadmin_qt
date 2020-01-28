#include "formcontactgroupcard.h"
#include "ui_formcontactgroupcard.h"
#include "seconfig.h"
#include "apiadapter.h"

FormContactGroupCard::FormContactGroupCard(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FormContactGroupCard)
{
    ui->setupUi(this);
    group = new DataItem();
    initSignals();
}

FormContactGroupCard::~FormContactGroupCard()
{
    delete group;
    delete ui;
}

void FormContactGroupCard::setGroup(const DataItem *group)
{
    this->group->copy(group);
}

void FormContactGroupCard::onCancel()
{
    reject();
}

void FormContactGroupCard::onEditNameChanged(const QString &arg)
{
    ui->btnSave->setEnabled(!arg.isEmpty());
    group->setName(arg.trimmed());
}

void FormContactGroupCard::onSave()
{
    bool isNew = group->getId().isEmpty();
    ui->btnCancel->setEnabled(false);
    ui->btnOK->setEnabled(false);
    ui->btnSave->setEnabled(false);

    if (save()) {
        emit saved(group, isNew);
        if (sender() == ui->btnSave)
            ui->btnSave->setEnabled(false);
        if (sender() == ui->btnOK)
            accept();
    }
    ui->btnCancel->setEnabled(true);
    ui->btnOK->setEnabled(true);
}

void FormContactGroupCard::showEvent(QShowEvent *)
{
    ui->edtName->setText(group->getName());
}

void FormContactGroupCard::initSignals()
{
    connect(ui->edtName, SIGNAL(textEdited(QString)), SLOT(onEditNameChanged(QString)));
    connect(ui->btnSave, SIGNAL(clicked()), SLOT(onSave()));
    connect(ui->btnCancel, SIGNAL(clicked()), SLOT(reject()));
    connect(ui->btnOK, SIGNAL(clicked()), SLOT(onSave()));
}

bool FormContactGroupCard::save()
{
    return ApiAdapter::getApi()->saveContactGroup(group);
}
