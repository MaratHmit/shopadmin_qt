#ifndef CHECKOAUTHTOKEN_H
#define CHECKOAUTHTOKEN_H

#include <QObject>
#include <QThread>

#include "apiadapter.h"
#include "dataitem.h"

class CheckOAuthToken : public QObject
{
    Q_OBJECT

public:
    CheckOAuthToken(QObject *parent = 0);

signals:
    void finished();

public slots:
    void stop();
    void process();

private:
    bool mIsStop;

};

#endif // CHECKOAUTHTOKEN_H
