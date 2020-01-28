#include "userpermission.h"

#include <QDebug>

const QString UserPermission::CONTACTS   = "contacts";
const QString UserPermission::ORDERS     = "orders";
const QString UserPermission::PRODUCTS   = "products";
const QString UserPermission::PAYSYSTEMS = "paysystems";
const QString UserPermission::PAYMENTS   = "payments";
const QString UserPermission::NEWS       = "news";
const QString UserPermission::SETTINGS   = "settings";
const QString UserPermission::DELIVERIES = "deliveries";
const QString UserPermission::MAILS      = "mails";
const QString UserPermission::CURRENCIES = "currencies";
const QString UserPermission::REVIEWS    = "reviews";
const QString UserPermission::COMMENTS   = "comments";
const QString UserPermission::IMAGES     = "images";

bool UserPermission::isAdmin = false;
QMap<QString, int> UserPermission::permissions;


void UserPermission::init(const QJsonArray &objects)
{
    foreach (const QJsonValue& value, objects) {
        QJsonObject jsobObject = value.toObject();
        permissions[jsobObject["code"].toString()] = jsobObject["mask"].toInt();
    }
}

void UserPermission::setIsAdmin()
{
    isAdmin = true;
}

bool UserPermission::isRead(const QString &objectName)
{
    return (permissions[objectName] & 8) || isAdmin;
}

bool UserPermission::isAdd(const QString &objectName)
{   
    return (permissions[objectName] & 4) || isAdmin;
}

bool UserPermission::isEdit(const QString &objectName)
{
    return (permissions[objectName] & 2) || isAdmin;
}

bool UserPermission::isDelete(const QString &objectName)
{
    return (permissions[objectName] & 1) || isAdmin;
}

void UserPermission::reset()
{
    isAdmin = false;
}
