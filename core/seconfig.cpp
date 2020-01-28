#include "seconfig.h"

#include "qsettings.h"
#include "seconsts.h"
#include "apiadapter.h"
#include "nametablemodel.h"

#include <QDesktopServices>
#include <QDir>
#include <QFile>
#include <QSettings>
#include <QTextStream>
#include <QDebug>
#include <QWindow>
#include <QApplication>


Qt::HANDLE SEConfig::idThreadUi;
QString SEConfig::apiUrl;
QString SEConfig::idAdminProject;
QString SEConfig::projectAlias;
QString SEConfig::projectName;
QString SEConfig::projectHostName;
QString SEConfig::userDisplayName;
QString SEConfig::idCurrentUser;
bool SEConfig::isUserAdmin;
QString SEConfig::token;
QString SEConfig::idSession;
QString SEConfig::serial;
QString SEConfig::keyHash;
QString SEConfig::langApp;
QString SEConfig::login;
QString SEConfig::password;
QString SEConfig::proxyHost;
QString SEConfig::proxyLogin;
QString SEConfig::proxyPassword;
QString SEConfig::yandexApi;
QString SEConfig::yandexToken;
QString SEConfig::yandexLogin;
QString SEConfig::coreVersion = "5.3";
int SEConfig::apiBuild = 0;
int SEConfig::proxyPort = 80;
bool SEConfig::isFirstStart;
bool SEConfig::isProxyConnected = false;
bool SEConfig::isExtHosting = false;
bool SEConfig::useCache = true;
bool SEConfig::useWYSIWYG = true;
ListDataItems *SEConfig::ordersStatuses = 0;
ListDataItems *SEConfig::deliveriesStatuses = 0;
ListDataItems *SEConfig::featuresTypes = 0;
ListDataItems *SEConfig::modificationsGroupsPricesTypes = 0;
ListDataItems *SEConfig::countries = 0;
ListDataItems *SEConfig::regions = 0;
ListIntegrations *SEConfig::shopParameters = 0;
ListTypesDeliveries *SEConfig::typesDeliveries = 0;
QStringList SEConfig::dbTables;
DataMain *SEConfig::dataMain = 0;
bool SEConfig::isDebugMode = false;
QString SEConfig::fileLogDebug = "log.txt";

SEConfig::SEConfig()
{

}

void SEConfig::setIsDebugMode(const bool &isDebugMode)
{
    SEConfig::isDebugMode = isDebugMode;            
}

QString SEConfig::apiProject()
{
    return apiUrl;
}

QString SEConfig::apiAuth()
{
    return SEConsts::URL_AUTH;
}

void SEConfig::loadSettings()
{
    QSettings *settings = new QSettings();
    projectAlias = settings->value("/settings/defaultProject", "").toString();
    langApp = settings->value("/settings/defaultLangApp", "rus").toString();
    isProxyConnected = settings->value("/isProxyConnected", false).toBool();
    proxyHost = settings->value("/proxyHost", "").toString();
    proxyLogin = settings->value("/proxyLogin", "").toString();
    proxyPassword = settings->value("/proxyPassword", "").toString();
    proxyPort = settings->value("/proxyPort", proxyPort).toInt();
    isFirstStart = projectAlias.isEmpty();

    delete settings;    
}

void SEConfig::saveSettings()
{
    QSettings *settings = new QSettings();
    settings->setValue("/settings/defaultProject", projectAlias);
    settings->setValue("/settings/applicationVersion", QApplication::applicationVersion());
    settings->setValue("/settings/applicationDirPatch", QApplication::applicationDirPath());
    settings->sync(); //записываем настройки
    delete settings;
}

void SEConfig::getKeysProject(const QString &projectAlias, QString *serial, QString *keyHash)
{
    QFile fileAdmin(getFolderProjects() + QDir::separator() + projectAlias + QDir::separator() + "admin.dat");
    if (fileAdmin.exists() && fileAdmin.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QByteArray a = fileAdmin.readAll();
        a = getCryptoLatin(a);
        QString s;
        s.append(a);
        QStringList sl;
        sl = s.split("\t");
        if (sl.size() > 0)
            *serial = sl.at(0);
        if (sl.size() > 1)
            *keyHash = sl.at(1);
    }
}

void SEConfig::loadAccountSettings()
{    
    if (!projectAlias.isEmpty())
        getKeysProject(projectAlias, &SEConfig::serial, &SEConfig::keyHash);            
}

void SEConfig::saveAccountSettings(bool isSavePassword)
{
    QFile fileAdmin(folderCurrentProject() + QDir::separator() + SEConsts::FILE_ADMIN);

    if(fileAdmin.open(QIODevice::WriteOnly)) {
        QString s(serial);
        if (isSavePassword)
            s += "\t" + keyHash;
        QByteArray a(s.toLatin1());
        fileAdmin.write(setCryptoLatin(a));
    }
    fileAdmin.close();    
}

void SEConfig::createHomeFolder()
{
    createDir(getFolderHome());
    QFile file(QDir::currentPath() + QDir::separator() + fileLogDebug);
    if (file.exists())
        file.remove();
}

void SEConfig::createProjectsFolder()
{
    createDir(getFolderProjects());
}

void SEConfig::createProjectFolder()
{
    QString pathDir = getFolderProjects() +
            QDir::separator() + projectAlias;
    createDir(pathDir);

    // создание языковой папки проекта
    pathDir = folderCurrentProject() + QDir::separator();
    createDir(pathDir);

    // создание папки для файлов кэша
    createDir(getFolderCache());
}

void SEConfig::saveProxySettings()
{
    QSettings *settings = new QSettings();
    settings->setValue("/proxyHost", proxyHost);
    settings->setValue("/proxyLogin", proxyLogin);
    settings->setValue("/proxyPort", proxyPort);
    settings->setValue("/proxyPassword", proxyPassword);
    settings->setValue("/isProxyConnected", isProxyConnected);
    settings->sync();
    delete settings;
}

void SEConfig::regAutoStart()
{
    QSettings settings("HKEY_CURRENT_USER\\SOFTWARE\\Classes",
                       QSettings::NativeFormat);
    settings.setValue("/SEManager5.3/.", "URL:SEManager Protocol");
    settings.setValue("/SEManager5.3/URL Protocol", QString());
    settings.setValue("/SEManager5.3/DefaultIcon/.",
                      "\"" + QApplication::applicationFilePath().replace("/", "\\") + "\",0" );
    settings.setValue("/SEManager5.3/shell/open/command/.",
                      "\"" + QApplication::applicationFilePath().replace("/", "\\") + "\" \"%1\"");
    settings.sync();
}

DataMain *SEConfig::getMainInfo()
{    
    if (!dataMain) {
        dataMain = new DataMain();
        ApiAdapter::getApi()->getInfoMain(dataMain);
        if (!dataMain->property("tokenYandex").isNull()) {
            yandexApi = dataMain->property("apiYandex").toString();
            yandexToken = dataMain->property("tokenYandex").toString();
            yandexLogin = dataMain->property("loginYandex").toString();
        } else {
            yandexApi.clear();
            yandexToken.clear();
            yandexLogin.clear();
        }
        if (!dataMain->property("apiBuild").isNull())
            apiBuild = dataMain->property("apiBuild").toInt();
    }    
    return dataMain;
}

const QString SEConfig::priceCurrency(const qreal &price)
{
    if (!getMainInfo()->baseCurrencyPrefix().isEmpty())
        return getMainInfo()->baseCurrencyPrefix() + QString::number(price, 'f', 2);
    return QString::number(price, 'f', 2) + getMainInfo()->baseCurrencySuffix();
}

const QString SEConfig::priceCurrency(const qreal &price, const QString &code)
{
    QString prefix, suffix;
    for (int i = 0; i < getMainInfo()->listCurrency()->size(); i++)
        if (getMainInfo()->listCurrency()->at(i)->getCode() == code) {
            prefix = getMainInfo()->listCurrency()->at(i)->prefix();
            suffix = getMainInfo()->listCurrency()->at(i)->suffix();
            break;
        }
    if (!prefix.isEmpty())
        return prefix + QString::number(price, 'f', 2);
    return QString::number(price, 'f', 2) + suffix;
}

QString SEConfig::getSectionImage(const int &section)
{
    switch (section) {
    case 0: return SEConsts::DIR_IMAGES_PRODUCTS;
    case 1: return SEConsts::DIR_IMAGES_GROUPSPRODUCTS;
    case 2: return SEConsts::DIR_IMAGES_NEWS;
    case 3: return SEConsts::DIR_IMAGES_BRANDS;
    case 4: return SEConsts::DIR_IMAGES_FEATURES;
    case 5: return SEConsts::DIR_IMAGES_PAYMENT;
    case 6: return SEConsts::DIR_IMAGES_CONTACTS;
    case 7: return SEConsts::DIR_IMAGES_SHOPSECTIONS;
    case 8: return SEConsts::DIR_IMAGES_YANDEXPHOTOS;
    case 9: return SEConsts::DIR_IMAGES_LABELS;
    }

    return QString();
}

bool SEConfig::createDir(const QString &dirName)
{
    QDir dir;
    if (dir.exists(dirName))
        return true;
    return dir.mkpath(dirName);
}

QString SEConfig::getFolderDocs()
{
    QStringList listDirs =
            QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation);


    if (listDirs.size() == 0)
        return folderCurrentProject();

    return listDirs.at(0);
}

QString SEConfig::getFolderExport()
{
    return folderCurrentProject() + QDir::separator() + SEConsts::DIR_EXPORTS;
}

QString SEConfig::getFolderCache()
{
    return folderCurrentProject() + QDir::separator() + SEConsts::DIR_CACHE;
}

QString SEConfig::getFolderHome()
{
    QStringList listDirs =
            QStandardPaths::standardLocations(QStandardPaths::DataLocation);


    if (listDirs.size() == 0)
        listDirs.append(QDir::homePath());
    if (listDirs.at(0).trimmed().isEmpty())
        listDirs[0] = QDir::homePath();

    return listDirs.at(0);
}

QString SEConfig::getFolderDumps()
{
    return getFolderProjects() + QDir::separator() + SEConsts::DIR_DUMPS;
}

QString SEConfig::folderCurrentProject()
{
    return getFolderProjects() + QDir::separator() + projectAlias;
}

QString SEConfig::getFolderProjects()
{
    return getFolderHome() + QDir::separator() + SEConsts::DIR_PROJECTS;
}

QString SEConfig::getFolderUIstates()
{
    return folderCurrentProject() + QDir::separator() + SEConsts::DIR_STATESWIDGETS;
}

QString SEConfig::getFolderImages(const int &section)
{
    QString folder;
    folder = folderCurrentProject() + QDir::separator() + SEConsts::DIR_IMAGES;

    switch (section) {
    case 0:
        return folder + QDir::separator() + SEConsts::DIR_IMAGES_PRODUCTS;
    case 1:
        return folder + QDir::separator() + SEConsts::DIR_IMAGES_GROUPSPRODUCTS;
    case 2:
        return folder + QDir::separator() + SEConsts::DIR_IMAGES_NEWS;
    case 3:
        return folder + QDir::separator() + SEConsts::DIR_IMAGES_BRANDS;
    case 4:
        return folder + QDir::separator() + SEConsts::DIR_IMAGES_FEATURES;
    case 5:
        return folder + QDir::separator() + SEConsts::DIR_IMAGES_PAYMENT;
    case 6:
        return folder + QDir::separator() + SEConsts::DIR_IMAGES_CONTACTS;
    case 7:
        return folder + QDir::separator() + SEConsts::DIR_IMAGES_SHOPSECTIONS;
    case 8:
        return folder + QDir::separator() + SEConsts::DIR_IMAGES_YANDEXPHOTOS;
    case 9:
        return folder + QDir::separator() + SEConsts::DIR_IMAGES_LABELS;

    }
    return  QString();
}

QString SEConfig::getUrlImages(const int &section, const bool &isLocalPathMode)
{
    QString url = "/" + SEConsts::DIR_IMAGES + "/rus/";
    if (!isLocalPathMode)
        url = "http://" + projectHostName + url;

    switch (section) {
    case 0:
        return url + SEConsts::DIR_IMAGES_PRODUCTS;
    case 1:
        return url + SEConsts::DIR_IMAGES_GROUPSPRODUCTS;
    case 2:
        return url + SEConsts::DIR_IMAGES_NEWS;
    case 3:
        return url + SEConsts::DIR_IMAGES_BRANDS;
    case 4:
        return url + SEConsts::DIR_IMAGES_FEATURES;
    case 5:
        return url + SEConsts::DIR_IMAGES_PAYMENT;
    case 6:
        return url + SEConsts::DIR_IMAGES_CONTACTS;
    case 7:
        return url + SEConsts::DIR_IMAGES_SHOPSECTIONS;
    case 8:
        return url + SEConsts::DIR_IMAGES_YANDEXPHOTOS;
    case 9:
        return url + SEConsts::DIR_IMAGES_LABELS;
    }
    return QString();
}

void SEConfig::setPaletteReadOnly(QPalette *pal)
{
    pal->setColor(QPalette::Base, QColor(255, 255, 220));
}

void SEConfig::saveTableState(QHeaderView *header, QString name,
                                     const int &tag)
{
    QString fileName = getFolderUIstates() + QDir::separator() + name;
    if (tag)
        fileName += QString::number(tag);
    createDir(getFolderUIstates());
    QFile fileStates(fileName);
     if(fileStates.open(QIODevice::WriteOnly)) {
         fileStates.write(header->saveState());
         fileStates.close();
     }
}

bool SEConfig::restoreTableState(QHeaderView *header, QString name,
                                     const int &tag)
{
    createDir(getFolderUIstates());
    QString fileName = getFolderUIstates() + QDir::separator() + name;
    if (tag)
        fileName += QString::number(tag);
    QFile fileStates(fileName);
    if(fileStates.exists() &&
            fileStates.open(QIODevice::ReadOnly)) {
        QByteArray a(fileStates.readAll());
        header->restoreState(a);
        fileStates.close();
        return true;
    }
    return false;
}

void SEConfig::saveSplitterLayout(QSplitter *splitter, const int &tag)
{
    createDir(getFolderUIstates());
    QString fileName = getFolderUIstates() + QDir::separator() + splitter->objectName();
    if (tag)
        fileName += QString::number(tag);
    QFile fileStates(fileName);
    if(fileStates.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QString s("");
        for (int i = 0; i < splitter->sizes().size(); i++)
            s += QString::number(splitter->sizes().at(i)) + "\n";
        QByteArray a(s.toLatin1());
        fileStates.write(a);
        fileStates.close();
    }
}

bool SEConfig::loadSplitterLayout(QSplitter *splitter, const int &tag)
{
    QString fileName = getFolderUIstates() + QDir::separator() + splitter->objectName();
    if (tag)
        fileName += QString::number(tag);
    QFile fileStates(fileName);
    if(fileStates.exists() && fileStates.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QByteArray a(fileStates.readAll());
        QString s;
        s.append(a);
        QStringList sl;
        sl = s.split("\n");
        int size;
        QList<int> listSizes;
        for (int i = 0; i <  splitter->sizes().size(); i++) {
            size = sl.at(i).toInt();
            if (size < 50)
                size = 50;
            listSizes.append(size);
        }
        splitter->setSizes(listSizes);
        fileStates.close();
        return true;
    }
    return false;
}

void SEConfig::saveStateGeometry(const QWidget *widget)
{
    QSettings *settings = new QSettings();
    settings->setValue(widget->metaObject()->className(), widget->saveGeometry());
    settings->sync(); //записываем настройки
    delete settings;
}

void SEConfig::restoreStateGeometry(QWidget *widget)
{
    QSettings *settings = new QSettings();
    widget->restoreGeometry(
                settings->value(
                    widget->metaObject()->className(),
                    widget->saveGeometry()).toByteArray());
    delete settings;
}

QString SEConfig::getApiProjectController(const QString &apiName, const QString &apiMethod)
{
    if (isDebugMode && !isExtHosting)
        return apiProject() +  "/" + SEConsts::API_PATH_DEVELOPMENT + "/" + apiName + "/" + apiMethod;
    return apiProject() +  "/" + SEConsts::API_PATH + "/" + apiName + "/" + apiMethod;
}

QString SEConfig::getApiAuthController(const QString &apiName, const QString &apiMethod)
{
    return apiAuth() +  "/" + SEConsts::API_PATH + "/" + apiName + "/" + apiMethod;
}

void SEConfig::saveStateModel(QTreeView *view, const QString name)
{
    QString settingsName = name.isEmpty() ? view->objectName() : name;
    QStringList listExpandeds;
    NameTableModel *model = dynamic_cast <NameTableModel *> (view->model());       

    foreach (QModelIndex index, model->indexList())
    {
        if (view->isExpanded(index))
            listExpandeds << index.data(Qt::UserRole).toString();
    }
    QStringList listSelecteds;
    QModelIndexList list = view->selectionModel()->selectedRows();
    foreach (QModelIndex index, list)
        listSelecteds << index.data(Qt::UserRole).toString();

    QSettings *settings = new QSettings();
    settings->beginGroup(QMD5(settingsName + view->parent()->objectName()));
    if (!listExpandeds.isEmpty())
        settings->setValue("ExpandedItems", QVariant::fromValue(listExpandeds));
    if (!listSelecteds.isEmpty())
        settings->setValue("SelectedItems", QVariant::fromValue(listSelecteds));
    settings->endGroup();
    settings->sync();
    delete settings;
}

void SEConfig::restoreStateModel(QTreeView *view, const QString name)
{
    QString settingsName = name.isEmpty() ? view->objectName() : name;
    QStringList listExpandeds;
    QStringList listSelecteds;
    NameTableModel *model = dynamic_cast <NameTableModel *> (view->model());

    QSettings *settings = new QSettings();
    settings->beginGroup(QMD5(settingsName + view->parent()->objectName()));
    listExpandeds = settings->value("ExpandedItems").toStringList();
    listSelecteds = settings->value("SelectedItems").toStringList();
    settings->endGroup();

    const ListDataItems *itemsData = model->getItems();

    for (int i = 0; i < itemsData->size(); ++i) {
        foreach (QString item, listExpandeds) {
            if (item.compare(itemsData->at(i)->getId()) == 0) {
                QModelIndex index = model->getModelIndexById(itemsData->at(i)->getId());
                view->setExpanded(index, true);
            }
        }
        foreach (QString item, listSelecteds) {
            if (item.compare(itemsData->at(i)->getId()) == 0) {
                QModelIndex index = model->getModelIndexById(itemsData->at(i)->getId());
                if (view->selectionBehavior() == QAbstractItemView::SelectRows)
                    view->selectionModel()->setCurrentIndex(index, QItemSelectionModel::Select |
                                                            QItemSelectionModel::Rows);
                else view->selectionModel()->setCurrentIndex(index, QItemSelectionModel::Select);
            }
        }
    }

    delete settings;
}

const ListDataItems *SEConfig::getOrdersStatuses()
{
    if (!ordersStatuses) {
        ordersStatuses = new ListDataItems();
        ApiAdapter::getApi()->getListOrdersStatuses(ordersStatuses);
    }
    return ordersStatuses;
}

const ListDataItems *SEConfig::getDeliveriesStatuses()
{
    if (!deliveriesStatuses) {
        deliveriesStatuses = new ListDataItems();
        ApiAdapter::getApi()->getListDeliveriesStatuses(deliveriesStatuses);
    }
    return deliveriesStatuses;
}

const ListDataItems *SEConfig::getFeaturesTypes()
{
    if (!featuresTypes) {
        featuresTypes = new ListDataItems();
        ApiAdapter::getApi()->getListFeaturesTypes(Request(), featuresTypes);
    }
    return featuresTypes;
}

const ListDataItems *SEConfig::getModificationsGroupsPricesTypes()
{
    if (!modificationsGroupsPricesTypes) {
        modificationsGroupsPricesTypes = new ListDataItems();
        ApiAdapter::getApi()->getListModificationsPricesTypes(modificationsGroupsPricesTypes);
    }
    return modificationsGroupsPricesTypes;
}

const ListTypesDeliveries *SEConfig::getTypesDeliveries()
{
    if (!typesDeliveries) {
        typesDeliveries = new ListTypesDeliveries();
        ApiAdapter::getApi()->getListTypesDeliveries(typesDeliveries);
    }
    return typesDeliveries;
}

const ListDataItems *SEConfig::getCountries()
{
    if (!countries) {
        countries = new ListDataItems();
        if (!ApiAdapter::getApi()->getListCountries(countries)) {
            delete countries;
            countries = 0;
        }
    }
    return countries;
}

const ListDataItems *SEConfig::getRegions()
{
    if (!regions) {
        regions = new ListDataItems();
        if (!ApiAdapter::getApi()->getListRegions(regions)) {
            delete regions;
            regions = 0;
        }
    }
    return regions;
}

ListIntegrations *SEConfig::getShopParameters()
{
    if (!shopParameters) {
        shopParameters = new ListIntegrations();
        if (!ApiAdapter::getApi()->getListShopParameters(shopParameters)) {
            delete shopParameters;
            shopParameters = 0;
        }
    }
    return shopParameters;
}

double SEConfig::priceConvertToBaseCurrency(const double &price, const QString &codeCurrency)
{
    if (codeCurrency == SEConfig::getMainInfo()->baseCurrency())
        return price;

    ListCurrencies *currencies = new ListCurrencies();
    if (ApiAdapter::getApi()->getListCurrencies(currencies)) {
        for (int i = 0; i < currencies->size(); i++) {
            if (currencies->at(i)->getCode() == codeCurrency) {
                double answer = currencies->at(i)->rate() * price;
                delete currencies;
                return answer;
            }
        }
    }
    delete currencies;
    return price;
}

void SEConfig::setSufficPrefixSpinBox(QDoubleSpinBox *spinBox, const ListCurrencies *listCurrencies,
                                      const QString &codeCurrency)
{
    for (int i = 0; i < listCurrencies->size(); ++i) {
        if (listCurrencies->at(i)->getCode() == codeCurrency) {
            const DataCurrency *curr = listCurrencies->at(i);
            spinBox->setSuffix(curr->suffix());
            spinBox->setPrefix(curr->prefix());
            break;
        }
    }
}

void SEConfig::setSufficPrefixLineEdit(QLineEdit *lineEdit, double &value,
                                       const ListCurrencies *listCurrencies,
                                       const QString &codeCurrency)
{
    for (int i = 0; i < listCurrencies->size(); ++i) {
        if (listCurrencies->at(i)->getCode() == codeCurrency) {
            const DataCurrency *curr = listCurrencies->at(i);
            lineEdit->setText(curr->prefix() + QString::number(value, 'f', 2) + curr->suffix());
            break;
        }
    }
}

void SEConfig::debug(const QString &tag, const QString &info)
{
    QFile file(QDir::currentPath() + QDir::separator() + fileLogDebug);
    if (file.open(QIODevice::Append)) {
        QString str;
        str = tag + ": " + info + "\n";
        QByteArray data;
        data.append(str);
        if (isDebugMode)
            qDebug() << data;
        file.write(data);
        file.close();
    }
}

void SEConfig::debug(const QByteArray &info)
{
    QFile file(QDir::currentPath() + QDir::separator() + fileLogDebug);
    if (file.open(QIODevice::Append)) {                
        if (isDebugMode)
            qDebug() << info;
        file.write(info);
        file.close();
    }
}

void SEConfig::reset()
{
    ordersStatuses = 0;
    deliveriesStatuses = 0;
    featuresTypes = 0;
    modificationsGroupsPricesTypes = 0;
    typesDeliveries = 0;
    dataMain = 0;
    shopParameters = 0;
    apiBuild = 0;
}

void SEConfig::createDebugLogFile()
{
    QFile file(QDir::currentPath() + QDir::separator() + fileLogDebug);
    if (file.exists())
        file.remove();
    if (file.open(QIODevice::WriteOnly)) {
        QByteArray data;
        data.append("Debug start: ");
        data.append(QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss"));
        file.write(data);
        file.close();
    }
}
