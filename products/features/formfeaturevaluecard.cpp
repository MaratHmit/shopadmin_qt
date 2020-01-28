#include "formfeaturevaluecard.h"
#include "ui_formfeaturevaluecard.h"
#include "apiadapter.h"
#include "seconfig.h"
#include "userpermission.h"

#include <QPalette>

FormFeatureValueCard::FormFeatureValueCard(QWidget *parent,
                                           const bool &isColorMode) :
    QDialog(parent),
    ui(new Ui::FormFeatureValueCard)
{    
    ui->setupUi(this);
    setWindowFlags(Qt::Window);

    dialogColor = 0;
    mIsColorMode = isColorMode;
    mFeatureValue = new DataFeature();

    ui->pageImage->hideImageAlt();
    ui->pageImage->setSection(SEConsts::Section_Features);

    initRegValidators();
}

FormFeatureValueCard::~FormFeatureValueCard()
{
    delete mFeatureValue;
    delete ui;
}

void FormFeatureValueCard::setIdFeature(const QString &idFeature)
{
    mFeatureValue->setIdFeature(idFeature);
}

void FormFeatureValueCard::setId(const QString &id)
{
    mFeatureValue->setId(id);
}

QString FormFeatureValueCard::idValue() const
{
    return mFeatureValue->getId();
}

void FormFeatureValueCard::on_pushButtonMainInfo_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void FormFeatureValueCard::on_pushButtonImage_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}

void FormFeatureValueCard::on_toolButton_clicked()
{
    if (!dialogColor)
        dialogColor = new QColorDialog(this);
    QColor color = dialogColor->getColor(mCurrentColor);
    if (color.isValid()) {
        setColor(color);
        disconnect(ui->lineEditColor, SIGNAL(textChanged(QString)), 0, 0);
        ui->lineEditColor->setText(mCurrentColor.name().toUpper());
        connect(ui->lineEditColor, SIGNAL(textChanged(QString)),
                SLOT(on_lineEditColor_textChanged(QString)));
    }
}

void FormFeatureValueCard::setColor(const QColor &color)
{
    mCurrentColor = color;

    QPalette pal;
    pal.setColor(QPalette::Background, color);
    ui->widgetColor->setPalette(pal);    
}

void FormFeatureValueCard::on_lineEditName_textChanged(const QString &arg1)
{
    ui->buttonSave->setEnabled(!arg1.isEmpty());
}

void FormFeatureValueCard::getData()
{
    if (!mFeatureValue->getId().isEmpty()) {
        if (ApiAdapter::getApi()->getInfoFeatureValue(mFeatureValue))
            fillControls();
    }
}

void FormFeatureValueCard::setData()
{
    if (ui->lineEditColor->isVisible()) {
        QString hexColor;
        hexColor = mCurrentColor.name().toUpper().remove(0, 1);
        mFeatureValue->setHEXColor(hexColor);
    }

    mFeatureValue->setName(ui->lineEditName->text());    
    mFeatureValue->setCode(ui->lineEditUrl->text().trimmed());
    mFeatureValue->setImageFile(ui->pageImage->fileImageName());    
    mFeatureValue->setIsSetAllFields(true);
}

void FormFeatureValueCard::fillControls()
{
    ui->lineEditName->setText(mFeatureValue->getName());
    ui->lineEditUrl->setText(mFeatureValue->getCode());
    if (mIsColorMode)
        if (!mFeatureValue->hexColor().isEmpty())
            ui->lineEditColor->setText("#" + mFeatureValue->hexColor());
    ui->pageImage->setImage(mFeatureValue->getImageFile());

}

bool FormFeatureValueCard::save()
{    
    if (ApiAdapter::getApi()->saveFeatureValue(mFeatureValue)) {
        ui->pageImage->saveImage();
        return true;
    }
    return false;
}

void FormFeatureValueCard::initRegValidators()
{    
    QRegExp regExpr("^#?([a-fA-F0-9]{6})$");
    QRegExpValidator *regColor = new QRegExpValidator(regExpr, this);
    ui->lineEditColor->setValidator(regColor);
}

void FormFeatureValueCard::on_buttonSave_clicked()
{
    ui->buttonSave->setEnabled(false);
    ui->buttonCancel->setEnabled(false);

    setData();
    if (save())
        accept();

    ui->buttonSave->setEnabled(true);
    ui->buttonCancel->setEnabled(true);
}

void FormFeatureValueCard::on_buttonCancel_clicked()
{
    reject();
}

void FormFeatureValueCard::showEvent(QShowEvent *)
{    
    if (!mIsColorMode) {
        ui->lineEditColor->hide();
        ui->widgetColor->hide();
        ui->labelName_2->hide();
        ui->toolButton->hide();
    }

    getData();
    ui->lineEditName->setFocus();
}


void FormFeatureValueCard::on_lineEditColor_textChanged(const QString &arg1)
{
    if (arg1.length() > 2)
        setColor(QColor(arg1));
}
