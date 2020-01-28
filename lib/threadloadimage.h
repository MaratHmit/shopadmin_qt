#ifndef THREADSHOWIMAGE_H
#define THREADSHOWIMAGE_H

#include <QPixmap>
#include <QLabel>

#include <seconfig.h>
#include <seconsts.h>

class QNetworkReply;

class ThreadLoadImage : public QObject
{
    Q_OBJECT

public:
    explicit ThreadLoadImage(QObject *parent = 0);
    ~ThreadLoadImage();    

    QStringList *getListFiles();

    void setUrl(const QString &urlFolder);
    void setFolder(const QString &diskFolder);
    void setPreviewMode();
    void setImageSection(const int &section);

signals:
    void finished();
    void finishLoad(const QString &fileName, const int &index);    

public slots:
    void onFinishedLoad(QNetworkReply* reply);
    void process();
    void stop();

private:
    QStringList *listFiles;
    QString urlFolder;
    QString diskFolder;
    int countLoaded;
    int imageSection;
    bool isPreviewMode;
    QMap<QNetworkReply*, QString> requestsFilesMap;
};

#endif // THREADSHOWIMAGE_H
