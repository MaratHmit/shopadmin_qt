#ifndef THREADSHOWIMAGE_H
#define THREADSHOWIMAGE_H

#include <QPixmap>
#include <QLabel>

#include <seconfig.h>
#include <seconsts.h>

class QNetworkReply;

class FileWebLoader : public QObject
{
    Q_OBJECT

public:
    explicit FileWebLoader(const QString &urlImage, const QModelIndex &modelIndex = QModelIndex());

signals:    
    void finishedDownloadImage(const QString &urlImage, const QModelIndex &index);

public slots:
    void onFinishedLoad(QNetworkReply* reply);
    void process();

private:
    QString urlImage;
    QModelIndex modelIndex;

};

#endif // THREADSHOWIMAGE_H
