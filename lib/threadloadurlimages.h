#ifndef THREADLOADURLIMAGES_H
#define THREADLOADURLIMAGES_H

#include <QSize>

class QNetworkReply;

#include "dataitem.h"

class ThreadLoadUrlImages: public QObject
{
    Q_OBJECT

public:
    explicit ThreadLoadUrlImages(QObject *parent = 0);
    ~ThreadLoadUrlImages();

    QStringList *listUrl();
    void setSize(const QSize size);
    void setSectionImage(const int &section);

signals:
    void finished();

public slots:
    void onFinishedLoad(QNetworkReply* reply);
    void process();
    void stop();

private:
    QStringList *mListUrl;
    QSize mSize;
    int mCountLoaded;
    int mSectionImages;

};

#endif // THREADLOADURLIMAGES_H
