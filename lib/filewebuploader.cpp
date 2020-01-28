#include "filewebuploader.h"

#include <QEventLoop>
#include <QNetworkReply>
#include <QUrl>
#include <QNetworkRequest>
#include <QtGuiDepends>
#include <QImageReader>
#include <QMessageBox>
#include <QHttpPart>
#include <QHttpMultiPart>

#include <seconfig.h>
#include <seconsts.h>


FileWebUploader::FileWebUploader(QObject *parent):
    QObject(parent)
{
    mListFiles = new QStringList;    
    mCountInHTTPMultiPart = 3;  // количество отправки файлов за раз
    mUploadedByte = 0;
    mUploadedFiles = 0;    
    isImageMode = true;
}

FileWebUploader::~FileWebUploader()
{
    delete mListFiles;
}

void FileWebUploader::process()
{
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(onFinishedLoad(QNetworkReply*)), Qt::QueuedConnection);

    QString apiObject = isImageMode ? SEConsts::API_FILESIMAGES : SEConsts::API_FILES;
    QString apiScriptMain = SEConfig::getApiProjectController(apiObject, SEConsts::APIMETHOD_UPLOAD);
    QString apiScript;

    int i = 0;
    int countRemaind = mListFiles->size();
    int countSentFiles = 0;

    while (countRemaind > 0) {

        if (countRemaind > mCountInHTTPMultiPart) {
            countSentFiles = mCountInHTTPMultiPart;
            countRemaind -= mCountInHTTPMultiPart;
        } else {
            countSentFiles = countRemaind;
            countRemaind = 0;
        }

        apiScript = apiScriptMain + "?&section=" + SEConfig::getSectionImage(mSection) +
                "&count=" + QString::number(countSentFiles);

        QHttpMultiPart *multiPart=
               new QHttpMultiPart(QHttpMultiPart::FormDataType);

        for (int j = i; j < i + countSentFiles; j++) {
            QString fileName = mListFiles->at(j);

            if (QFileInfo(fileName).size() > 0) {
                QHttpPart imagePart;
                if (isImageMode)
                    imagePart.setHeader(QNetworkRequest::ContentTypeHeader,
                                        QVariant("image/jpeg"));
                else imagePart.setHeader(QNetworkRequest::ContentTypeHeader,
                                         QVariant("application/pdf"));
                imagePart.setHeader(QNetworkRequest::ContentDispositionHeader,
                        QVariant("form-data; name=\"file" +
                                 QString::number(j - i) + "\"; filename=\"" +
                                 QFileInfo(fileName).fileName().toHtmlEscaped() + "\""));

                QFile *file = new QFile(fileName);
                if(!file->open(QIODevice::ReadOnly))
                {
                    QMessageBox::critical(0, tr("Ошибка чтения файла"),
                                          tr("Не удаётся прочитать файл") + ": " +
                                          fileName + "!");
                    return;
                }
                imagePart.setBodyDevice(file);
                multiPart->append(imagePart);
                file->setParent(multiPart);
            }
        }

        i += countSentFiles;

        QUrl url(apiScript);
        QNetworkRequest request(url);

        QByteArray header;
        header.append("Token");
        QByteArray value;
        value.append(SEConfig::token);
        request.setRawHeader(header, value);
        header.clear();
        header.append("Secookie");
        value.clear();
        value.append(SEConfig::idSession);
        request.setRawHeader(header, value);

        QNetworkReply * reply = manager->post(request, multiPart);
        multiPart->setParent(reply);
        connect(reply, SIGNAL(uploadProgress(qint64,qint64)),
                SLOT(uploadProgress(qint64,qint64)));
        requests.insert(reply, countSentFiles);
    }    
}

QStringList *FileWebUploader::listFiles()
{
    return mListFiles;
}

void FileWebUploader::setSection(const int &section)
{
    mSection = section;
}

void FileWebUploader::setImageMode(const bool isImageMode)
{
    this->isImageMode = isImageMode;
}

void FileWebUploader::stop()
{
    emit finished();
}

void FileWebUploader::onFinishedLoad(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray bytes;
        bytes = reply->readAll();

        SEConfig::debug("Answer from server:", QString());
        SEConfig::debug(bytes);
        QJsonDocument json;
        if(json.fromJson(bytes).object()["status"].toString() == "ok") {
            if(json.fromJson(bytes).object().value("data").isObject()) {
                QJsonObject object = json.fromJson(bytes).object()["data"].toObject();
                if (object["items"].isArray()) {
                    QJsonArray jsonItems = object["items"].toArray();
                    for (int i = 0; i < jsonItems.size(); ++i)
                        emit uploadFinished(jsonItems.at(i).toObject());
                }
            }
        }

        mUploadedFiles += requests.value(reply);
        if (mUploadedFiles == mListFiles->size())
            emit finished();
    }

    reply->deleteLater();
}

void FileWebUploader::uploadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    Q_UNUSED(bytesTotal);

    mUploadedByte += bytesReceived;
    emit uploadBytes(mUploadedByte);
}
