#ifndef API_H
#define API_H

#include <QWidget>
#include <QMessageBox>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QStringList>
#include <QPixmap>
#include <QNetworkProxy>
#include <QAuthenticator>

#include "seconsts.h"
#include "datacontact.h"
#include "dataorder.h"
#include "dataproduct.h"
#include "dataproductsgroup.h"
#include "datafeature.h"
#include "datamain.h"
#include "datadelivery.h"
#include "datalicense.h"
#include "datacurrency.h"
#include "datapaysystem.h"
#include "request.h"
#include "datalettergroup.h"
#include "datanew.h"
#include "datacoupon.h"
#include "datadiscount.h"
#include "datapayment.h"
#include "datatypedelivery.h"
#include "datacompanyrequisite.h"
#include "dataintegration.h"
#include "datareview.h"
#include "dataoption.h"

class QNetworkReply;
class QNetworkRequest;

class Api : public QObject
{
    Q_OBJECT

public:    
    static bool isCompressed;
    /* Метод авторизации */
    bool auth();
    /* Метод регистрации проекта */
    bool registration(const QString &serial, const QString &hash, const QString &project,
                      const bool &isCheckedMode = false);

    void setApiRequestParams(const QString &apiName, const QString &apiMethod,
                             const QString &apiRequestParams);

    /* Методы сохранения объектов */
    bool save(const QString &apiName, DataItem *data,
             const bool &isBlockingMode = true);
    bool saveContact(DataContact *contact);
    bool saveContactGroup(DataItem *group);
    bool saveProductGroup(DataProductsGroup *group);
    bool saveBrand(DataItem *brand);
    bool saveFeatureGroup(DataItem *group);
    bool saveFeatureValue(DataFeature *value);
    bool saveFeature(DataFeature *feature);
    bool saveMainSettings(DataMain *main);
    bool saveParamRequisites(DataItem *param);
    bool saveLicense(DataLicense *license);
    bool saveDelivery(DataDelivery *delivery);
    bool saveModificationsGroup(DataModificationGroup *group);
    bool saveCommentary(DataItem *comment);
    bool saveReview(DataReview *review);
    bool saveProduct(DataProduct *product);
    bool saveSpecialOffer(DataItem *item);
    bool savePaySystem(DataPaySystem *item);
    bool saveTemplateLetter(DataItem *item);
    bool saveTemplateLetterGroup(DataLetterGroup *item);
    bool saveNewsGroup(DataItem *item);
    bool saveNews(DataNew *item);
    bool saveCurrency(DataCurrency *item);
    bool saveCurrencyRate(DataCurrency *item);
    bool saveCoupon(DataCoupon *item);
    bool saveDiscount(DataDiscount *item);
    bool saveOrder(DataOrder *item);
    bool savePreorder(DataOrder *item);
    bool saveCreditRequest(DataOrder *item);
    bool savePayment(DataPayment *item);
    bool saveIntegrations(DataItem *item);
    bool saveShopParameters(DataItem *item);
    bool saveManager(DataContact *item);
    bool saveDynFieldsGroup(DataItem *item);
    bool saveDynField(DataItem *item);
    bool saveSESection(DataItem *item);
    bool saveSESectionItem(DataItem *item);
    bool saveIntegrationsStores(DataItem *item);
    bool saveSEODynVar(DataItem *item);
    bool saveOptionGroup(DataItem *group);
    bool saveOption(DataItem *item);
    bool saveOptionValue(DataItem *item);

    /* Методы запроса списка объектов */
    template <class T>
    /* Обобщенный метод. Возвращает true - при удачном запросе */
    bool getListData(const QString &apiName, SEVector<T *> *list,
                     const Request &request = Request(), const bool &useCache = false);
    /* Заполнят список групп контактов */
    bool getListContactsGroups(ListDataItems *listGroups);
    /* Запоняет список контактов */
    bool getListContacts(const Request &request, ListContacts *listContacts);
    /* Получить список пользователей */
    bool getListUsers(const Request &request, ListContacts *listUsers);
    /* Получить список заказов */
    bool getListOrders(const Request &request, ListOrders *listOrders, bool useCache = false);
    /* Получить список предзаказов */
    bool getListPreorders(const Request &request, ListOrders *listOrders, bool useCache = false);
    /* Получить список заявок на кредит */
    bool getListCreditRequests(const Request &request, ListOrders *listOrders, bool useCache = false);
    /* Получить список платежей по заказам */
    bool getListPayments(const Request &request, ListPayments *listPayments);
    /* Получить список статусов заказов */
    bool getListOrdersStatuses(ListDataItems *listStatuses);
    /* Получить список статусов доставок */
    bool getListDeliveriesStatuses(ListDataItems *listStatuses);
    /* Получить список товаров */
    bool getListProducts(const Request &request, ListProducts *listProducts, bool useCache = false);
    /* Получить список брендов  */
    bool getListBrands(const Request &request, ListDataItems *listBrands);
    /* Получить список групп товаров */
    bool getListProductsGroups(const Request &request, ListProductsGroups *listProducts, bool useCache = false);
    /* Получить список групп параметров товара */
    bool getListFeaturesGroups(ListDataItems *listFeatures);
    /* Получить список списковых параметров товара */
    bool getListFeaturesValues(const Request &request, ListFeatures *listFeatures);
    /* Получить список параметров товара */
    bool getListFeatures(const Request &request, ListFeatures *listFeatures);
    /* Получить список платежных систем */
    bool getListPaySystems(ListDataPaySystems *listPaysSystems);
    /* Получить список плагинов платежных систем */
    bool getListPaySystemsPlugins(ListDataItems *listPlugins);
    /* Получить список платежных реквизитов компании */
    bool getListPayrequisites(const Request &request, ListDataItems *listRequisites);
    /* Получить список доставок */
    bool getListDeliveries(ListDeliveries *listDeliveries);
    /* Получить список типов доставок */
    bool getListTypesDeliveries(ListTypesDeliveries *listTypes);
    /*Получить список лицензий*/
    bool getListLicenses(ListLicense *listLicense);
    /*Получить список валют*/
    bool getListCurrencies(ListCurrencies *listCurrencies);
    bool getListCurrencies(const Request &request, ListCurrencies *listCurrencies);
    /*Получить список производителей*/
    bool getListManufacturers(ListDataItems *listManufacturers);
    /*Получить список групп модификаций*/
    bool getListModificationsGroups(ListModificationsGroups *listModifications);
    /*Получить список типов цен модификаций*/
    bool getListModificationsPricesTypes(ListDataItems *listTypes);
    /* Получить список типов параметров (list, colorlist, number) */
    bool getListFeaturesTypes(const Request &request, ListDataItems *listTypes);
    /* Получить список комментариев товаров */
    bool getListComents(const Request &request, ListDataItems *listComments);
    /* Получить список отзывов о товарах */
    bool getListReviews(const Request &request, ListReviews *listReviews);
    /* Получить список специальных предложений */
    bool getListSpecialOffer(const Request &request, ListProducts *listProducts);    
    /* Получить список групп шаблонов писем */
    bool getListTemplatesLetters(ListLettersGroups *listGroups);
    /* Получить список групп новостей */
    bool getListNewsGroups(ListDataItems *listGroups);
    /* Получить список новостей */
    bool getListNews(const Request &request, ListNews *listNews);
    /* Получить список купонов */
    bool getListCoupons(const Request &request, ListCoupons *listCoupons);
    /* Получить список скидок */
    bool getListDiscounts(ListDiscounts *listDiscounts);
    /* Получить список фильтров параметров для групп товаров */
    bool getListParametersFilters(ListDataItems *listFilters);
    /* Получить список реквизитов компании */
    bool getListCompanyRequisites(ListCompanyRequisites *listRequisites);
    /* Получить список городов */
    bool getListCities(const Request &request, ListDataItems *listCities);
    /* Получить список регионов */
    bool getListRegions(ListDataItems *listRegions);
    /* Получить список стран */
    bool getListCountries(ListDataItems *listCountries);
    /* Получить список интеграций */
    bool getListIntegrations(ListIntegrations *listIntegrations);
    /* Получить список интеграций с сервисами */
    bool getListIntegrationsServices(ListDataItems *listIntegrations);
    /* Получить список параметров магазина */
    bool getListShopParameters(ListIntegrations *listShopParameters);
    /* Получить список типов операций по аккаунтам (лицевым счетам) */
    bool getListTypeOperationsAccounts(ListDataItems *listTypes);
    /* Получить список групп для динамических полей пользователя */
    bool getListDynFieldsGroups(const Request &request, ListDataItems *listGroups);
    /* Получить список динамических полей пользователя */
    bool getListDynFields(const Request &request, ListDataItems *listFields);
    /* Получить список переменных конструктора автоматического формировавния статических мета тегов */
    bool getListSEOTagsVars(const Request &request,ListDataItems *listVars);
    /* Получить список переменных конструктора автоматического формировавния динамических мета тегов */
    bool getListSEODynVars(const Request &request,ListDataItems *listVars);
    /* Получить список секций для настройки модулей SiteEdit */
    bool getListSESections(const Request &request, ListDataItems *listSections);
    /* Получить список элементов секций для настройки модулей SiteEdit */
    bool getListSESectionsItems(const Request &request, ListDataItems *listSectionsItems);
    /* Получить список групп опций товара */
    bool getListOptionsGroups(ListDataItems *listOptionsGroups);
    /* Получить список опций товара */
    bool getListOptions(ListOptions *listOptions);
    /* Получить список значений опций товара */
    bool getListOptionsValues(const Request &request, ListOptionValues *listOptionsValues);

    bool getListProjects(ListDataItems *projects);

    /* Методы запроса объекта */
    bool getInfoData(const QString &apiName, DataItem *data, bool useCache = false);
    bool getInfoContact(DataContact *contact);
    bool getInfoProductGroup(DataProductsGroup *group);
    bool getInfoFeatureGroup(DataItem *group);
    bool getInfoFeature(DataFeature *feature);
    bool getInfoFeatureValue(DataFeature *feature);
    bool getInfoBrand(DataItem *brand);
    bool getInfoMain(DataMain *dataMain);
    bool getInfoLicense(DataLicense *license);
    bool getInfoDelivery(DataDelivery *delivery);
    bool getInfoProduct(DataProduct *product, bool useCache = false);
    bool getInfoPaySystem(DataPaySystem *paySystem);
    bool getInfoNewsGroup(DataItem *group);
    bool getInfoNews(DataNew *news);
    bool getInfoDiscounts(DataDiscount *discount);
    bool getInfoOrder(DataOrder *order, bool useCache = false);
    bool getInfoPreorder(DataOrder *order, bool useCache = false);
    bool getInfoCreditRequest(DataOrder *order, bool useCache = false);
    bool getInfoPayment(DataPayment *payment);
    bool getInfoCoupon(DataCoupon *coupon);
    bool getInfoCurrencyRate(DataCurrency *currency);
    bool getInfoModificationGroup(DataModificationGroup *group);
    bool getInfoOptionGroup(DataItem *group);
    bool getInfoOption(DataItem *item);
    bool getInfoOptionValue(DataItem *item);


    /* Методы удаления объектов */
    bool deleteItem(const QString &apiName, const Request &request);
    bool deleteProductsGroups(const Request &request);
    bool deleteContactGroups(const Request &request);
    bool deleteContacts(const Request &request);
    bool deleteCompanies(const Request &request);
    bool deleteFeatures(const Request &request);
    bool deleteFeaturesValues(const Request &request);
    bool deleteFeaturesGroups(const Request &request);
    bool deleteImages(const Request &request);
    bool deleteBrands(const Request &request);
    bool deletePayParams(const Request &request);
    bool deleteLicenses(const Request &request);
    bool deleteDeliveries(const Request &request);
    bool deleteModificationsGroups(const Request &request);
    bool deleteComments(const Request &request);
    bool deleteProducts(const Request &request);
    bool deleteSpecialOffer(const Request &request);
    bool deleteCountries(const Request &request);
    bool deleteRegions(const Request &request);
    bool deleteCities(const Request &request);
    bool deletePaySystems(const Request &request);
    bool deleteTemplateLetter(const Request &request);
    bool deleteTemplateLetterGroup(const Request &request);
    bool deleteNewsGroups(const Request &request);
    bool deleteNews(const Request &request);
    bool deleteCurrencies(const Request &request);
    bool deleteCoupons(const Request &request);
    bool deleteDiscounts(const Request &request);
    bool deleteOrders(const Request &request);
    bool deletePreorders(const Request &request);
    bool deleteCreditRequests(const Request &request);
    bool deletePayments(const Request &request);
    bool deleteManager(const Request &request);
    bool deleteReviews(const Request &request);
    bool deleteDynFieldsGroup(const Request &request);
    bool deleteDynFields(const Request &request);
    bool deleteSESectionItem(const Request &request);
    bool deleteSEODynVars(const Request &request);
    bool deleteOptionsGroups(const Request &request);
    bool deleteOptions(const Request &request);
    bool deleteOptionsValues(const Request &request);


    /* Методы групповых операций */
    bool sortFeatureGroups(const Request &request);
    bool sortFeatures(const Request &request);
    bool sortFeaturesValues(const Request &request);
    bool setSEOFeatures(const Request &request);
    bool sortModificationsGroups(const Request &request);
    bool sortPaySystems(const Request &request);
    bool sortProductsGroups(const Request &request);
    bool sortDeliveriesTypes(const Request &request);
    bool sortDynFieldsGroup(const Request &request);
    bool sortDynField(const Request &request);
    bool sortSESectionItem(const Request &request);
    bool sortDiscounts(const Request &request);
    bool sortBrands(const Request &request);
    bool sortOptionGroups(const Request &request);
    bool sortOption(const Request &request);
    bool sortOptionValues(const Request &request);

    bool getBinaryData(const QString &apiName, const QString &apiMethod, const Request &params, QByteArray *data);
    QString sendBinaryData(const QString &apiName, const QString &apiMethod, const QString &params, QByteArray *data = 0, const QString &fileName = QString());
    bool exportData(const QString &apiName, const Request &params, QByteArray *data);
    bool exportItemData(const QString &apiName, const Request &params, QByteArray *data);
    bool getPaymentCheck(const Request &params, QByteArray *data);
    bool getShipmentBlank(const Request &params, QByteArray *data);
    QString importData(const QString &apiName, const QString &params, QByteArray *data,
                       const QString &fileName);
    bool importProductFromYMUrl(DataProduct *product);
    bool restoreOrder(DataOrder *item);

    QPixmap getImageFromURL(const QString &URL);
    QByteArray getContentFromUrl(const QString &URL);
    void clearCache();

    QStringList translit(const QStringList list);
    bool setCodeUrlProduct(const DataItem *item);
    bool setCodeUrlProductGroup(const DataItem *item);
    bool setArticleProduct(const DataItem *item);
    bool addProductPrice(const DataProduct *product);

    const QString uploadImageToYandex(const QString &fileName);
    const QString getLastError() const;

    bool executeAPI(const QString &apiName, const QString &apiMethod,
                    const QString &params, QJsonObject *answerJSON,
                    const bool &isBlockingMode = true, const bool &useCache = false);

signals:
    void finishAuth(bool isAuth, const QString &answer);
    void finishedRequest(bool status, QString message = QString());

public slots:    
    void onFinished(QNetworkReply* reply);        
    void onProxyAuthentication(const QNetworkProxy & proxy, QAuthenticator * authenticator);    
    void requestInThread(const QString &apiObject, const QString &apiMethod, const Request &apiRequest);
    void requestInThread(const QString &apiObject, const QString &apiMethod, const DataItem &item);
    void requestInThread(const QString &apiObject, const QString &apiMethod, const QString &apiRequestParams);

    void updateApiExt();

private slots:
    void executeRequestInThread();

private:
    void setAuthParamsForRequest(QNetworkRequest &request);
    void setProxyParams();

    bool readApiReply(QNetworkReply *reply, const QString &url, const QString &params,
                      QJsonObject *requestJSON);

    /* isBlockingMode - блокирующий режим */   
    bool executeAPI(const QString &apiName, const QString &apiMethod,
                    const Request &request, QJsonObject *answerJSON,
                    const bool &isBlockingMode = true, const bool &useCache = false);
    bool getFromCache(const QString &url, const QString &params, QJsonObject *answerJSON);


    QJsonObject returnRequest;

    QString editorWYSWIG;
    QString editorHTMLCode;

    QString apiRequestParams;
    QString apiMethod;
    QString apiObject;    
    QString lastError;
};

#endif // API_H
