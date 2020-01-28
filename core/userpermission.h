#ifndef USERPERMISSION_H
#define USERPERMISSION_H

#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QMap>

class UserPermission
{
public:
    static const QString CONTACTS;
    static const QString ORDERS;
    static const QString PRODUCTS;
    static const QString PAYMENTS;
    static const QString PAYSYSTEMS;
    static const QString NEWS;
    static const QString SETTINGS;
    static const QString DELIVERIES;
    static const QString MAILS;
    static const QString CURRENCIES;
    static const QString REVIEWS;
    static const QString COMMENTS;
    static const QString IMAGES;

    static bool isAdmin;

    static void init(const QJsonArray &objects);
    static void setIsAdmin();

    static bool isRead(const QString &objectName);
    static bool isAdd(const QString &objectName);
    static bool isEdit(const QString &objectName);
    static bool isDelete(const QString &objectName);

    static void reset();

private:
    static QMap<QString, int> permissions;


};

#endif // USERPERMISSION_H
