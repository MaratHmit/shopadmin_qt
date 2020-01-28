#ifndef THREADUPLOADIMAGE_H
#define THREADUPLOADIMAGE_H

#include <QStringList>
#include <QThread>
#include <QMap>

#include "dataitem.h"

class QNetworkReply;

class FileWebUploader : public QObject
{
    Q_OBJECT

public:
    explicit FileWebUploader(QObject *parent = 0);
    ~FileWebUploader();

    QStringList *listFiles();
    void setSection(const int &section);
    void setImageMode(const bool isImageMode);

signals:
    void uploadBytes(const int &count);    
    void uploadFinished(const QJsonObject object);
    void finished();

public slots:
    void stop();
    void process();

private slots:
    void onFinishedLoad(QNetworkReply* reply);
    void uploadProgress(qint64 bytesReceived, qint64 bytesTotal);

private:    
    QStringList *mListFiles;
    int mSection;
    qint64 mUploadedByte;
    int mUploadedFiles;    
    int mCountInHTTPMultiPart;
    bool isImageMode;

    QMap<QNetworkReply*, int> requests;
};

#endif // THREADUPLOADIMAGE_H
