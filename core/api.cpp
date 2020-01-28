#include "api.h"

#include <QEventLoop>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrl>
#include <QNetworkRequest>
#include <QtGuiDepends>
#include <QDebug>
#include <QHttpPart>
#include <QXmlStreamReader>
#include <QNetworkProxy>
#include <QThread>

#include "seutils.h"
#include "seconfig.h"
#include "dialogproxysettings.h"
#include "userpermission.h"

bool Api::isCompressed;

bool Api::auth()
{
    if (SEConfig::isProxyConnected)
        setProxyParams();

    QJsonObject obj;    
    obj.insert("login", QJsonValue(SEConfig::serial));
    obj.insert("password", QJsonValue(SEConfig::keyHash));   

    SEConfig::idSession = QMD5(QTime::currentTime().toString());

    QJsonDocument doc;
    doc.setObject(obj);
    QString params(doc.toJson(QJsonDocument::Compact));
    QJsonObject object;
    bool result = executeAPI(SEConsts::API_AUTH, SEConsts::APIMETHOD_INFO,
                             params, &object);
    if (result) {
        UserPermission::reset();
        SEConfig::coreVersion = object["coreVersion"].toString();
        SEConfig::projectName = object["hostname"].toString();
        SEConfig::projectHostName = object["hostname"].toString();       
        SEConfig::userDisplayName = object["userDisplay"].toString();
        SEConfig::idCurrentUser = object["idUser"].toString();
        SEConfig::isUserAdmin = object["isUserAdmin"].toBool();
        if (!object["useWysiwyg"].isNull())
            SEConfig::useWYSIWYG = object["useWysiwyg"].toBool();
        SEConfig::createProjectFolder();
        SEConfig::dbTables.clear();
        if (object["tables"].isArray()) {
            QJsonArray tables = object["tables"].toArray();
            for (int i = 0; i < tables.size(); ++i)
                SEConfig::dbTables.append(tables[i].toString());
        }
        if (SEConfig::idCurrentUser == "admin") {
            SEConfig::userDisplayName = tr("Администратор");
            UserPermission::setIsAdmin();
        } else
            UserPermission::init(object["permission"].toArray());
        return true;
    }

    return false;
}

bool Api::registration(const QString &serial, const QString &hash, const QString &project,
                       const bool &isCheckedMode)
{
    if (SEConfig::isProxyConnected)
        setProxyParams();

    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(proxyAuthenticationRequired(QNetworkProxy,QAuthenticator*)),
            SLOT(onProxyAuthentication(QNetworkProxy,QAuthenticator*)));

    QEventLoop *loop = new QEventLoop();
    connect(manager, SIGNAL(finished(QNetworkReply*)),
            loop, SLOT(quit()));

    QUrl url;
    QString s = SEConfig::getApiAuthController(SEConsts::API_AUTH, SEConsts::APIMETHOD_REGISTER);
    url.setUrl(s);

    QJsonObject obj;
    if (!isCheckedMode)
        obj.insert("project", QJsonValue(project));
    obj.insert("serial", QJsonValue(serial));
    obj.insert("hash", QJsonValue(hash));
    QJsonDocument doc;
    doc.setObject(obj);

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QByteArray strPost(doc.toJson(QJsonDocument::Compact));

    QNetworkReply *reply = manager->post(request, strPost);
    loop->exec();
    delete loop;

    if (reply->error() == QNetworkReply::NoError)
    {
        QByteArray bytes;
        bytes = reply->readAll();
        QJsonDocument json;
        if(json.fromJson(bytes).object()["status"].toString() == "ok") {
            if (!isCheckedMode) {
                SEConfig::projectName = json.fromJson(bytes).object()["data"].toObject()["project"].toString();
                SEConfig::idAdminProject = json.fromJson(bytes).object()["data"].toObject()["idAdminProject"].toString();
                SEConfig::token = json.fromJson(bytes).object()["data"].toObject()["token"].toString();
                SEConfig::apiUrl = json.fromJson(bytes).object()["data"].toObject()["uri"].toString();
                SEConfig::isExtHosting = json.fromJson(bytes).object()["data"].toObject()["isExt"].toBool();
                SEConfig::coreVersion = json.fromJson(bytes).object()["data"].toObject()["version"].toString();
            }
            delete reply;            
            return true;
        } else {
            QString error = json.fromJson(bytes).object()["data"].toObject()["error"].toString();
            error = error.isEmpty() ? tr("Не удаётся распознать ответ сервера авторизации") + "!" :
                                      error;
            QMessageBox::critical(0, tr("Ошибка авторизации"), error);
        }
    }
    else
        QMessageBox::critical(0, tr("Ошибка подключения"),
                              tr("Не удаётся подключиться к серверу авторизации") + "!");

    delete reply;
    return false;
}

void Api::setApiRequestParams(const QString &apiObject, const QString &apiMethod,
                                const QString &apiRequestParams)
{
    this->apiObject = apiObject;
    this->apiMethod = apiMethod;
    this->apiRequestParams = apiRequestParams;
}

bool Api::save(const QString &apiName, DataItem *data,
              const bool &isBlockingMode)
{
    if (!data->isModified())
        return true;

    QJsonObject object;
    bool result = executeAPI(apiName, SEConsts::APIMETHOD_SAVE,
                             data->getAsJsonString(), &object, isBlockingMode);

    if (result && (!object["id"].isNull() && !object["id"].toString().isEmpty())) {
        data->setId(object["id"].toString());
        data->setUnModified();
    } else result = false;
    return result;
}

bool Api::saveContact(DataContact *contact)
{
    return save(SEConsts::API_CONTACTS, contact);
}

bool Api::saveContactGroup(DataItem *group)
{
    return save(SEConsts::API_GROUPSCONTACTS, group);
}

bool Api::saveProductGroup(DataProductsGroup *group)
{
    return save(SEConsts::API_PRODUCTS_GROUPS, group);
}

bool Api::saveBrand(DataItem *brand)
{
    return save(SEConsts::API_BRANDSPRODUCTS, brand);
}

bool Api::saveFeatureGroup(DataItem *group)
{
    return save(SEConsts::API_GROUPSFEATURES, group);
}

bool Api::saveFeatureValue(DataFeature *value)
{
    return save(SEConsts::API_VALUESFEATURES, value);
}

bool Api::saveFeature(DataFeature *feature)
{
    return save(SEConsts::API_FEATURES, feature);
}

bool Api::saveMainSettings(DataMain *main)
{
    return save(SEConsts::API_SETTINGSMAIN, main, false);
}

bool Api::saveParamRequisites(DataItem *param)
{
    return save(SEConsts::API_PAYREQUISITES, param);
}

bool Api::saveLicense(DataLicense *license)
{
    return save(SEConsts::API_LICENSES, license);
}

bool Api::saveDelivery(DataDelivery *delivery)
{
    return save(SEConsts::API_DELIVERIES, delivery);
}

bool Api::saveModificationsGroup(DataModificationGroup *group)
{
    return save(SEConsts::API_GROUPSMODIFICATIONS, group);
}

bool Api::saveCommentary(DataItem *comment)
{
    return save(SEConsts::API_COMMENTS, comment);
}

bool Api::saveReview(DataReview *review)
{
    return save(SEConsts::API_REVIEWS, review);
}

bool Api::saveProduct(DataProduct *product)
{
    return save(SEConsts::API_PRODUCTS, product);
}

bool Api::saveSpecialOffer(DataItem *item)
{
    return save(SEConsts::API_SPECIALPRODUCTS, item);
}

bool Api::savePaySystem(DataPaySystem *item)
{
    return save(SEConsts::API_PAYSYSTEMS, item);
}

bool Api::saveTemplateLetter(DataItem *item)
{
    return save(SEConsts::API_LETTERSTEMPLATES, item);
}

bool Api::saveTemplateLetterGroup(DataLetterGroup *item)
{
    return save(SEConsts::API_GROUPSLETTERSTEMPLATES, item);
}

bool Api::saveNewsGroup(DataItem *item)
{
    return save(SEConsts::API_GROUPSNEWS, item);
}

bool Api::saveNews(DataNew *item)
{
    return save(SEConsts::API_NEWS, item);
}

bool Api::saveCurrency(DataCurrency *item)
{
    return save(SEConsts::API_CURRENCIES, item);
}

bool Api::saveCurrencyRate(DataCurrency *item)
{
    return save(SEConsts::API_CURRENCIES_RATE, item);
}

bool Api::saveCoupon(DataCoupon *item)
{
    return save(SEConsts::API_COUPONS, item);
}

bool Api::saveDiscount(DataDiscount *item)
{
    return save(SEConsts::API_DISCOUNTS, item);
}

bool Api::saveOrder(DataOrder *item)
{
    return save(SEConsts::API_ORDERS, item);
}

bool Api::savePreorder(DataOrder *item)
{
    return save(SEConsts::API_PREORDERS, item);
}

bool Api::saveCreditRequest(DataOrder *item)
{
    return save(SEConsts::API_CREDIT_REQUESTS, item);
}

bool Api::savePayment(DataPayment *item)
{
    return save(SEConsts::API_ORDERS_PAYMENTS, item);
}

bool Api::saveIntegrations(DataItem *item)
{
    return save(SEConsts::API_INTEGRATIONS, item);
}

bool Api::saveShopParameters(DataItem *item)
{
    return save(SEConsts::API_SHOP_PARAMETERS, item);
}

bool Api::saveManager(DataContact *item)
{
    return save(SEConsts::API_PERMISSION_USERS, item);
}

bool Api::saveDynFieldsGroup(DataItem *item)
{
    return save(SEConsts::API_UI_DYNFIELDS_GROUPS, item);
}

bool Api::saveDynField(DataItem *item)
{
    return save(SEConsts::API_UI_DYNFIELDS, item);
}

bool Api::saveSESection(DataItem *item)
{
    return save(SEConsts::API_SE_SECTIONS, item);
}

bool Api::saveSESectionItem(DataItem *item)
{
    return save(SEConsts::API_SE_SECTIONS_ITEMS, item);
}

bool Api::saveIntegrationsStores(DataItem *item)
{
    return save(SEConsts::API_INTEGRATIONS_SERVICES, item);
}

bool Api::saveSEODynVar(DataItem *item)
{
    return save(SEConsts::API_SEO_DYNVARS, item);
}

bool Api::saveOptionGroup(DataItem *group)
{
    return save(SEConsts::API_GROUPSOPTIONS, group);
}

bool Api::saveOption(DataItem *item)
{
    return save(SEConsts::API_OPTIONS, item);
}

bool Api::saveOptionValue(DataItem *item)
{
    return save(SEConsts::API_OPTIONSVALUES, item);
}

template <class T>
void setChildrens(T *data, SEVector<T *> *list)
{
    for (int i = 0; i < list->size(); i++) {
        if (!data->getId().isEmpty() && data->getId() == list->at(i)->getIdParent()) {
            DataItem *obj = dynamic_cast <DataItem *> (list->at(i));
            obj->setParent(dynamic_cast <DataItem *> (data));
            dynamic_cast <DataItem *> (data)->setIsFilledChilds(true);
            setChildrens(list->at(i), list);
        }
    }
}

template <class T>
bool Api::getListData(const QString &apiName, SEVector<T *> *list,
                      const Request &request, const bool &useCache)
{    
    QJsonObject object;

    QString outParams = request.getAsJsonString();

    bool result = executeAPI(apiName, SEConsts::APIMETHOD_FETCH,
                             outParams, &object, true, useCache);
    if (result && list) {
        QJsonArray items = object["items"].toArray();

        list->deleteItems();
        list->setCountInBase(object["count"].toInt());
        if (!object["totalAmount"].isNull())
            list->setAmount(object["totalAmount"].toDouble());        

        foreach (const QJsonValue& value, items)
        {
            QJsonObject obj = value.toObject();
            T *dataT = new T;

            DataItem *data = reinterpret_cast<DataItem *> (dataT);            
            data->setFromJSONObject(obj);
            list->append(dataT, 0);
        }

        for (int i = 0; i < list->size(); i++)
            if (list->at(i)->getIdParent().isEmpty())
                setChildrens(list->at(i), list);

        list->setUnModified();
        return true;
    }
    return false;
}

bool Api::getListContactsGroups(ListDataItems *listGroups)
{
    return getListData(SEConsts::API_GROUPSCONTACTS, listGroups);
}

bool Api::getListContacts(const Request &request, ListContacts *listContacts)
{
    return getListData(SEConsts::API_CONTACTS, listContacts, request);
}

bool Api::getListUsers(const Request &request, ListContacts *listUsers)
{
    return getListData(SEConsts::API_PERMISSION_USERS, listUsers, request);
}

bool Api::getListOrders(const Request &request, ListOrders *listOrders, bool useCache)
{    
    return getListData(SEConsts::API_ORDERS, listOrders, request, useCache);
}

bool Api::getListPreorders(const Request &request, ListOrders *listOrders, bool useCache)
{
    return getListData(SEConsts::API_PREORDERS, listOrders, request, useCache);
}

bool Api::getListCreditRequests(const Request &request, ListOrders *listOrders, bool useCache)
{
    return getListData(SEConsts::API_CREDIT_REQUESTS, listOrders, request, useCache);
}

bool Api::getListPayments(const Request &request, ListPayments *listPayments)
{
    return getListData(SEConsts::API_ORDERS_PAYMENTS, listPayments,
                       request);
}

bool Api::getListOrdersStatuses(ListDataItems *listStatuses)
{
    return getListData(SEConsts::API_ORDERS_STATUSES, listStatuses);
}

bool Api::getListDeliveriesStatuses(ListDataItems *listStatuses)
{
    return getListData(SEConsts::API_DELIVERIES_STATUSES, listStatuses);
}

bool Api::getListProducts(const Request &request, ListProducts *listProducts, bool useCache)
{    
    return getListData(SEConsts::API_PRODUCTS, listProducts, request, useCache);
}

bool Api::getListBrands(const Request &request, ListDataItems *listBrands)
{
    return getListData(SEConsts::API_BRANDSPRODUCTS, listBrands, request);
}

bool Api::getListProductsGroups(const Request &request, ListProductsGroups *listGroups, bool useCache)
{
    return getListData(SEConsts::API_PRODUCTS_GROUPS, listGroups, request, useCache);
}

bool Api::getListFeaturesGroups(ListDataItems *listGroups)
{
    return getListData(SEConsts::API_GROUPSFEATURES, listGroups);
}

bool Api::getListFeaturesValues(const Request &request, ListFeatures *listFeatures)
{
    return getListData(SEConsts::API_VALUESFEATURES, listFeatures, request);
}

bool Api::getListFeatures(const Request &request, ListFeatures *listFeatures)
{
    return getListData(SEConsts::API_FEATURES, listFeatures, request);
}

bool Api::getListPaySystems(ListDataPaySystems *listPaysSystems)
{
    return getListData(SEConsts::API_PAYSYSTEMS, listPaysSystems);
}

bool Api::getListPaySystemsPlugins(ListDataItems *listPlugins)
{
    return getListData(SEConsts::API_PAYSYSTEMS_PLUGINS, listPlugins);
}

bool Api::getListPayrequisites(const Request &request, ListDataItems *listRequisites)
{
    return getListData(SEConsts::API_PAYREQUISITES, listRequisites, request);
}

bool Api::getListDeliveries(ListDeliveries *listDeliveries)
{
    return getListData(SEConsts::API_DELIVERIES, listDeliveries);
}

bool Api::getListTypesDeliveries(ListTypesDeliveries *listTypes)
{
    return getListData(SEConsts::API_DELIVERIES_TYPES, listTypes);
}

bool Api::getListLicenses(ListLicense *listLicense)
{
    return getListData(SEConsts::API_LICENSES, listLicense);
}

bool Api::getListCurrencies(ListCurrencies *listCurrencies)
{
    return getListData(SEConsts::API_CURRENCIES, listCurrencies);
}

bool Api::getListCurrencies(const Request &request, ListCurrencies *listCurrencies)
{
    return getListData(SEConsts::API_CURRENCIES, listCurrencies, request);
}

bool Api::getListManufacturers(ListDataItems *listManufacturers)
{
    return getListData(SEConsts::API_MANUFACTURERS, listManufacturers);
}

bool Api::getListModificationsGroups(ListModificationsGroups *listModifications)
{
    return getListData(SEConsts::API_GROUPSMODIFICATIONS, listModifications);
}

bool Api::getListModificationsPricesTypes(ListDataItems *listTypes)
{
    return getListData(SEConsts::API_MODIFICATIONSPRICESTYPES, listTypes);
}

bool Api::getListFeaturesTypes(const Request &request, ListDataItems *listTypes)
{
    return getListData(SEConsts::API_FEATURESTYPES, listTypes, request);
}

bool Api::getListComents(const Request &request, ListDataItems *listComments)
{
    return getListData(SEConsts::API_COMMENTS, listComments, request);
}

bool Api::getListReviews(const Request &request, ListReviews *listReviews)
{
    return getListData(SEConsts::API_REVIEWS, listReviews, request);
}

bool Api::getListSpecialOffer(const Request &request, ListProducts *listProducts)
{
    return getListData(SEConsts::API_SPECIALPRODUCTS, listProducts, request);
}

bool Api::getListTemplatesLetters(ListLettersGroups *listGroups)
{
    return getListData(SEConsts::API_LETTERSTEMPLATES, listGroups);
}

bool Api::getListNewsGroups(ListDataItems *listGroups)
{
    return getListData(SEConsts::API_GROUPSNEWS, listGroups);
}

bool Api::getListNews(const Request &request, ListNews *listNews)
{
    return getListData(SEConsts::API_NEWS, listNews, request);
}

bool Api::getListCoupons(const Request &request, ListCoupons *listCoupons)
{
    return getListData(SEConsts::API_COUPONS, listCoupons, request);
}

bool Api::getListDiscounts(ListDiscounts *listDiscounts)
{
    return getListData(SEConsts::API_DISCOUNTS, listDiscounts);
}

bool Api::getListParametersFilters(ListDataItems *listFilters)
{
    return getListData(SEConsts::API_PARAMETERS_FILTERS, listFilters);
}

bool Api::getListCompanyRequisites(ListCompanyRequisites *listRequisites)
{
    return getListData(SEConsts::API_COMPANY_REQUISITES, listRequisites);
}

bool Api::getListCities(const Request &request, ListDataItems *listCities)
{
    return getListData(SEConsts::API_CITIES, listCities, request);
}

bool Api::getListRegions(ListDataItems *listRegions)
{
    return getListData(SEConsts::API_REGIONS, listRegions);
}

bool Api::getListCountries(ListDataItems *listCountries)
{
    return getListData(SEConsts::API_COUNTRIES, listCountries);
}

bool Api::getListIntegrations(ListIntegrations *listIntegrations)
{
    return getListData(SEConsts::API_INTEGRATIONS, listIntegrations);
}

bool Api::getListIntegrationsServices(ListDataItems *listIntegrations)
{
    return getListData(SEConsts::API_INTEGRATIONS_SERVICES, listIntegrations);
}

bool Api::getListShopParameters(ListIntegrations *listShopParameters)
{
    return getListData(SEConsts::API_SHOP_PARAMETERS, listShopParameters);
}

bool Api::getListTypeOperationsAccounts(ListDataItems *listTypes)
{
    return getListData(SEConsts::API_ACCOUNTS_TYPE_OPERATIONS, listTypes);
}

bool Api::getListDynFieldsGroups(const Request &request, ListDataItems *listGroups)
{
    return getListData(SEConsts::API_UI_DYNFIELDS_GROUPS, listGroups, request);
}

bool Api::getListDynFields(const Request &request, ListDataItems *listFields)
{
    return getListData(SEConsts::API_UI_DYNFIELDS, listFields, request);
}

bool Api::getListSEOTagsVars(const Request &request, ListDataItems *listVars)
{
    return getListData(SEConsts::API_SEO_TAGSVARS, listVars, request);
}

bool Api::getListSEODynVars(const Request &request, ListDataItems *listVars)
{
    return getListData(SEConsts::API_SEO_DYNVARS, listVars, request);
}

bool Api::getListSESections(const Request &request, ListDataItems *listSections)
{
    return getListData(SEConsts::API_SE_SECTIONS, listSections, request);
}

bool Api::getListSESectionsItems(const Request &request, ListDataItems *listSectionsItems)
{
    return getListData(SEConsts::API_SE_SECTIONS_ITEMS, listSectionsItems, request);
}

bool Api::getListOptionsGroups(ListDataItems *listOptionsGroups)
{
    return getListData(SEConsts::API_GROUPSOPTIONS, listOptionsGroups);
}

bool Api::getListOptions(ListOptions *listOptions)
{
    return getListData(SEConsts::API_OPTIONS, listOptions);
}

bool Api::getListOptionsValues(const Request &request, ListOptionValues *listOptionsValues)
{
    return getListData(SEConsts::API_OPTIONSVALUES, listOptionsValues, request);
}

bool Api::getListProjects(ListDataItems *projects)
{
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);

    QEventLoop *loop = new QEventLoop();
    connect(manager, SIGNAL(finished(QNetworkReply*)),loop, SLOT(quit()));

    QUrl url;
    QString s = SEConfig::getApiAuthController(SEConsts::API_PROJECTS, SEConsts::APIMETHOD_FETCH);
    url.setUrl(s);

    QJsonObject obj;
    obj.insert("id", QJsonValue(SEConfig::idAdminProject));
    QJsonDocument doc;
    doc.setObject(obj);

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QByteArray strPost(doc.toJson(QJsonDocument::Compact));

    QNetworkReply *reply = manager->post(request, strPost);
    loop->exec();
    delete loop;

    if (reply->error() == QNetworkReply::NoError)
    {
        QByteArray bytes;
        bytes = reply->readAll();
        QJsonDocument json;
        if(json.fromJson(bytes).object()["status"].toString() == "ok") {            
            QJsonArray items = json.fromJson(bytes).object()["data"].toObject()["items"].toArray();
            for (int i = 0; i < items.size(); ++i) {
                DataItem *item = new DataItem;
                item->setName(items.at(i).toString());
                projects->append(item);
            }
            delete reply;
            return true;
        }
    }

    delete reply;
    return false;
}

bool Api::getInfoData(const QString &apiName, DataItem *data, bool useCache)
{
    QString params(data->getAsJsonString());

    QJsonObject objAnswer;
    bool result = executeAPI(apiName, SEConsts::APIMETHOD_INFO,
                             params, &objAnswer, false, useCache);
    if (result) {
        QJsonArray items = objAnswer["items"].toArray();        
        if (items.count()) {
            objAnswer = items.at(0).toObject();                        
            data->setFromJSONObject(objAnswer);
            return true;
        }
    }
    return false;
}

bool Api::getInfoContact(DataContact *contact)
{    
    return getInfoData(SEConsts::API_CONTACTS, contact);
}

bool Api::getInfoProductGroup(DataProductsGroup *group)
{
    return getInfoData(SEConsts::API_PRODUCTS_GROUPS, group);
}

bool Api::getInfoFeatureGroup(DataItem *group)
{
    return getInfoData(SEConsts::API_GROUPSFEATURES, group);
}

bool Api::getInfoFeature(DataFeature *feature)
{
    return getInfoData(SEConsts::API_FEATURES, feature);
}

bool Api::getInfoFeatureValue(DataFeature *feature)
{
    return getInfoData(SEConsts::API_VALUESFEATURES, feature);
}

bool Api::getInfoBrand(DataItem *brand)
{
    return getInfoData(SEConsts::API_BRANDSPRODUCTS, brand);
}

bool Api::getInfoMain(DataMain *dataMain)
{
    return getInfoData(SEConsts::API_SETTINGSMAIN, dataMain);
}

bool Api::getInfoLicense(DataLicense *license)
{
    return getInfoData(SEConsts::API_LICENSES, license);
}

bool Api::getInfoDelivery(DataDelivery *delivery)
{
    return getInfoData(SEConsts::API_DELIVERIES, delivery);
}

bool Api::getInfoProduct(DataProduct *product, bool useCache)
{
    return getInfoData(SEConsts::API_PRODUCTS, product, useCache);
}

bool Api::getInfoPaySystem(DataPaySystem *paySystem)
{
    return getInfoData(SEConsts::API_PAYSYSTEMS, paySystem);
}

bool Api::getInfoNewsGroup(DataItem *group)
{
    return getInfoData(SEConsts::API_GROUPSNEWS, group);
}

bool Api::getInfoNews(DataNew *news)
{
    return getInfoData(SEConsts::API_NEWS, news);
}

bool Api::getInfoDiscounts(DataDiscount *discount)
{
    return getInfoData(SEConsts::API_DISCOUNTS, discount);
}

bool Api::getInfoOrder(DataOrder *order, bool useCache)
{
    return getInfoData(SEConsts::API_ORDERS, order, useCache);
}

bool Api::getInfoPreorder(DataOrder *order, bool useCache)
{
    return getInfoData(SEConsts::API_PREORDERS, order, useCache);
}

bool Api::getInfoCreditRequest(DataOrder *order, bool useCache)
{
    return getInfoData(SEConsts::API_CREDIT_REQUESTS, order, useCache);
}

bool Api::getInfoPayment(DataPayment *payment)
{
    return getInfoData(SEConsts::API_ORDERS_PAYMENTS, payment);
}

bool Api::getInfoCoupon(DataCoupon *coupon)
{
    return getInfoData(SEConsts::API_COUPONS, coupon);
}

bool Api::getInfoCurrencyRate(DataCurrency *currency)
{
    return getInfoData(SEConsts::API_CURRENCIES_RATE, currency);
}

bool Api::getInfoModificationGroup(DataModificationGroup *group)
{
    return getInfoData(SEConsts::API_GROUPSMODIFICATIONS, group);
}

bool Api::getInfoOptionGroup(DataItem *group)
{
    return getInfoData(SEConsts::API_GROUPSOPTIONS, group);
}

bool Api::getInfoOption(DataItem *item)
{
    return getInfoData(SEConsts::API_OPTIONS, item);
}

bool Api::getInfoOptionValue(DataItem *item)
{
    return getInfoData(SEConsts::API_OPTIONSVALUES, item);
}

bool Api::deleteItem(const QString &apiName, const Request &request)
{
    return executeAPI(apiName, SEConsts::APIMETHOD_DELETE, request, 0);
}

bool Api::deleteProductsGroups(const Request &request)
{
    return executeAPI(SEConsts::API_PRODUCTS_GROUPS,
                      SEConsts::APIMETHOD_DELETE, request, 0);
}

bool Api::deleteContactGroups(const Request &request)
{
    return executeAPI(SEConsts::API_GROUPSCONTACTS,
                      SEConsts::APIMETHOD_DELETE, request, 0);
}

bool Api::deleteContacts(const Request &request)
{
    return executeAPI(SEConsts::API_CONTACTS,
                      SEConsts::APIMETHOD_DELETE, request, 0);
}

bool Api::deleteCompanies(const Request &request)
{
    return executeAPI(SEConsts::API_COMPANIES,
                      SEConsts::APIMETHOD_DELETE, request, 0);
}

bool Api::deleteFeatures(const Request &request)
{
    return executeAPI(SEConsts::API_FEATURES,
                      SEConsts::APIMETHOD_DELETE, request, 0);
}

bool Api::deleteFeaturesValues(const Request &request)
{
    return executeAPI(SEConsts::API_VALUESFEATURES,
                      SEConsts::APIMETHOD_DELETE, request, 0);
}

bool Api::deleteFeaturesGroups(const Request &request)
{
    return executeAPI(SEConsts::API_GROUPSFEATURES,
                      SEConsts::APIMETHOD_DELETE, request, 0);
}

bool Api::deleteImages(const Request &request)
{
    return executeAPI(SEConsts::API_FILESIMAGES,
                      SEConsts::APIMETHOD_DELETE, request, 0);
}

bool Api::deleteBrands(const Request &request)
{
    return executeAPI(SEConsts::API_BRANDSPRODUCTS,
                      SEConsts::APIMETHOD_DELETE, request, 0);
}

bool Api::deletePayParams(const Request &request)
{
    return executeAPI(SEConsts::API_PAYREQUISITES,
                      SEConsts::APIMETHOD_DELETE, request, 0);
}

bool Api::deleteLicenses(const Request &request)
{
    return executeAPI(SEConsts::API_LICENSES, SEConsts::APIMETHOD_DELETE,
                      request, 0);
}

bool Api::deleteDeliveries(const Request &request)
{
    return executeAPI(SEConsts::API_DELIVERIES, SEConsts::APIMETHOD_DELETE,
                      request, 0);
}

bool Api::deleteModificationsGroups(const Request &request)
{
    return executeAPI(SEConsts::API_GROUPSMODIFICATIONS, SEConsts::APIMETHOD_DELETE,
                      request, 0);
}

bool Api::deleteComments(const Request &request)
{
    return executeAPI(SEConsts::API_COMMENTS, SEConsts::APIMETHOD_DELETE,
                      request, 0);
}

bool Api::deleteProducts(const Request &request)
{
    return executeAPI(SEConsts::API_PRODUCTS, SEConsts::APIMETHOD_DELETE,
                      request, 0);
}

bool Api::deleteSpecialOffer(const Request &request)
{
    return executeAPI(SEConsts::API_SPECIALPRODUCTS, SEConsts::APIMETHOD_DELETE, request, 0);
}

bool Api::deleteCountries(const Request &request)
{
    return executeAPI(SEConsts::API_COUNTRIES, SEConsts::APIMETHOD_DELETE, request, 0);
}

bool Api::deleteRegions(const Request &request)
{
    return executeAPI(SEConsts::API_REGIONS, SEConsts::APIMETHOD_DELETE, request, 0);
}

bool Api::deleteCities(const Request &request)
{
    return executeAPI(SEConsts::API_CITIES, SEConsts::APIMETHOD_DELETE, request, 0);
}

bool Api::deletePaySystems(const Request &request)
{
    return executeAPI(SEConsts::API_PAYSYSTEMS, SEConsts::APIMETHOD_DELETE, request, 0);
}

bool Api::deleteTemplateLetter(const Request &request)
{
    return executeAPI(SEConsts::API_LETTERSTEMPLATES, SEConsts::APIMETHOD_DELETE, request, 0);
}

bool Api::deleteTemplateLetterGroup(const Request &request)
{
    return executeAPI(SEConsts::API_GROUPSLETTERSTEMPLATES, SEConsts::APIMETHOD_DELETE, request, 0);
}

bool Api::deleteNewsGroups(const Request &request)
{
    return executeAPI(SEConsts::API_GROUPSNEWS, SEConsts::APIMETHOD_DELETE, request, 0);
}

bool Api::deleteNews(const Request &request)
{
    return executeAPI(SEConsts::API_NEWS, SEConsts::APIMETHOD_DELETE, request, 0);
}

bool Api::deleteCurrencies(const Request &request)
{
    return executeAPI(SEConsts::API_CURRENCIES, SEConsts::APIMETHOD_DELETE, request, 0);
}

bool Api::deleteCoupons(const Request &request)
{
    return executeAPI(SEConsts::API_COUPONS, SEConsts::APIMETHOD_DELETE, request, 0);
}

bool Api::deleteDiscounts(const Request &request)
{
    return executeAPI(SEConsts::API_DISCOUNTS, SEConsts::APIMETHOD_DELETE, request, 0);
}

bool Api::deleteOrders(const Request &request)
{
    return executeAPI(SEConsts::API_ORDERS, SEConsts::APIMETHOD_DELETE, request, 0);
}

bool Api::deletePreorders(const Request &request)
{
    return executeAPI(SEConsts::API_PREORDERS, SEConsts::APIMETHOD_DELETE, request, 0);
}

bool Api::deleteCreditRequests(const Request &request)
{
    return executeAPI(SEConsts::API_CREDIT_REQUESTS, SEConsts::APIMETHOD_DELETE, request, 0);
}

bool Api::deletePayments(const Request &request)
{
    return executeAPI(SEConsts::API_ORDERS_PAYMENTS, SEConsts::APIMETHOD_DELETE, request, 0);
}

bool Api::deleteManager(const Request &request)
{
    return executeAPI(SEConsts::API_PERMISSION_USERS, SEConsts::APIMETHOD_DELETE, request, 0);
}

bool Api::deleteReviews(const Request &request)
{
    return executeAPI(SEConsts::API_REVIEWS, SEConsts::APIMETHOD_DELETE, request, 0);
}

bool Api::deleteDynFieldsGroup(const Request &request)
{
    return executeAPI(SEConsts::API_UI_DYNFIELDS_GROUPS, SEConsts::APIMETHOD_DELETE, request, 0);
}

bool Api::deleteDynFields(const Request &request)
{
    return executeAPI(SEConsts::API_UI_DYNFIELDS, SEConsts::APIMETHOD_DELETE, request, 0);
}

bool Api::deleteSESectionItem(const Request &request)
{
    return executeAPI(SEConsts::API_SE_SECTIONS_ITEMS, SEConsts::APIMETHOD_DELETE, request, 0);
}

bool Api::deleteSEODynVars(const Request &request)
{
    return executeAPI(SEConsts::API_SEO_DYNVARS, SEConsts::APIMETHOD_DELETE, request, 0);
}

bool Api::deleteOptionsGroups(const Request &request)
{
    return executeAPI(SEConsts::API_GROUPSOPTIONS, SEConsts::APIMETHOD_DELETE, request, 0);
}

bool Api::deleteOptions(const Request &request)
{
    return executeAPI(SEConsts::API_OPTIONS, SEConsts::APIMETHOD_DELETE, request, 0);
}

bool Api::deleteOptionsValues(const Request &request)
{
    return executeAPI(SEConsts::API_OPTIONSVALUES, SEConsts::APIMETHOD_DELETE, request, 0);
}

bool Api::sortFeatureGroups(const Request &request)
{
    return executeAPI(SEConsts::API_GROUPSFEATURES, SEConsts::APIMETHOD_SORT, request, 0, false);
}

bool Api::sortFeatures(const Request &request)
{
    return executeAPI(SEConsts::API_FEATURES, SEConsts::APIMETHOD_SORT, request, 0, false);
}

bool Api::sortFeaturesValues(const Request &request)
{
    return executeAPI(SEConsts::API_VALUESFEATURES, SEConsts::APIMETHOD_SORT, request, 0, false);
}

bool Api::setSEOFeatures(const Request &request)
{
    return executeAPI(SEConsts::API_FEATURES, SEConsts::APIMETHOD_SEO, request, 0, false);
}

bool Api::sortModificationsGroups(const Request &request)
{
    return executeAPI(SEConsts::API_GROUPSMODIFICATIONS, SEConsts::APIMETHOD_SORT, request, 0, false);
}

bool Api::sortPaySystems(const Request &request)
{
    return executeAPI(SEConsts::API_PAYSYSTEMS, SEConsts::APIMETHOD_SORT, request, 0, false);
}

bool Api::sortProductsGroups(const Request &request)
{
    return executeAPI(SEConsts::API_PRODUCTS_GROUPS, SEConsts::APIMETHOD_SORT, request, 0, false);
}

bool Api::sortDeliveriesTypes(const Request &request)
{
    return executeAPI(SEConsts::API_DELIVERIES_TYPES, SEConsts::APIMETHOD_SORT, request, 0, false);
}

bool Api::sortDynFieldsGroup(const Request &request)
{
    return executeAPI(SEConsts::API_UI_DYNFIELDS_GROUPS, SEConsts::APIMETHOD_SORT, request, 0, false);
}

bool Api::sortDynField(const Request &request)
{
    return executeAPI(SEConsts::API_UI_DYNFIELDS, SEConsts::APIMETHOD_SORT, request, 0, false);
}

bool Api::sortSESectionItem(const Request &request)
{
    return executeAPI(SEConsts::API_SE_SECTIONS_ITEMS, SEConsts::APIMETHOD_SORT, request, 0, false);
}

bool Api::sortDiscounts(const Request &request)
{
    return executeAPI(SEConsts::API_DISCOUNTS, SEConsts::APIMETHOD_SORT, request, 0, false);
}

bool Api::sortBrands(const Request &request)
{
    return executeAPI(SEConsts::API_BRANDSPRODUCTS, SEConsts::APIMETHOD_SORT, request, 0, false);
}

bool Api::sortOptionGroups(const Request &request)
{
    return executeAPI(SEConsts::API_GROUPSOPTIONS, SEConsts::APIMETHOD_SORT, request, 0, false);
}

bool Api::sortOption(const Request &request)
{
    return executeAPI(SEConsts::API_OPTIONS, SEConsts::APIMETHOD_SORT, request, 0, false);
}

bool Api::sortOptionValues(const Request &request)
{
    return executeAPI(SEConsts::API_OPTIONSVALUES, SEConsts::APIMETHOD_SORT, request, 0, false);
}

QPixmap Api::getImageFromURL(const QString &URL)
{
    QNetworkAccessManager manager;
    QEventLoop loop;
    connect(&manager, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));
    QUrl url(URL);
    QNetworkRequest request(url);
    QNetworkReply *reply = manager.get(request);

    loop.exec();

    QPixmap pixmap;
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray jpegData = reply->readAll();
        pixmap.loadFromData(jpegData);
    }
    reply->deleteLater();

    return pixmap;
}

QByteArray Api::getContentFromUrl(const QString &URL)
{
    QNetworkAccessManager manager;
    QEventLoop loop;
    connect(&manager, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));
    QUrl url(URL);
    QNetworkRequest request(url);
    QNetworkReply *reply = manager.get(request);

    loop.exec();

    QByteArray data;
    if (reply->error() == QNetworkReply::NoError)
        data = reply->readAll();
    reply->deleteLater();

    return data;
}

void Api::clearCache()
{
    QString URL = "http://" + SEConfig::projectHostName + "/lib/image.php?deletecache";

    QNetworkAccessManager *manager = new QNetworkAccessManager;
    QUrl url(URL);
    QNetworkRequest request(url);
    manager->get(request);
    manager->deleteLater();
}

QStringList Api::translit(const QStringList list)
{
    QJsonObject object;
    QJsonObject objectOut;

    QJsonArray items;
    for (int i = 0; i < list.size(); i++)
        items.append(QJsonValue(list.at(i)));
    objectOut.insert("vars", QJsonValue(items));
    QJsonDocument doc;
    doc.setObject(objectOut);
    QString outParams = QString(doc.toJson(QJsonDocument::Compact));
    bool result = executeAPI(SEConsts::API_FUNCTIONS, "Translit.api",
                             outParams, &object);
    QStringList answer;
    if (result) {
        QJsonArray items = object["items"].toArray();
        for (int i = 0; i < items.size(); i++)
            answer.append(items.at(i).toString());
    }
    return answer;
}

bool Api::setCodeUrlProduct(const DataItem *item)
{
    QJsonObject object;
    return executeAPI(SEConsts::API_PRODUCTS, SEConsts::APIMETHOD_SETCODEURL,
                      item->getAsJsonString(), &object);
}

bool Api::setCodeUrlProductGroup(const DataItem *item)
{
    QJsonObject object;
    return executeAPI(SEConsts::API_PRODUCTS_GROUPS, SEConsts::APIMETHOD_SETCODEURL,
                      item->getAsJsonString(), &object);
}

bool Api::setArticleProduct(const DataItem *item)
{
    QJsonObject object;
    return executeAPI(SEConsts::API_PRODUCTS, SEConsts::APIMETHOD_SETARTICLE,
                      item->getAsJsonString(), &object);
}

bool Api::addProductPrice(const DataProduct *product)
{
    QJsonObject object;
    return executeAPI(SEConsts::API_PRODUCTS, SEConsts::APIMETHOD_ADDPRODUCTPRICE,
                      product->getAsJsonString(), &object);
}

const QString Api::uploadImageToYandex(const QString &fileName)
{
    if (!QFile(fileName).exists() || SEConfig::yandexApi.isEmpty() ||
            SEConfig::yandexLogin.isEmpty() || SEConfig::yandexToken.isEmpty())
        return QString();

    QFile *fileImage = new QFile(fileName);
    if(!fileImage->open(QIODevice::ReadOnly)) {
        QMessageBox::critical(0, tr("Ошибка чтения файла"),
                              tr("Не удаётся прочитать файл") + ": " +
                              fileName + "!");
        return QString();
    }

    QNetworkAccessManager manager;
    QEventLoop loop;
    connect(&manager, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));
    QString apiUrl = SEConfig::yandexApi + "/api/users/" + SEConfig::yandexLogin +  "/photos/";    
    QUrl url;
    url.setUrl(apiUrl);
    QNetworkRequest request(url);    
    QByteArray auth;
    auth.append("OAuth " + SEConfig::yandexToken);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "image/jpeg");
    request.setRawHeader("Host", "api-fotki.yandex.ru");
    request.setRawHeader("Content-Length", QByteArray::number(fileImage->size()));
    request.setRawHeader("Accept", "application/json");
    request.setRawHeader("Authorization", auth);
    QNetworkReply *reply = manager.post(request, fileImage->readAll());
    loop.exec();
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray bytes = reply->readAll();
        SEConfig::debug("YandexPhotos", QString(bytes));
        QJsonDocument json;
        if(!json.fromJson(bytes).object()["img"].isNull()) {
            QJsonObject object = json.fromJson(bytes).object()["img"].toObject();
            if (!object["L"].isNull())
                return object["L"].toObject()["href"].toString();
        }
    };

    return QString();
}

const QString Api::getLastError() const
{
    return lastError;
}

void Api::updateApiExt()
{
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    QEventLoop loop;
    connect(manager, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));

    QString apiScript = SEConfig::getApiProjectController(SEConsts::API_FILESIMAGES,
                                                          SEConsts::APIMETHOD_UPLOAD);
    apiScript += "?section=products&count=2";

    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    QHttpPart partInit, partUpdate;
    partInit.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("text/php"));
    partInit.setHeader(QNetworkRequest::ContentDispositionHeader,
                        QVariant("form-data; name=\"file0\"; filename=\"init.php\""));
    QFile fileInit(":/strings/res/strings/init.php");
    fileInit.open(QIODevice::ReadOnly);
    QByteArray b;
    partInit.setBody(b = fileInit.readAll());

    multiPart->append(partInit);
    fileInit.close();

    partUpdate.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("text/php"));
    partUpdate.setHeader(QNetworkRequest::ContentDispositionHeader,
                        QVariant("form-data; name=\"file1\"; filename=\"update.php\""));
    QFile fileUpdate(":/strings/res/strings/update.php");
    fileUpdate.open(QIODevice::ReadOnly);
    partUpdate.setBody(fileUpdate.readAll());
    multiPart->append(partUpdate);
    fileUpdate.close();

    QUrl url(apiScript);
    QNetworkRequest request(url);

    QByteArray header;
    header.append("Token");
    QByteArray value;
    value.append(SEConfig::token);
    request.setRawHeader(header, value);

    QNetworkReply * replyExt = manager->post(request, multiPart);
    multiPart->setParent(replyExt);

    loop.exec();
    replyExt->deleteLater();

    QString host = SEConfig::apiProject().replace("api/1", "");
    QNetworkReply * replyUpdate =
            manager->get(QNetworkRequest(QUrl(QString(host + "/images/rus/shopprice/init.php"))));

    loop.exec();
    replyUpdate->deleteLater();

    emit finishedRequest(true, QString());
}

bool Api::executeAPI(const QString &apiName, const QString &apiMethod,
                     const Request &request, QJsonObject *answerJSON,
                     const bool &isBlockingMode, const bool &useCache)
{
    return executeAPI(apiName, apiMethod, request.getAsJsonString(),
                      answerJSON, isBlockingMode, useCache);
}

bool Api::getFromCache(const QString &url, const QString &params, QJsonObject *answerJSON)
{
    bool result = false;
    if (answerJSON) {
        QString fileName = SEConfig::getFolderCache() + QDir::separator() + QMD5(url + params);
        QFile file(fileName);
        if (file.exists() && file.open(QIODevice::ReadOnly)) {
            QByteArray bytes(file.readAll());
            QJsonDocument json;
            if(json.fromJson(bytes).object().value("data").isObject()) {
                *answerJSON = json.fromJson(bytes).object()["data"].toObject();
                result = true;
            }
            file.close();
            file.remove();
        }        
    }
    return result;
}

void Api::requestInThread(const QString &apiObject, const QString &apiMethod, const Request &request)
{
    requestInThread(apiObject, apiMethod, request.getAsJsonString());
}

void Api::requestInThread(const QString &apiObject, const QString &apiMethod, const DataItem &item)
{
    requestInThread(apiObject, apiMethod, item.getAsJsonString());
}

void Api::requestInThread(const QString &apiObject, const QString &apiMethod, const QString &apiRequestParams)
{
    QThread* thread = new QThread;
    Api *api = new Api;
    api->setApiRequestParams(apiObject, apiMethod, apiRequestParams);
    api->moveToThread(thread);
    connect(thread, SIGNAL(started()), api, SLOT(executeRequestInThread()));
    connect(api, SIGNAL(finishedRequest(bool,QString)), thread, SLOT(quit()));
    connect(api, SIGNAL(finishedRequest(bool,QString)), api, SLOT(deleteLater()));
    connect(api, SIGNAL(finishedRequest(bool,QString)), SIGNAL(finishedRequest(bool,QString)));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    thread->start();
}

bool Api::executeAPI(const QString &apiName, const QString &apiMethod,
                     const QString &params, QJsonObject *answerJSON,
                     const bool &isBlockingMode, const bool &useCache)
{
    QString apiScript = SEConfig::getApiProjectController(apiName, apiMethod);
    if (Api::isCompressed)
        apiScript += "?compressed=2";
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(proxyAuthenticationRequired(QNetworkProxy,QAuthenticator*)),
            SLOT(onProxyAuthentication(QNetworkProxy,QAuthenticator*)));

    QEventLoop *loop = new QEventLoop();
    if (isBlockingMode  || answerJSON)
        connect(manager, SIGNAL(finished(QNetworkReply*)),
                loop, SLOT(quit()));
    else connect(manager, SIGNAL(finished(QNetworkReply*)),
                 SLOT(onFinished(QNetworkReply*)));

    QUrl url;    
    url.setUrl(apiScript);
    QNetworkRequest request(url);
    setAuthParamsForRequest(request);

    SEConfig::debug("************************************************", QString());
    SEConfig::debug("Request to server", apiScript);
    QByteArray strPost;    
    strPost.append(params);
    SEConfig::debug("Data to server", QString(strPost));

    if (useCache && getFromCache(apiScript, params, answerJSON))
        return true;

    QNetworkReply *reply = manager->post(request, strPost);

    if (isBlockingMode || answerJSON) {
        loop->exec();        
        delete loop;
        return readApiReply(reply, apiScript, params, answerJSON);
    }
    return true;
}

bool Api::readApiReply(QNetworkReply *reply, const QString &url, const QString &params,
                       QJsonObject *requestJSON)
{    

    if (reply->error() == QNetworkReply::NoError)
    {
        QByteArray bytes;
        // Читаем ответ от сервера
        bytes = reply->readAll();

        if (Api::isCompressed && !bytes.isEmpty())
            bytes = qUncompress(bytes);                

        SEConfig::debug("Answer from server:", QString());
        SEConfig::debug(bytes);
        QJsonDocument json;        
        if(json.fromJson(bytes).object()["status"].toString() == "ok") {
            QString fileName = SEConfig::getFolderCache() + QDir::separator() + QMD5(url + params);
            QFile file(fileName);
            if (file.open(QIODevice::WriteOnly))
                file.write(bytes);
            file.close();
            if(requestJSON && json.fromJson(bytes).object().value("data").isObject()) {
                *requestJSON = json.fromJson(bytes).object()["data"].toObject();
            }
            delete reply;     
            return true;
        } else {
            QString error = json.fromJson(bytes).object()["error"].toString();
            lastError = error.isEmpty() ? tr("Не удаётся распознать ответ сервера") + "!" :
                                      error;
            if (SEConfig::idThreadUi == QThread::currentThreadId())
                QMessageBox::critical(0, tr("Ой, ошибка сервера"), lastError);
        }
    }
    delete reply;
    return false;
}


void Api::onFinished(QNetworkReply *reply)
{
    delete reply;
}

void Api::onProxyAuthentication(const QNetworkProxy &proxy, QAuthenticator *authenticator)
{
    DialogProxySettings dialogProxy;
    SEConfig::proxyHost = proxy.hostName();
    SEConfig::proxyPort = proxy.port();
    SEConfig::proxyLogin = proxy.user();
    SEConfig::proxyPassword = proxy.password();
    SEConfig::isProxyConnected = true;
    if (dialogProxy.exec() == QDialog::Accepted) {
        authenticator->setUser(SEConfig::proxyLogin);
        authenticator->setPassword(SEConfig::proxyPassword);
        setProxyParams();
    }
}

void Api::executeRequestInThread()
{
    bool result = executeAPI(apiObject, apiMethod, apiRequestParams, 0);
    emit finishedRequest(result, lastError);
}

void Api::setAuthParamsForRequest(QNetworkRequest &request)
{
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
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
    header.clear();  
    request.setRawHeader(header, value);
    if (!SEConfig::login.isEmpty()) {
        header.clear();
        header.append("Login");
        value.clear();
        value.append(SEConfig::login);
        request.setRawHeader(header, value);
        if (!SEConfig::password.isEmpty()) {
            header.clear();
            header.append("Password");
            value.clear();
            value.append(SEConfig::password);
            request.setRawHeader(header, value);
        }
    }
}

void Api::setProxyParams()
{
    QNetworkProxy proxy;
    proxy.setType(QNetworkProxy::HttpProxy);
    proxy.setHostName(SEConfig::proxyHost);
    proxy.setPort(SEConfig::proxyPort);
    proxy.setUser(SEConfig::proxyLogin);
    proxy.setPassword(SEConfig::proxyPassword);
    QNetworkProxy::setApplicationProxy(proxy);
}

bool Api::getBinaryData(const QString &apiName, const QString &apiMethod, const Request &params,
                        QByteArray *data)
{
    QString apiScript = SEConfig::getApiProjectController(apiName, apiMethod);
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    QEventLoop loop;
    connect(manager, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));

    QUrl url;
    url.setUrl(apiScript);
    QNetworkRequest request(url);
    setAuthParamsForRequest(request);

    QByteArray strPost;
    strPost.append(params.getAsJsonString());

    QNetworkReply *reply = manager->post(request, strPost);
    loop.exec();
    if (reply->error() == QNetworkReply::NoError)
    {        
        *data = reply->readAll();
        return 1;
    } else qDebug() << reply->error();
    return 0;
}

QString Api::sendBinaryData(const QString &apiName, const QString &apiMethod, const QString &params,
                            QByteArray *data, const QString &fileName)
{
    QString apiScript = SEConfig::getApiProjectController(apiName, apiMethod);
    if (!params.isEmpty())
        apiScript += "?" + params;
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    QEventLoop loop;
    connect(manager, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));

    QUrl url;
    url.setUrl(apiScript);
    QNetworkRequest request(url);
    QByteArray header;
    header.append("Token");
    QByteArray value;
    value.append(SEConfig::token);
    request.setRawHeader(header, value);
    header.clear();
    header.append("Core-Version");
    value.clear();
    value.append(SEConfig::coreVersion);
    request.setRawHeader(header, value);
    header.clear();
    header.append("Cookie");
    value.clear();
    value.append("PHPSESSID=" + SEConfig::idSession);
    request.setRawHeader(header, value);

    QNetworkReply *reply = 0;
    if (data && !fileName.isEmpty()) {
        QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);
        QHttpPart bytesPart;
        bytesPart.setHeader(QNetworkRequest::ContentTypeHeader,
                            QVariant("application/zip"));
        bytesPart.setHeader(QNetworkRequest::ContentDispositionHeader,
                QVariant("form-data; name=\"file\"; filename=\"" + fileName + "\""));

        bytesPart.setBody(*data);
        multiPart->append(bytesPart);

        reply = manager->post(request, multiPart);
    } else reply = manager->get(request);
    loop.exec();
    QString result;    
    if (reply->error() == QNetworkReply::NoError)
        result.append(reply->readAll());
    if (result.isEmpty())
        result = tr("Неизвестная ошибка запроса данных!");
    return result;
}

bool Api::exportData(const QString &apiCatalogName, const Request &params, QByteArray *data)
{    
    return getBinaryData(apiCatalogName, SEConsts::APIMETHOD_EXPORT, params, data);
}

bool Api::exportItemData(const QString &apiCatalogName, const Request &params, QByteArray *data)
{
    return getBinaryData(apiCatalogName, SEConsts::APIMETHOD_EXPORT_ITEM, params, data);
}

bool Api::getPaymentCheck(const Request &params, QByteArray *data)
{
    return getBinaryData(SEConsts::API_ORDERS, SEConsts::APIMETHOD_DOC, params, data);
}

bool Api::getShipmentBlank(const Request &params, QByteArray *data)
{
    return getBinaryData(SEConsts::API_ORDERS, SEConsts::APIMETHOD_BLANK, params, data);
}

QString Api::importData(const QString &apiCatalogName, const QString &params, QByteArray *data,
                        const QString &fileName)
{
    return sendBinaryData(apiCatalogName, SEConsts::APIMETHOD_IMPORT, params, data, fileName);
}

bool Api::importProductFromYMUrl(DataProduct *product)
{
    QString params(product->getAsJsonString());

    QJsonObject objAnswer;
    bool result = executeAPI(SEConsts::API_PRODUCTS, SEConsts::APIMETHOD_YM_IMPORT,
                             params, &objAnswer);
    if (result) {
        QJsonArray items = objAnswer["items"].toArray();
        if (items.count()) {
            objAnswer = items.at(0).toObject();
            product->setFromJSONObject(objAnswer);
            return true;
        }
    }
    return false;
}

bool Api::restoreOrder(DataOrder *item)
{
    QJsonObject object;
    bool result = executeAPI(SEConsts::API_ORDERS, SEConsts::APIMETHOD_RESTORE,
                             item->getAsJsonString(), &object);

    if (result && (!object["id"].isNull() && !object["id"].toString().isEmpty())) {
        item->setFromJSONObject(object);
    } else result = false;
    return result;
}

