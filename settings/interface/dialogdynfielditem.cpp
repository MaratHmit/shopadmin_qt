#include "dialogdynfielditem.h"
#include "ui_dialogdynfielditem.h"
#include "apiadapter.h"

#include <QMessageBox>

DialogDynFieldItem::DialogDynFieldItem(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogDynFieldItem)
{
    ui->setupUi(this);

    initVariables();
    initSignals();
}

DialogDynFieldItem::~DialogDynFieldItem()
{
    delete ui;
    delete mDynFieldItem;
}

void DialogDynFieldItem::setDataType(const QString &dataType)
{
    mDynFieldItem->setProperty("data", dataType);
}

void DialogDynFieldItem::setGroups(const ListDataItems *groups)
{
    for (int i = 0; i < groups->size(); ++i)
        ui->comboBoxGroups->addItem(groups->at(i)->getName(), groups->at(i)->getId());            
}

void DialogDynFieldItem::setDynFieldItem(const DataItem *dynFieldItem)
{
    mDynFieldItem->copy(dynFieldItem);
}

const DataItem *DialogDynFieldItem::dynFieldItem() const
{
    return mDynFieldItem;
}

void DialogDynFieldItem::onChangeType()
{
    bool isShowFieldValues = false;
    switch (ui->comboBoxType->currentIndex()) {
    case 3:
        isShowFieldValues = true;
        break;
    case 4:
        isShowFieldValues = true;
        break;
    case 5:
        isShowFieldValues = true;
        break;
    default:
        break;
    }
    ui->labelValues->setVisible(isShowFieldValues);
    ui->lineEditValues->setVisible(isShowFieldValues);
}

void DialogDynFieldItem::onSave()
{
    if (ui->lineEditName->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, tr("Сохранение элемента"),
                             tr("Введите наименование элемента!"));
        ui->lineEditName->setFocus();
        return;
    }
    fillData();
    if (save())
        accept();
}

void DialogDynFieldItem::showEvent(QShowEvent *)
{
    fillControls();
}

void DialogDynFieldItem::fillData()
{
    mDynFieldItem->setName(ui->lineEditName->text().trimmed());
    mDynFieldItem->setCode(ui->lineEditCode->text().trimmed());
    mDynFieldItem->setDescription(ui->plainTextEditDescription->document()->toPlainText());
    mDynFieldItem->setIsActive(ui->checkBoxIsActive->isChecked());
    mDynFieldItem->setProperty("isRequired", ui->checkBoxIsRequired->isChecked());
    mDynFieldItem->setProperty("placeholder", ui->lineEditPlaceholder->text().trimmed());
    mDynFieldItem->setListValues(ui->lineEditValues->text().trimmed());
    switch (ui->comboBoxType->currentIndex()) {
    case 0:
        mDynFieldItem->setValueType("S");
        break;
    case 1:
        mDynFieldItem->setValueType("D");
        break;
    case 2:
        mDynFieldItem->setValueType("T");
        break;
    case 3:
        mDynFieldItem->setValueType("L");
        break;
    case 4:
        mDynFieldItem->setValueType("CB");
        break;
    case 5:
        mDynFieldItem->setValueType("R");
        break;
    case 6:
        mDynFieldItem->setValueType("DT");
        break;
    default:
        break;
    }
    if (ui->comboBoxGroups->currentIndex())
        mDynFieldItem->setIdGroup(ui->comboBoxGroups->currentData().toString());
}

void DialogDynFieldItem::fillControls()
{
    if (mDynFieldItem->getId().isEmpty())
        return;

    ui->lineEditName->setText(mDynFieldItem->getName());
    ui->lineEditCode->setText(mDynFieldItem->getCode());
    ui->plainTextEditDescription->setPlainText(mDynFieldItem->property("description").toString());
    ui->checkBoxIsActive->setChecked(mDynFieldItem->getIsActive());
    ui->checkBoxIsRequired->setChecked(mDynFieldItem->property("isRequired").toBool());
    ui->lineEditPlaceholder->setText(mDynFieldItem->property("placeholder").toString());
    ui->lineEditValues->setText(mDynFieldItem->getValues().join(","));
    if (!mDynFieldItem->getIdGroup().isEmpty()) {
        for (int i = 0; i < ui->comboBoxGroups->count(); i++)
            if (ui->comboBoxGroups->itemData(i).toString() == mDynFieldItem->getIdGroup()) {
                ui->comboBoxGroups->setCurrentIndex(i);
                break;
            }
    }
    ui->comboBoxType->setCurrentText(mDynFieldItem->valueTypeTitle());
}

void DialogDynFieldItem::initVariables()
{
    mDynFieldItem = new DataItem();
    ui->labelValues->setVisible(false);
    ui->lineEditValues->setVisible(false);
}

void DialogDynFieldItem::initSignals()
{
    connect(ui->comboBoxType, SIGNAL(currentIndexChanged(int)), SLOT(onChangeType()));
    connect(ui->pushButtonCancel, SIGNAL(clicked(bool)), SLOT(reject()));
    connect(ui->pushButtonOk, SIGNAL(clicked(bool)), SLOT(onSave()));
}

bool DialogDynFieldItem::save()
{
    if (ApiAdapter::getApi()->saveDynField(mDynFieldItem))
        return true;
    return false;
}
