#include "filewebloader.h"

#include <QNetworkReply>
#include <QUrl>
#include <QNetworkRequest>
#include <QImageReader>
#include <QFile>
#include <QDir>

FileWebLoader::FileWebLoader(const QString &urlImage, const QModelIndex &modelIndex):
    QObject(nullptr)
{
    this->urlImage = urlImage;
    this->modelIndex = modelIndex;
}

void FileWebLoader::process()
{
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)), this,
            SLOT(onFinishedLoad(QNetworkReply*)), Qt::QueuedConnection);

    QUrl url(urlImage);
    QNetworkRequest request(url);
    manager->get(request);
}

void FileWebLoader::onFinishedLoad(QNetworkReply *reply)
{
    bool result = false;
    if (reply->error() == QNetworkReply::NoError)
    {        
        int status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        if (status == 301) {
            QUrl url = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();            
            QNetworkAccessManager *manager = new QNetworkAccessManager(this);
            connect(manager, SIGNAL(finished(QNetworkReply*)),
                    this, SLOT(onFinishedLoad(QNetworkReply*)), Qt::QueuedConnection);
            QNetworkRequest request(url);
            manager->get(request);
            return;
        }

        QString folder(SEConfig::getFolderCache());
        SEConfig::createDir(folder);
        QByteArray buffer = reply->readAll();
        if (buffer.size()) {
            QFile file(folder + QDir::separator() + QMD5(urlImage));
            if (file.open(QFile::WriteOnly)) {
                file.write(buffer);
                file.close();
                result = true;
                emit finishedDownloadImage(file.fileName(), modelIndex);
            }
        }        
    }
    if (!result)
        emit finishedDownloadImage(QString(), modelIndex);

    reply->deleteLater();
}
