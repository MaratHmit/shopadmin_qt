#include "dialogcardmeasure.h"
#include "ui_dialogcardmeasure.h"

#include "apiadapter.h"
#include "field.h"


DialogCardMeasure::DialogCardMeasure(QWidget *parent) :
    DialogCard(parent),
    ui(new Ui::DialogCardMeasure)
{
    ui->setupUi(this);    
    initVariables();
    initSignals();
}

DialogCardMeasure::~DialogCardMeasure()
{
    delete ui;
}

void DialogCardMeasure::setVolumeMode()
{
    mIsVolumeMode = true;
    setWindowTitle("Мера объёма");
    mApiName = SEConsts::API_MEASURES_VOLUME;
}

void DialogCardMeasure::initSignals()
{
    connect(ui->spinBoxPrecision, SIGNAL(valueChanged(int)), this, SLOT(setDecimalsPrecision(int)));
}

void DialogCardMeasure::initVariables()
{
    mIsVolumeMode = false;
    mApiName = SEConsts::API_MEASURES_WEIGHT;
}

void DialogCardMeasure::initControls()
{
    ui->lineEditName->clear();
    ui->lineEditMeasure->clear();
    ui->doubleSpinBoxValue->setValue(0);
    ui->spinBoxPrecision->setValue(3);
    ui->checkBoxIsBase->setChecked(false);
}

void DialogCardMeasure::fillControls()
{
    ui->lineEditName->setFocus();
    if (item->getId().isEmpty() || !ApiAdapter::getApi()->getInfoData(mApiName, item))
        return;

    ui->lineEditName->setText(item->getName());
    ui->lineEditMeasure->setText(item->property("designation").toString());
    ui->lineEditCode->setText(item->property("code").toString());
    ui->spinBoxPrecision->setValue(item->property("precision").toInt());
    ui->doubleSpinBoxValue->setValue(item->property("value").toDouble());
    ui->checkBoxIsBase->setChecked(item->property("is_base").toBool());
}

bool DialogCardMeasure::setData()
{
    if (ui->lineEditName->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Сохранение меры", "Не заполнено обязателное поле: <Наименование>!");
        ui->lineEditName->setFocus();
        return false;
    }
    if (ui->lineEditMeasure->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Сохранение меры", "Не заполнено обязателное поле: <Обозначение>!");
        ui->lineEditMeasure->setFocus();
        return false;
    }
    if (ui->lineEditCode->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Сохранение меры", "Не заполнено обязателное поле: <Код по ОКЕИ>!");
        ui->lineEditCode->setFocus();
        return false;
    }

    item->setName(ui->lineEditName->text().trimmed());
    item->setCode(ui->lineEditCode->text().trimmed());
    item->setProperty("designation", ui->lineEditMeasure->text().trimmed());
    item->setProperty("precision", ui->spinBoxPrecision->value());
    item->setProperty("value", ui->doubleSpinBoxValue->value());
    item->setProperty("is_base", ui->checkBoxIsBase->isChecked());

    return true;
}

bool DialogCardMeasure::save()
{
    return ApiAdapter::getApi()->save(mApiName, item);
}

void DialogCardMeasure::setDecimalsPrecision(const int value)
{
    ui->doubleSpinBoxValue->setDecimals(value);
}
