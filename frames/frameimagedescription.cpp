#include "frameimagedescription.h"
#include "ui_frameimagedescription.h"

#include <QFile>

FrameImageDescription::FrameImageDescription(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FrameImageDescription)
{
    ui->setupUi(this);
}

FrameImageDescription::~FrameImageDescription()
{
    delete ui;
}

void FrameImageDescription::setImage(DataItem *image)
{
    mImage = image;
    ui->lineEditFileName->setText(fileShortName(image->getImageFile()));
    ui->lineEditDescription->setText(image->getImageAlt());
    connect(ui->lineEditDescription, SIGNAL(textEdited(QString)),
            SLOT(onChangeImageAlt(QString)));
}

DataItem *FrameImageDescription::image() const
{    
    return mImage;
}

void FrameImageDescription::onChangeImageAlt(const QString &arg)
{
    mImage->setImageAlt(arg);
    emit modified();
}

QString FrameImageDescription::fileShortName(const QString &fileName)
{
    QString result;
    if (!fileName.contains("http"))
        for (int i = 0; i < fileName.size(); i++) {
            if (fileName.at(i) != '\\' && fileName.at(i) != '/')
                result += fileName.at(i);
            else result.clear();
        }
    else result = fileName;
    return result;
}

