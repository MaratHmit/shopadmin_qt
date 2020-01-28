#include "updater.h"
#include "seconsts.h"

#include <QEventLoop>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrl>
#include <QNetworkRequest>
#include <QDebug>
#include <QApplication>


Updater::Updater()
{

}

bool Updater::isHaveUpdate()
{
    QString script = "http://e-stile.ru/update/bug_fixed.php";

    QString build = QApplication::applicationVersion().split(".").at(2);
    script += "?cur_ver=" + build + "&typ_ver=" + SEConsts::UPDATE_TYPE_VERSION;

    QNetworkAccessManager *manager = new QNetworkAccessManager();
    QEventLoop *loop = new QEventLoop();    
    QObject::connect(manager, SIGNAL(finished(QNetworkReply*)), loop, SLOT(quit()));

    QUrl url;
    url.setUrl(script);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    QNetworkReply *reply = manager->get(request);
    loop->exec();
    delete loop;

    if (reply->error() == QNetworkReply::NoError) {        
        QString answer(reply->readAll());
        if (answer.isEmpty())
            return false;

        mNewVersion = answer;

        return true;
    }

    return false;
}

