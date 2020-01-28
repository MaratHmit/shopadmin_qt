#include "frameimage.h"
#include "ui_frameimage.h"

#include <QDir>
#include <QFile>
#include <QPixmap>
#include <QFileDialog>
#include <QThread>
#include <QtMath>
#include <QDebug>

#include "filewebloader.h"
#include "filewebuploader.h"
#include "formlibimages.h"
#include "seutils.h"
#include "apiadapter.h"

FrameImage::FrameImage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FrameImage), bufferImage(new QByteArray)
{
    ui->setupUi(this);        
    connect(ui->lineEditImageAlt, SIGNAL(textEdited(QString)), SIGNAL(modified()));
    connect(ui->lineEditImageName, SIGNAL(textEdited(QString)), SLOT(onImageNameChange()));
    connect(ui->lineEditImageName, SIGNAL(textEdited(QString)), SLOT(editImage(QString)));
}

FrameImage::~FrameImage()
{
    delete ui;
    delete bufferImage;
}

void FrameImage::setSection(const int &section)
{
    mSection = section;
}

void FrameImage::setImage(const QString &fileName)
{
    if (fileName.isEmpty())
        return;

    ui->lineEditImageName->setText(fileName);
    if (isVisible())
        loadImage();
}

void FrameImage::setImageAlt(const QString &strAlt)
{
    ui->lineEditImageAlt->setText(strAlt);
}

void FrameImage::hideImageAlt()
{
    ui->lineEditImageAlt->setHidden(true);
    ui->labelImageAlt->setHidden(true);
}

const QSize FrameImage::imageSize() const
{
    return mImageSize;
}

bool FrameImage::isScaledOriginImage(int *w, int *h)
{
    ListIntegrations *shopParameters = SEConfig::getShopParameters();
    if (!shopParameters)
        return false;

    for (int j = 0; j < shopParameters->size(); ++j) {
        const ListDataItems *listParameters = shopParameters->at(j)->listParameters();
        for (int i = 0; i < listParameters->size(); ++i) {
            bool isScaledOrigin = (listParameters->at(i)->getCode() ==
                                   SEConsts::SHOPPARAMS_SIZEPICTURE) && listParameters->at(i)->getIsActive();
            if (isScaledOrigin) {
                QString params =  listParameters->at(i)->getValue();
                QStringList sizes = params.split("x");
                if (sizes.count() > 0) {
                    *w = QString(sizes.at(0)).toInt();
                    *h = *w;
                }
                if (sizes.count() > 1)
                    *h = QString(sizes.at(1)).toInt();
                isScaledOrigin  = isScaledOrigin && (*w >= 64) && (*h >= 64);
                return true;
            }
        }
    }
    return false;
}

QString FrameImage::fileImageName() const
{
    return ui->lineEditImageName->text();
}

QString FrameImage::fileImagePathName() const
{
    QString folder = SEConfig::getFolderImages(mSection);

    return folder + QDir::separator() +
            ui->lineEditImageName->text();
}

QString FrameImage::altImage() const
{
    return ui->lineEditImageAlt->text();
}

void FrameImage::saveImage()
{
    if (!mIsModified || ui->lineEditImageName->text().isEmpty() ||
            (ui->lineEditImageName->text().trimmed().indexOf("http") == 0))
        return;

    QString folder = SEConfig::getFolderImages(mSection);
    SEConfig::createDir(folder);

    QString fullFileName = folder + QDir::separator() +
            ui->lineEditImageName->text().trimmed();
    QFile file(fullFileName);
    if (bufferImage->size() > 0 && file.open(QIODevice::WriteOnly)) {
        file.write(*bufferImage);
        file.close();

        QImage img(fullFileName);        
        int w = 0, h = 0;
        bool isScaledOrigin = isScaledOriginImage(&w, &h);
        QImage prev;
        if (isScaledOrigin) {
            QImage origin = img.scaled(w, h, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            prev = origin.scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            origin.save(fullFileName);
        } else prev = img.scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation);


        QThread* thread = new QThread;
        FileWebUploader *uploader = new FileWebUploader;
        uploader->moveToThread(thread);
        uploader->listFiles()->append(fullFileName);
        uploader->setSection(mSection);
        connect(thread, SIGNAL(started()), uploader, SLOT(process()));
        connect(uploader, SIGNAL(finished()), thread, SLOT(quit()));
        connect(uploader, SIGNAL(finished()), uploader, SLOT(deleteLater()));
        connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
        thread->start();
    }
}

void FrameImage::editImage(const QString &fileName)
{
    if (fileName.isEmpty())
        return;

    loadImage();
}

void FrameImage::loadImage()
{
    if (ui->lineEditImageName->text().isEmpty())
        return;

    QString fileName = ui->lineEditImageName->text().trimmed();
    if (fileName.indexOf("http") == 0)
        fileName = QMD5(fileName) + ".jpeg";

    QString folder(SEConfig::getFolderImages(mSection));
    QString fullFileName = folder + QDir::separator() + fileName;

    QFile file(fullFileName);    
    if (file.exists() && file.size())
        showImageByFullName(fullFileName);
    else loadFromServer();
}

void FrameImage::onImageNameChange()
{
    emit imageChanged(ui->lineEditImageName->text());
    emit modified();
}

void FrameImage::loadFromServer()
{
    QString urlImage =
            SEConfig::getUrlImages(mSection) + "/" + ui->lineEditImageName->text().trimmed();

    QThread* thread = new QThread;
    FileWebLoader *loader = new FileWebLoader(urlImage);
    loader->moveToThread(thread);
    connect(thread, SIGNAL(started()), loader, SLOT(process()));
    connect(loader, SIGNAL(finishedDownloadImage(QString,QModelIndex)), thread, SLOT(quit()));
    connect(loader, SIGNAL(finishedDownloadImage(QString,QModelIndex)), SLOT(showImageByFullName(QString)));
    connect(loader, SIGNAL(finishedDownloadImage(QString,QModelIndex)), loader, SLOT(deleteLater()));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

    thread->start(QThread::IdlePriority);
}

void FrameImage::on_toolButtonOpenImage_clicked()
{
    QString fileImage =
            QFileDialog::getOpenFileName(this,
                                         tr("Выбор изображения"),
                                         "", tr("Изображения") +
                                         "(*.png *.xpm *.jpg)");
    if (!fileImage.isEmpty()) {
        QPixmap pm(fileImage);
        if (pm.height() && pm.width()) {
            mIsModified = true;
            QFileInfo fileInfo(fileImage);
            ui->lineEditImageName->setText(fileInfo.fileName().toLower().replace(QString(" "), QString("_")));
            showImageByFullName(fileImage);
            emit modified();
            emit imageChanged(ui->lineEditImageName->text());
        } else QMessageBox::information(this, tr("Добавление изображения"),
                                        fileImage + " не является изображением!", QMessageBox::Ok);
    }
}

void FrameImage::on_toolButtonDeleteImage_clicked()
{
    bufferImage->clear();
    ui->lineEditImageName->clear();
    if (ui->labelImage->pixmap())
        ui->labelImage->setPixmap(QPixmap());
    emit modified();
    emit imageChanged(QString());
}

void FrameImage::on_toolButtonOpenImageFromLib_clicked()
{       
    FormLibImages *libImages = new FormLibImages();

    libImages->setSelectMode(mSection, false);
    libImages->setWindowTitleBySection(mSection);
    if (libImages->exec() == QDialog::Accepted) {
        QString fileImage = libImages->selectionFile();
        setImage(fileImage);
        emit modified();
        emit imageChanged( ui->lineEditImageName->text());
    }

    delete libImages;
}

void FrameImage::showEvent(QShowEvent *)
{
    loadImage();
}

void FrameImage::showImageByFullName(const QString &fullFileName)
{
    QFile file(fullFileName);
    if (file.open(QIODevice::ReadOnly)) {
        bufferImage->clear();
        bufferImage->append(file.readAll());
        file.close();        
    }

    QPixmap pm(fullFileName);        
    mImageSize = pm.size();
    if (pm.size().width() > width() ||
            pm.size().height() > height()) {
        int m = qMin(width(), height());        
        pm = pm.scaled(QSize(m, m), Qt::KeepAspectRatio);
    }
    ui->labelImage->setPixmap(pm);
}
