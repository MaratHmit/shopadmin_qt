#include "threadloadimage.h"

#include <QEventLoop>
#include <QNetworkReply>
#include <QUrl>
#include <QNetworkRequest>
#include <QtGuiDepends>
#include <QImageReader>
#include <QMessageBox>

ThreadLoadImage::ThreadLoadImage(QObject *parent):
    QObject(parent)
{
    listFiles = new QStringList;
    countLoaded = 0;
    isPreviewMode = false;
    imageSection = SEConsts::Section_Products;
}

ThreadLoadImage::~ThreadLoadImage()
{
    delete listFiles;
}

void ThreadLoadImage::process()
{
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(onFinishedLoad(QNetworkReply*)), Qt::QueuedConnection);

    QString fileURL = urlFolder;

    int i = 0;
    foreach (QString fileName, *listFiles) {
        if (isPreviewMode)
            fileURL += "?size=64&img=images/rus/" + SEConfig::getSectionImage(imageSection);
        fileURL += "/" + fileName;
        QUrl url;
        url.setUrl(fileURL);
        QNetworkRequest request(url);
        QNetworkReply * reply = manager->get(request);
        requestsFilesMap.insert(reply, fileName);

        i++;
    }    
}

QStringList *ThreadLoadImage::getListFiles()
{
    return listFiles;
}

void ThreadLoadImage::setUrl(const QString &urlFolder)
{
    this->urlFolder = urlFolder;
}

void ThreadLoadImage::setFolder(const QString &diskFolder)
{
    this->diskFolder = diskFolder;
}

void ThreadLoadImage::setPreviewMode()
{
    isPreviewMode = true;
}

void ThreadLoadImage::setImageSection(const int &section)
{
    imageSection = section;
}

void ThreadLoadImage::onFinishedLoad(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError)
    {
        QString fileName = requestsFilesMap.value(reply);
        int status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        if (status == 301) {
            QUrl url = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
            QNetworkAccessManager *manager = new QNetworkAccessManager(this);
            connect(manager, SIGNAL(finished(QNetworkReply*)),
                    this, SLOT(onFinishedLoad(QNetworkReply*)), Qt::QueuedConnection);
            QNetworkRequest request(url);
            QNetworkReply * reply = manager->get(request);
            requestsFilesMap.insert(reply, fileName);
            return;
        }

        QByteArray jpegData = reply->readAll();
        if (!jpegData.isNull()) {
            QPixmap pixmap;
            pixmap.loadFromData(jpegData);
            if (!pixmap.isNull()) {
                SEConfig::createDir(diskFolder);
                SEConfig::createDir(diskFolder + QDir::separator() + "prev");
                QFile fileImage(diskFolder + QDir::separator() + fileName);
                QFile filePrevImage(diskFolder + QDir::separator() +
                                    "prev" + QDir::separator() + fileName);
                isPreviewMode ? pixmap.save(filePrevImage.fileName()) : pixmap.save(fileImage.fileName());
            }
        }

        emit finishLoad(fileName, ++countLoaded);
        if (countLoaded == listFiles->size())
            emit finished();
    }

    reply->deleteLater();
}

void ThreadLoadImage::stop()
{
    emit finished();
}


