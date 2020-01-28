#include "threadloadurlimages.h"

#include <QEventLoop>
#include <QNetworkReply>
#include <QUrl>
#include <QNetworkRequest>
#include <QtGuiDepends>
#include <QImageReader>
#include <QMessageBox>

#include "seutils.h"
#include "seconfig.h"
#include "seconsts.h"

ThreadLoadUrlImages::ThreadLoadUrlImages(QObject *parent):
    QObject(parent)
{
    mListUrl = new QStringList;
    mCountLoaded = 0;
    mSectionImages = SEConsts::Section_Products;
}

ThreadLoadUrlImages::~ThreadLoadUrlImages()
{
    delete mListUrl;
}

QStringList *ThreadLoadUrlImages::listUrl()
{
    return mListUrl;
}

void ThreadLoadUrlImages::setSize(const QSize size)
{
    mSize = size;
}

void ThreadLoadUrlImages::setSectionImage(const int &section)
{
    mSectionImages = section;
}

void ThreadLoadUrlImages::onFinishedLoad(QNetworkReply *reply)
{
    mCountLoaded++;

    QString url = reply->url().toString();
    QString fileName = QMD5(url) + ".jpeg";
    QString filePathOrigin = SEConfig::getFolderImages(mSectionImages) + QDir::separator() + fileName;
    QString filePathPrevios = SEConfig::getFolderPrevImages(mSectionImages) + QDir::separator() + fileName;
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray jpegData = reply->readAll();
        QPixmap pixmap;
        if (jpegData.size()) {
            pixmap.loadFromData(jpegData);
            pixmap.save(filePathOrigin, "JPEG");
            pixmap = pixmap.scaled(mSize);
            pixmap.save(filePathPrevios, "JPEG");
        }
    }

    reply->deleteLater();

    if (mListUrl->size() <= mCountLoaded)
        emit finished();
}

void ThreadLoadUrlImages::process()
{
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    connect(manager, &QNetworkAccessManager::finished,
            this, &ThreadLoadUrlImages::onFinishedLoad, Qt::QueuedConnection);

    for (int i = 0; i < mListUrl->size(); ++i) {
        QUrl url;
        url.setUrl(mListUrl->at(i));
        QNetworkRequest request(url);
        manager->get(request);
    }
}

void ThreadLoadUrlImages::stop()
{
    emit finished();
}

