#include "formoptionvaluecard.h"
#include "ui_formoptionvaluecard.h"

#include "apiadapter.h"
#include "seconsts.h"

FormOptionValueCard::FormOptionValueCard(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FormOptionValueCard)
{
    ui->setupUi(this);

    mValue = new DataOptionValue;
    ui->pageImage->hideImageAlt();
    ui->pageImage->setSection(SEConsts::Section_Features);
}

FormOptionValueCard::~FormOptionValueCard()
{
    delete ui;
    delete mValue;
}

void FormOptionValueCard::setId(const QString id)
{
    mValue->setId(id);
}

void FormOptionValueCard::setOptions(const ListOptions *options)
{
    for(int i = 0; i < options->size(); i++)
        ui->comboBoxOptions->addItem(options->at(i)->getName(), options->at(i)->getId());
    ui->comboBoxOptions->setCurrentIndex(-1);
}

void FormOptionValueCard::setCurrentIdOption(const QString id)
{
    if (!id.isEmpty())
        mValue->setProperty("idOption", id);
}

void FormOptionValueCard::hideSelectedOption()
{
    ui->comboBoxOptions->hide();
    ui->labelOption->hide();
}

void FormOptionValueCard::save()
{
    if (ui->lineEditName->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Сохранение значения", "Не заполнено обязателное поле: <Наименование>!");
        ui->lineEditName->setFocus();
        return;
    }
    if (mValue->property("idOption").isNull()) {
        QMessageBox::warning(this, "Сохранение значения", "Не указана опция!");
        ui->comboBoxOptions->setFocus();
        return;
    }

    ui->buttonSave->setEnabled(false);
    if (ApiAdapter::getApi()->saveOptionValue(mValue)) {
        ui->pageImage->saveImage();
        accept();
    }
    ui->buttonSave->setEnabled(true);
}

void FormOptionValueCard::changeData()
{
    if (sender() == ui->comboBoxOptions)
        mValue->setProperty("idOption", ui->comboBoxOptions->currentData().toInt());
    if (sender() == ui->lineEditName)
        mValue->setName(ui->lineEditName->text().trimmed());
    if (sender() == ui->doubleSpinBox)
        mValue->setPrice(ui->doubleSpinBox->value());
    if (sender() == ui->checkBoxIsActive)
        mValue->setIsActive(ui->checkBoxIsActive->isChecked());
    if (sender() == ui->pageImage)
        mValue->setImageFile(ui->pageImage->fileImageName());

    if (!ui->lineEditName->text().isEmpty())
        ui->buttonSave->setEnabled(true);
}

void FormOptionValueCard::showEvent(QShowEvent *)
{
    getData();
    initSignals();
    ui->lineEditName->setFocus();
}

void FormOptionValueCard::initSignals()
{
    connect(ui->buttonSave, SIGNAL(clicked(bool)), SLOT(save()));
    connect(ui->lineEditName, SIGNAL(textChanged(QString)), SLOT(changeData()));
    connect(ui->comboBoxOptions, SIGNAL(currentIndexChanged(int)), SLOT(changeData()));
    connect(ui->doubleSpinBox, SIGNAL(valueChanged(double)), SLOT(changeData()));
    connect(ui->checkBoxIsActive, SIGNAL(clicked(bool)), SLOT(changeData()));
    connect(ui->pageImage, SIGNAL(modified()), SLOT(changeData()));

}

void FormOptionValueCard::getData()
{
    if (mValue->getId().isEmpty()) {
        setComboBoxOptions();
        return;
    }

    if (ApiAdapter::getApi()->getInfoOptionValue(mValue))
        fillControls();
}

void FormOptionValueCard::fillControls()
{
    setComboBoxOptions();

    ui->lineEditName->setText(mValue->getName());
    ui->doubleSpinBox->setValue(mValue->price());
    ui->pageImage->setImage(mValue->getImageFile());
    ui->checkBoxIsActive->setChecked(mValue->getIsActive());
}

void FormOptionValueCard::setComboBoxOptions()
{
    if (!mValue->property("idOption").isNull() && (mValue->property("idOption").toInt() > 0)) {
        for (int i = 0; i < ui->comboBoxOptions->count(); i++)
            if (ui->comboBoxOptions->itemData(i).toInt() == mValue->property("idOption").toInt()) {
                ui->comboBoxOptions->setCurrentIndex(i);
                break;
            }
    }
}

void FormOptionValueCard::on_pushButtonMainInfo_clicked()
{
     ui->stackedWidget->setCurrentIndex(0);
}

void FormOptionValueCard::on_pushButtonNote_clicked()
{
     ui->stackedWidget->setCurrentIndex(1);
}
