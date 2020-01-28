#ifndef APIADAPTER_H
#define APIADAPTER_H

#include <QObject>

#include "api.h"
#include "qobject.h"
#include "request.h"
#include "dataitem.h"

class ApiAdapter : public QObject
{
    Q_OBJECT

public:    
    static Api *getApi();

private:
    static Api *api;
};

#endif // APIADAPTER_H
