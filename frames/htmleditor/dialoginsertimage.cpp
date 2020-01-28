#include "dialoginsertimage.h"
#include "ui_dialoginsertimage.h"

#include <QRegExp>
#include <QRegExpValidator>
#include <QFile>
#include <QFileInfo>

DialogInsertImage::DialogInsertImage(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogInsertImage)
{
    ui->setupUi(this);
    ui->tabWidget->setCurrentIndex(0);
    initSignals();
    setValidators();
}

DialogInsertImage::~DialogInsertImage()
{
    delete ui;
}

void DialogInsertImage::setSection(const int &section)
{
    ui->widgetImage->setSection(section);
    mSection = section;
}

void DialogInsertImage::setImageCode(const QString &imageSrc, const QString &imageHeight,
                                     const QString &imageWidth, const QString &imageStyle)
{    
    if (!imageSrc.isEmpty()) {
        QString file = imageSrc;
        int i = file.lastIndexOf("/");
        file = file.right(file.length() - i - 1);
        ui->widgetImage->setImage(file);
    }

    if (!imageStyle.isEmpty()) {
        QRegExp exp;
        exp.setMinimal(true);
        exp.setPattern("height:(\\d+)px");
        if (exp.indexIn(imageStyle) != -1 && exp.captureCount())
            ui->lineEditSizeHeight->setText(exp.cap(1));
        exp.setPattern("width:(\\d+)px");
        if (exp.indexIn(imageStyle) != -1 && exp.captureCount())
            ui->lineEditSizeWidth->setText(exp.cap(1));
        exp.setPattern("border:(\\d+)px");
        if (exp.indexIn(imageStyle) != -1 && exp.captureCount())
            ui->lineEditBorder->setText(exp.cap(1));
        exp.setPattern("align:(.+);");
        if (exp.indexIn(imageStyle) != -1 && exp.captureCount())
            ui->comboBoxAlignment->setCurrentText(exp.cap(1));
        exp.setPattern("margin-left:(\\d+)px");
        if (exp.indexIn(imageStyle) != -1 && exp.captureCount())
            ui->lineEditLeftMargin->setText(exp.cap(1));
        exp.setPattern("margin-top:(\\d+)px");
        if (exp.indexIn(imageStyle) != -1 && exp.captureCount())
            ui->lineEditTopMargin->setText(exp.cap(1));
        exp.setPattern("margin-right:(\\d+)px");
        if (exp.indexIn(imageStyle) != -1 && exp.captureCount())
            ui->lineEditRightMargin->setText(exp.cap(1));
        exp.setPattern("margin-bottom:(\\d+)px");
        if (exp.indexIn(imageStyle) != -1 && exp.captureCount())
            ui->lineEditBottomMargin->setText(exp.cap(1));
        exp.setPattern("margin:(\\d+)px\\s(\\d+)px\\s(\\d+)px\\s(\\d+)px");
        if (exp.indexIn(imageStyle) != -1 && exp.captureCount() > 3) {
            ui->lineEditTopMargin->setText(exp.cap(1));
            ui->lineEditRightMargin->setText(exp.cap(2));
            ui->lineEditBottomMargin->setText(exp.cap(3));
            ui->lineEditLeftMargin->setText(exp.cap(4));
        }

    }
    if (!imageHeight.isEmpty())
        ui->lineEditSizeHeight->setText(imageHeight);
    if (!imageWidth.isEmpty())
        ui->lineEditSizeWidth->setText(imageWidth);

}

const QString DialogInsertImage::getURL() const
{
    QString result;

    QFile file(ui->widgetImage->fileImagePathName());
    QFileInfo fileInfo(ui->widgetImage->fileImagePathName());

    if (file.exists()) {
        result = "<img src=\"file:///" + ui->widgetImage->fileImagePathName() + "\"";
        result.replace("\\", "/");
    }
    else result = "<img src=\"" + SEConfig::getUrlImages(mSection) + "/" + fileInfo.fileName() + "\"";
    if (!ui->widgetImage->altImage().isEmpty())
        result += " alt=\"" + ui->widgetImage->altImage() + "\"";

    QString style;    
    if (!ui->comboBoxAlignment->currentText().isEmpty())
        style += " align:" + ui->comboBoxAlignment->currentText() + ";";
    if (!ui->lineEditSizeHeight->text().isEmpty())
        style += " height:" + ui->lineEditSizeHeight->text() + "px;";
    if (!ui->lineEditSizeWidth->text().isEmpty())
        style += " width:" + ui->lineEditSizeWidth->text() + "px;";
    if (!ui->lineEditLeftMargin->text().isEmpty())
        style += " margin-left:" + ui->lineEditLeftMargin->text() + "px;";
    if (!ui->lineEditTopMargin->text().isEmpty())
        style += " margin-top:" + ui->lineEditTopMargin->text() + "px;";
    if (!ui->lineEditRightMargin->text().isEmpty())
        style += " margin-right:" + ui->lineEditRightMargin->text() + "px;";
    if (!ui->lineEditBottomMargin->text().isEmpty())
        style += " margin-bottom:" + ui->lineEditBottomMargin->text() + "px;";
    if (!ui->lineEditBorder->text().isEmpty())
        style += " border:" + ui->lineEditBorder->text() + "px solid;";
    if (!ui->lineEditId->text().isEmpty())
        result += " id=\"" + ui->lineEditId->text() + "\"";
    if (!ui->comboBoxClasses->currentText().isEmpty())
        result += " class=\"" + ui->comboBoxClasses->currentText() + "\"";
    if (!ui->lineEditStyle->text().isEmpty())
        style += " " + ui->lineEditStyle->text() + " ";

    if (!style.isEmpty())
        result += " style=\"" + style + "\"";

    result += " />";

    return result;
}

void DialogInsertImage::saveImage()
{
    ui->widgetImage->saveImage();
}

void DialogInsertImage::onImageChange()
{
    mImageSize = ui->widgetImage->imageSize();
}

void DialogInsertImage::onTabChange(const int &index)
{
    if (!index)
        return;

    if (ui->lineEditSizeHeight->text().isEmpty() && mImageSize.height() > 0)
        ui->lineEditSizeHeight->setText(QString::number(mImageSize.height()));
    if (ui->lineEditSizeWidth->text().isEmpty() && mImageSize.width() > 0)
        ui->lineEditSizeWidth->setText(QString::number(mImageSize.width()));
}

void DialogInsertImage::initSignals()
{
    connect(this, SIGNAL(accepted()), SLOT(saveImage()));
    connect(ui->widgetImage, SIGNAL(imageChanged(QString)), SLOT(onImageChange()));
    connect(ui->tabWidget, SIGNAL(currentChanged(int)), SLOT(onTabChange(int)));
}

void DialogInsertImage::setValidators()
{
    QRegExp regExprFloat("\\d*$");
    QRegExpValidator *validInt = new QRegExpValidator(regExprFloat, this);
    ui->lineEditSizeHeight->setValidator(validInt);
    ui->lineEditSizeWidth->setValidator(validInt);
}
