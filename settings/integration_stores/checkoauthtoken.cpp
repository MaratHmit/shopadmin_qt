#include "checkoauthtoken.h"

#include <QEventLoop>
#include <QTimer>

CheckOAuthToken::CheckOAuthToken(QObject *parent):
    QObject(parent)
{

}

void CheckOAuthToken::stop()
{    
    mIsStop = true;
}

void CheckOAuthToken::process()
{
    Api api;
    ListDataItems items;
    mIsStop = false;

    while (!mIsStop) {
       api.getListIntegrationsServices(&items);
       if (items.size() && !items.at(0)->property("token").toString().isEmpty())
            break;
       thread()->sleep(1);
    }    
    emit finished();
}

