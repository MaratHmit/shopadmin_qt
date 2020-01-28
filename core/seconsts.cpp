#include "seconsts.h"

SEConsts::SEConsts()
{

}

const QString SEConsts::URL_PERMIT = "http://dverra.ru/se.php";
const QString SEConsts::URL_AUTH = "http://api.siteedit.ru";
const QString SEConsts::FILE_ADMIN = "admin.dat";

const int SEConsts::MAX_COUNT_MODIFICATIONS = 1000;

const int SEConsts::IMAGE_SIZE_PREVIEW = 64;
const int SEConsts::MAX_COUNT_ATTEMPTS_IMAGE_LOAD = 3;

const int SEConsts::PERMISSION_MASK_SIZE = 4;

const QString SEConsts::UPDATE_TYPE_VERSION = "224";
const QString SEConsts::UPDATER_NAME = "updaterSE.exe";
const QString SEConsts::UPDATER_KEY = "72D05246";

const QString SEConsts::SHADOW_COLOR = "#FFFFEB";

const QString SEConsts::DIR_PROJECTS = "projects";
const QString SEConsts::DIR_STATESWIDGETS = "uistates";
const QString SEConsts::DIR_IMAGES = "images";
const QString SEConsts::DIR_IMAGES_PRODUCTS = "shopprice";
const QString SEConsts::DIR_IMAGES_GROUPSPRODUCTS = "shopgroup";
const QString SEConsts::DIR_IMAGES_NEWS = "newsimg";
const QString SEConsts::DIR_IMAGES_BRANDS = "shopbrand";
const QString SEConsts::DIR_IMAGES_FEATURES = "shopfeature";
const QString SEConsts::DIR_IMAGES_PAYMENT = "shoppayment";
const QString SEConsts::DIR_IMAGES_CONTACTS = "contacts";
const QString SEConsts::DIR_IMAGES_LABELS = "labels";
const QString SEConsts::DIR_IMAGES_SHOPSECTIONS = "shopsections";
const QString SEConsts::DIR_IMAGES_YANDEXPHOTOS = "yandexphotos";
const QString SEConsts::DIR_EXPORTS = "exports";
const QString SEConsts::DIR_DUMPS = "dumps";
const QString SEConsts::DIR_CACHE = "cache";

const QString SEConsts::ECHO_PASSWORDTEXT = "**********";

const QString SEConsts::API_PATH = "api/1";
const QString SEConsts::API_PATH_DEVELOPMENT = "api/1_dev";
const QString SEConsts::API_AUTH = "Auth";
const QString SEConsts::API_GROUPSCONTACTS = "Contacts/Groups";
const QString SEConsts::API_CONTACTS = "Contacts";
const QString SEConsts::API_COMPANIES = "Companies";
const QString SEConsts::API_ORDERS = "Orders";
const QString SEConsts::API_PREORDERS = "Preorders";
const QString SEConsts::API_CREDIT_REQUESTS = "CreditRequest";
const QString SEConsts::API_ORDERS_PAYMENTS = "Orders/Payments";
const QString SEConsts::API_ORDERS_STATUSES = "Orders/OrdersStatuses";
const QString SEConsts::API_DELIVERIES_STATUSES = "Orders/DeliveriesStatuses";
const QString SEConsts::API_PRODUCTS = "Products";
const QString SEConsts::API_PARAMETERS_FILTERS = "Products/Groups/ParametersFilters";
const QString SEConsts::API_PRODUCTS_GROUPS = "Products/Groups";
const QString SEConsts::API_PRODUCTS_LABELS = "Products/Labels";
const QString SEConsts::API_BRANDSPRODUCTS = "Products/Brands";
const QString SEConsts::API_GROUPSFEATURES = "Products/Features/Groups";
const QString SEConsts::API_FEATURES = "Products/Features";
const QString SEConsts::API_VALUESFEATURES = "Products/Features/Values";
const QString SEConsts::API_SPECIALPRODUCTS = "Products/SpecialOffer";
const QString SEConsts::API_COUPONS = "Products/Coupons";
const QString SEConsts::API_DISCOUNTS = "Products/Discounts";
const QString SEConsts::API_FILESIMAGES = "Images";
const QString SEConsts::API_FILES = "Files";
const QString SEConsts::API_SETTINGSMAIN = "Main";
const QString SEConsts::API_PAYSYSTEMS = "Paysystems";
const QString SEConsts::API_PAYSYSTEMS_PLUGINS = "Paysystems/Plugins";
const QString SEConsts::API_PAYREQUISITES = "Payrequisites";
const QString SEConsts::API_DELIVERIES = "Deliveries";
const QString SEConsts::API_DELIVERIES_TYPES = "Deliveries/Types";
const QString SEConsts::API_LICENSES = "License";
const QString SEConsts::API_PROJECTS = "Projects";
const QString SEConsts::API_CURRENCIES = "Currencies";
const QString SEConsts::API_CURRENCIES_RATE = "Currencies/Rate";
const QString SEConsts::API_MANUFACTURERS = "Manufacturers";
const QString SEConsts::API_GROUPSMODIFICATIONS = "Products/Modifications/Groups";
const QString SEConsts::API_MODIFICATIONSPRICESTYPES = "Products/Modifications/PricesTypes";
const QString SEConsts::API_FEATURESTYPES = "Products/Features/Types";
const QString SEConsts::API_COMMENTS = "Products/Comments";
const QString SEConsts::API_REVIEWS = "Products/Reviews";
const QString SEConsts::API_FUNCTIONS = "Functions";
const QString SEConsts::API_COUNTRIES = "Countries";
const QString SEConsts::API_REGIONS = "Regions";
const QString SEConsts::API_CITIES = "Cities";
const QString SEConsts::API_LETTERSTEMPLATES = "LettersTemplates";
const QString SEConsts::API_GROUPSLETTERSTEMPLATES = "LettersTemplates/Groups";
const QString SEConsts::API_GROUPSNEWS = "News/Groups";
const QString SEConsts::API_NEWS = "News";
const QString SEConsts::API_COMPANY_REQUISITES = "Contacts/CompanyRequisites";
const QString SEConsts::API_INTEGRATIONS = "Integrations";
const QString SEConsts::API_SHOP_PARAMETERS = "ShopParameters";
const QString SEConsts::API_ACCOUNTS_TYPE_OPERATIONS = "Accounts/TypesOperations";
const QString SEConsts::API_UI_DYNFIELDS_GROUPS = "UiSettings/DynFields/Groups";
const QString SEConsts::API_UI_DYNFIELDS = "UiSettings/DynFields";
const QString SEConsts::API_SEO_TAGSVARS = "SEO/TagsVars";
const QString SEConsts::API_SEO_DYNVARS = "SEO/DynVars";
const QString SEConsts::API_SE_SECTIONS = "SESections";
const QString SEConsts::API_SE_SECTIONS_ITEMS = "SESections/Items";
const QString SEConsts::API_MIGRATION = "Migration";
const QString SEConsts::API_INTEGRATIONS_SERVICES = "Integrations/Services";
const QString SEConsts::API_PRODUCTS_TYPES = "Products/Types";
const QString SEConsts::API_PERMISSION_ROLES = "Permission/Roles";
const QString SEConsts::API_PERMISSION_USERS = "Permission/Users";
const QString SEConsts::API_PERMISSION = "Permission";
const QString SEConsts::API_GEOTARGETING_CONTACTS = "Geotargeting";
const QString SEConsts::API_IMPORT_PROFILE = "ImportProfile";
const QString SEConsts::API_MEASURES_WEIGHT = "Measures/Weight";
const QString SEConsts::API_MEASURES_VOLUME = "Measures/Volume";
const QString SEConsts::API_GROUPSOPTIONS = "Products/Options/Groups";
const QString SEConsts::API_OPTIONS = "Products/Options";
const QString SEConsts::API_OPTIONSVALUES = "Products/Options/Values";

const QString SEConsts::APIMETHOD_REGISTER = "Register.api";
const QString SEConsts::APIMETHOD_FETCH = "Fetch.api";
const QString SEConsts::APIMETHOD_INFO = "Info.api";
const QString SEConsts::APIMETHOD_SAVE = "Save.api";
const QString SEConsts::APIMETHOD_DELETE = "Delete.api";
const QString SEConsts::APIMETHOD_UPLOAD = "Upload.api";
const QString SEConsts::APIMETHOD_SORT = "Sort.api";
const QString SEConsts::APIMETHOD_SEO = "SEO.api";
const QString SEConsts::APIMETHOD_SETCODEURL = "SetCodeUrl.api";
const QString SEConsts::APIMETHOD_SETARTICLE = "SetArticle.api";
const QString SEConsts::APIMETHOD_ADDPRODUCTPRICE = "AddPrice.api";
const QString SEConsts::APIMETHOD_DOC = "Doc.api";
const QString SEConsts::APIMETHOD_BLANK = "Blank.api";
const QString SEConsts::APIMETHOD_EXPORT = "Export.api";
const QString SEConsts::APIMETHOD_EXPORT_ITEM = "ExportItem.api";
const QString SEConsts::APIMETHOD_IMPORT = "Import.api";
const QString SEConsts::APIMETHOD_COMMAND = "Command.api";
const QString SEConsts::APIMETHOD_YM_IMPORT = "YMImport.api";
const QString SEConsts::APIMETHOD_SEND = "Send.api";
const QString SEConsts::APIMETHOD_RESTORE = "Restore.api";

const QString SEConsts::SHOPPARAMS_SIZEPICTURE = "size_picture";
