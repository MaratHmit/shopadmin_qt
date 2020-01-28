#include "dialogdynfieldsgroup.h"
#include "ui_dialogdynfieldsgroup.h"

#include "apiadapter.h"

DialogDynFieldsGroup::DialogDynFieldsGroup(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogDynFieldsGroup)
{
    ui->setupUi(this);

    initVariables();
    initSignals();
}

DialogDynFieldsGroup::~DialogDynFieldsGroup()
{
    delete ui;
    delete mGroup;
}

void DialogDynFieldsGroup::setDataType(const QString &dataType)
{
    mGroup->setProperty("data", dataType);
}

void DialogDynFieldsGroup::setGroup(const DataItem *group)
{
    mGroup->copy(group);
}

const DataItem *DialogDynFieldsGroup::group() const
{
    return mGroup;
}

void DialogDynFieldsGroup::onSave()
{
    if (ui->lineEditName->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, tr("Сохранение группы"),
                             tr("Введите наименование группы!"));
        ui->lineEditName->setFocus();
        return;
    }
    fillData();
    if (save())
        accept();
}

void DialogDynFieldsGroup::showEvent(QShowEvent *)
{
    fillControls();
}

void DialogDynFieldsGroup::fillData()
{
    mGroup->setName(ui->lineEditName->text().trimmed());
    mGroup->setDescription(ui->plainTextEdit->document()->toPlainText());
    mGroup->setIsActive(ui->checkBoxIsActive->isChecked());
}

void DialogDynFieldsGroup::fillControls()
{
    if (mGroup->getId().isEmpty())
        return;

    ui->lineEditName->setText(mGroup->getName());
    ui->plainTextEdit->setPlainText(mGroup->property("description").toString());
    ui->checkBoxIsActive->setChecked(mGroup->getIsActive());
}

void DialogDynFieldsGroup::initVariables()
{
    mGroup = new DataItem();
}

void DialogDynFieldsGroup::initSignals()
{
    connect(ui->pushButtonCancel, SIGNAL(clicked(bool)), SLOT(reject()));
    connect(ui->pushButtonOk, SIGNAL(clicked(bool)), SLOT(onSave()));
}

bool DialogDynFieldsGroup::save()
{
    return ApiAdapter::getApi()->saveDynFieldsGroup(mGroup);
}
