#ifndef SECONFIG_H
#define SECONFIG_H

#include <QString>
#include <QTableWidget>
#include <QStandardPaths>
#include <QHeaderView>
#include <QTreeWidget>
#include <QPalette>
#include <QColor>
#include <QSplitter>
#include <QDate>
#include <QLineEdit>
#include <QDoubleSpinBox>

#include "seutils.h"
#include "datamain.h"
#include "datatypedelivery.h"
#include "dataintegration.h"

class SEConfig
{  
private:
    static DataMain *dataMain;

public:
    SEConfig();

    static Qt::HANDLE idThreadUi;

    static QString apiUrl;    
    static QString idAdminProject;
    static QString projectAlias;
    static QString projectName;
    static QString projectHostName;
    static QString userDisplayName;
    static QString idCurrentUser;    
    static bool isUserAdmin;
    static QString serial;
    static QString keyHash;
    static QString token;
    static QString idSession;
    static QString login;
    static QString password;    
    static QString langApp;               
    static bool isExtHosting;
    static QString coreVersion;
    static int apiBuild;
    static QString yandexApi;
    static QString yandexToken;
    static QString yandexLogin;
    static bool useCache;
    static bool useWYSIWYG;

    static QString proxyHost;
    static QString proxyLogin;
    static QString proxyPassword;
    static int proxyPort;

    static QStringList dbTables;

    static bool isProxyConnected;

    static bool isFirstStart;

    static void setIsDebugMode(const bool &isDebugMode);

    static void loadSettings();
    static void saveSettings();
    static void getKeysProject(const QString &projectAlias, QString *serial, QString *keyHash);
    static void loadAccountSettings();
    static void saveAccountSettings(bool isSavePassword);    
    static void createHomeFolder();
    static void createProjectsFolder();
    static void createProjectFolder();
    static void saveProxySettings();
    static void regAutoStart();

    static DataMain *getMainInfo();
    static const QString priceCurrency(const qreal &price);
    static const QString priceCurrency(const qreal &price, const QString &code);

    static QString getSectionImage(const int &section);

    static bool createDir(const QString &dirName);
    static QString getFolderDocs();
    static QString getFolderExport();
    static QString getFolderCache();
    static QString getFolderHome();
    static QString getFolderDumps();
    static QString folderCurrentProject();
    static QString getFolderProjects();
    static QString getFolderUIstates();
    static QString getFolderImages(const int &section);    

    static QString apiProject();
    static QString apiAuth();    
    static QString getUrlImages(const int &section, const bool &isLocalPathMode = 0);    

    static void setPaletteReadOnly(QPalette *pal);

    static void saveTableState(QHeaderView *header, QString name, const int &tag = 0);
    static bool restoreTableState(QHeaderView *header, QString name, const int &tag = 0);
    static void saveSplitterLayout(QSplitter *splitter, const int &tag = 0);
    static bool loadSplitterLayout(QSplitter *splitter, const int &tag = 0);
    static void saveStateGeometry(const QWidget *widget);
    static void restoreStateGeometry(QWidget *widget);

    static QString getApiProjectController(const QString &apiName, const QString &apiMethod);
    static QString getApiAuthController(const QString &apiName, const QString &apiMethod);

    static void saveStateModel(QTreeView *view, const QString name = QString());
    static void restoreStateModel(QTreeView *view, const QString name = QString());

    static const ListDataItems *getOrdersStatuses();
    static const ListDataItems *getDeliveriesStatuses();
    static const ListDataItems *getFeaturesTypes();
    static const ListDataItems *getModificationsGroupsPricesTypes();
    static const ListTypesDeliveries *getTypesDeliveries();
    static const ListDataItems *getCountries();
    static const ListDataItems *getRegions();
    static ListIntegrations *getShopParameters();

    static double priceConvertToBaseCurrency(const double &price, const QString &codeCurrency);
    static void setSufficPrefixSpinBox(QDoubleSpinBox *spinBox, const ListCurrencies *listCurrencies,
                                       const QString &codeCurrency);
    static void setSufficPrefixLineEdit(QLineEdit *lineEdit, double &value,
                                        const ListCurrencies *listCurrencies,
                                        const QString &codeCurrency);

    static void debug(const QString &tag, const QString &info = QString());
    static void debug(const QByteArray &info = QByteArray());
    static void createDebugLogFile();

    static void reset();

private:
    static QString fileLogDebug;
    static bool isDebugMode;
    static ListDataItems *mShops;
    static ListDataItems *ordersStatuses;
    static ListDataItems *deliveriesStatuses;
    static ListDataItems *featuresTypes;
    static ListDataItems *modificationsGroupsPricesTypes;
    static ListTypesDeliveries *typesDeliveries;
    static ListDataItems *countries;
    static ListDataItems *regions;
    static ListIntegrations *shopParameters;    

};

#endif // SECONFIG_H
