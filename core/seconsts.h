#include <QString>

#ifndef SECONSTS_H
#define SECONSTS_H

class SEConsts
{
public:
    SEConsts();

    enum ImagesSection {
        Section_Products = 0,
        Section_GroupsProducts = 1,
        Section_News = 2,
        Section_Brands = 3,
        Section_Features = 4,
        Section_ShopPayment = 5,
        Section_Contacts = 6,
        Section_ShopSections = 7,
        Section_YandexFotos = 8,
        Section_Labels = 9
    };

    static const int MAX_COUNT_MODIFICATIONS;    

    static const int IMAGE_SIZE_PREVIEW;    
    static const int MAX_COUNT_ATTEMPTS_IMAGE_LOAD;

    static const int PERMISSION_MASK_SIZE;

    static const QString SHADOW_COLOR;

    static const QString UPDATE_TYPE_VERSION;
    static const QString UPDATER_NAME;
    static const QString UPDATER_KEY;

    static const QString URL_PERMIT;
    static const QString URL_AUTH;
    static const QString FILE_ADMIN;

    static const QString DIR_PROJECTS;
    static const QString DIR_STATESWIDGETS;
    static const QString DIR_IMAGES;    
    static const QString DIR_IMAGES_PRODUCTS;
    static const QString DIR_IMAGES_GROUPSPRODUCTS;
    static const QString DIR_IMAGES_NEWS;
    static const QString DIR_IMAGES_BRANDS;
    static const QString DIR_IMAGES_FEATURES;
    static const QString DIR_IMAGES_PAYMENT;
    static const QString DIR_IMAGES_CONTACTS;
    static const QString DIR_IMAGES_SHOPSECTIONS;
    static const QString DIR_IMAGES_YANDEXPHOTOS;
    static const QString DIR_IMAGES_LABELS;
    static const QString DIR_EXPORTS;
    static const QString DIR_DUMPS;
    static const QString DIR_CACHE;

    static const QString ECHO_PASSWORDTEXT;    

    static const QString API_PATH;
    static const QString API_PATH_DEVELOPMENT;
    static const QString API_AUTH;
    static const QString API_GROUPSCONTACTS;
    static const QString API_CONTACTS;    
    static const QString API_COMPANIES;
    static const QString API_ORDERS;
    static const QString API_PREORDERS;
    static const QString API_CREDIT_REQUESTS;
    static const QString API_ORDERS_PAYMENTS;
    static const QString API_ORDERS_STATUSES;
    static const QString API_DELIVERIES_STATUSES;
    static const QString API_PRODUCTS;
    static const QString API_PRODUCTS_GROUPS;
    static const QString API_BRANDSPRODUCTS;
    static const QString API_GROUPSFEATURES;
    static const QString API_VALUESFEATURES;
    static const QString API_SPECIALPRODUCTS;
    static const QString API_FEATURES;
    static const QString API_FILESIMAGES;
    static const QString API_FILES;
    static const QString API_SETTINGSMAIN;
    static const QString API_PAYSYSTEMS;
    static const QString API_PAYSYSTEMS_PLUGINS;
    static const QString API_PAYREQUISITES;
    static const QString API_DELIVERIES;
    static const QString API_DELIVERIES_TYPES;
    static const QString API_LICENSES;
    static const QString API_PROJECTS;
    static const QString API_CURRENCIES;
    static const QString API_CURRENCIES_RATE;
    static const QString API_MANUFACTURERS;
    static const QString API_GROUPSMODIFICATIONS;
    static const QString API_MODIFICATIONSPRICESTYPES;
    static const QString API_FEATURESTYPES;
    static const QString API_COMMENTS;
    static const QString API_REVIEWS;
    static const QString API_FUNCTIONS;
    static const QString API_COUNTRIES;
    static const QString API_REGIONS;
    static const QString API_CITIES;
    static const QString API_LETTERSTEMPLATES;
    static const QString API_GROUPSLETTERSTEMPLATES;
    static const QString API_GROUPSNEWS;
    static const QString API_NEWS;
    static const QString API_COUPONS;
    static const QString API_DISCOUNTS;
    static const QString API_PARAMETERS_FILTERS;    
    static const QString API_COMPANY_REQUISITES;
    static const QString API_INTEGRATIONS;    
    static const QString API_SHOP_PARAMETERS;
    static const QString API_ACCOUNTS_TYPE_OPERATIONS;
    static const QString API_UI_DYNFIELDS_GROUPS;
    static const QString API_UI_DYNFIELDS;
    static const QString API_SEO_TAGSVARS;
    static const QString API_SEO_DYNVARS;
    static const QString API_SE_SECTIONS;
    static const QString API_SE_SECTIONS_ITEMS;
    static const QString API_MIGRATION;
    static const QString API_INTEGRATIONS_SERVICES;
    static const QString API_PRODUCTS_TYPES;
    static const QString API_PRODUCTS_LABELS;
    static const QString API_PERMISSION_ROLES;
    static const QString API_PERMISSION_USERS;
    static const QString API_PERMISSION;
    static const QString API_GEOTARGETING_CONTACTS;
    static const QString API_IMPORT_PROFILE;
    static const QString API_MEASURES_WEIGHT;
    static const QString API_MEASURES_VOLUME;
    static const QString API_GROUPSOPTIONS;
    static const QString API_OPTIONS;
    static const QString API_OPTIONSVALUES;

    static const QString APIMETHOD_REGISTER;
    static const QString APIMETHOD_FETCH;
    static const QString APIMETHOD_INFO;    
    static const QString APIMETHOD_SAVE;
    static const QString APIMETHOD_DELETE;        
    static const QString APIMETHOD_UPLOAD;
    static const QString APIMETHOD_SORT;
    static const QString APIMETHOD_SEO;
    static const QString APIMETHOD_SETCODEURL;
    static const QString APIMETHOD_SETARTICLE;
    static const QString APIMETHOD_ADDPRODUCTPRICE;
    static const QString APIMETHOD_DOC;
    static const QString APIMETHOD_BLANK;
    static const QString APIMETHOD_EXPORT;
    static const QString APIMETHOD_EXPORT_ITEM;    
    static const QString APIMETHOD_IMPORT;
    static const QString APIMETHOD_COMMAND;
    static const QString APIMETHOD_YM_IMPORT;
    static const QString APIMETHOD_SEND;
    static const QString APIMETHOD_RESTORE;

    static const QString SHOPPARAMS_SIZEPICTURE;

};

#endif // SECONSTS_H
