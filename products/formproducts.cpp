#include "formproducts.h"
#include "ui_formproducts.h"
#include "seutils.h"
#include "seconfig.h"
#include "apiadapter.h"
#include "formproductsgroups.h"
#include "framecardspecifications.h"
#include "framecardmodifications.h"
#include "formbrands.h"
#include "framecardsimilar.h"
#include "framecardcrossgroups.h"
#include "dialogproductmodifications.h"
#include "dialogaddprice.h"
#include "dialogexport.h"
#include "dialogimport.h"
#include "dialogsetprices.h"
#include "dialogsetrates.h"
#include "userpermission.h"
#include "dialogproductsmasterimport.h"
#include "framecardoptions.h"

#include <QScrollBar>
#include <QPushButton>
#include <QComboBox>
#include <QLineEdit>
#include <QTreeWidgetItemIterator>
#include <QMenu>

FormProducts::FormProducts(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FormProducts)
{
    ui->setupUi(this);    
    ui->stackedWidgetMain->setCurrentIndex(0);

    setHidden(true);
    isSelectMode = false;    

    initVariables();
    initNavigator();    
    initActions();
    initSignals();    
    setGroupsMode(false);
    initPermissions();
}

FormProducts::FormProducts(QWidget *parent, const bool &isMultiSelect, const bool &isSelectModification) :
    QDialog(parent),
    ui(new Ui::FormProducts)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Window);
    ui->stackedWidgetMain->setCurrentIndex(0);
    this->isSelectModification = isSelectModification;
    initVariables();
    initNavigator();
    initActions();
    initSignals();    
    setGroupsMode(false);
    isSelectMode = true;
    ui->frameButtons->hide();
    ui->actionDeleteProducts->setVisible(false);
    ui->actionSelect->setVisible(true);
    ui->toolButtonSelect->show();    
    if (!isMultiSelect)
        ui->treeViewProducts->setSelectionMode(QAbstractItemView::SingleSelection);
    initPermissions();
}

FormProducts::~FormProducts()
{
    delete ui;

    selectedProducts->deleteItems();
    products->deleteItems();
    api->deleteLater();
    apiInfo->deleteLater();
    mListLabels->deleteItems();

    delete selectedProducts;
}

void FormProducts::refreshData()
{
    if (!UserPermission::isRead(UserPermission::PRODUCTS))
        return;

    initFilterBrands();
    ui->widgetGroups->refreshData();
    refreshDataProducts();
}

void FormProducts::setSystemSearchText(const QString &searchText)
{
    mSystemSearchText = searchText;
}

const ListProducts *FormProducts::getSelectedProducts() const
{    
    return selectedProducts;
}

void FormProducts::onChangeProduct()
{
    if (modelProducts->rowCount() > 0 &&
            ui->treeViewProducts->currentIndex().isValid())
        navigatorProducts->setNumRecord(numPage * navigatorProducts->getLimit() +
                                         ui->treeViewProducts->currentIndex().row() + 1,
                                         countProducts);
    else navigatorProducts->setNumRecord(0, 0);

    QModelIndex index = ui->treeViewProducts->currentIndex();

    ui->actionAddProduct->setEnabled(isAddPermission);
    ui->actionEditProduct->setEnabled(index.isValid() && isEditPermission);
    ui->actionDeleteProducts->setEnabled(index.isValid() && isDeletePermission);
    ui->actionSelect->setEnabled(index.isValid());
    ui->actionAddPrice->setEnabled(index.isValid() && isEditPermission);
    ui->actionIsYAMarket->setEnabled(index.isValid() && isEditPermission);

    if (timerShowImage->isActive())
        timerShowImage->stop();
    timerShowImage->start(300);
}

void FormProducts::onClickHeaderItem(int )
{
    refreshDataProducts();
}

void FormProducts::runSearchProducts()
{
    if (timerSearch->isActive())
        timerSearch->stop();    
    numPage = 0;
    timerSearch->start(300);
}

void FormProducts::onDoubleClicked()
{
    if (!isSelectMode) {
        if (ui->actionEditProduct->isEnabled())
            ui->actionEditProduct->trigger();
    } else
        ui->actionSelect->trigger();
}

void FormProducts::goNextPage()
{
    numPage++;
    if (numPage > int(countProducts / navigatorProducts->getLimit()))
        numPage = int(countProducts / navigatorProducts->getLimit());
    refreshDataProducts();
}

void FormProducts::goPreviosPage()
{
    numPage--;
    if (numPage < 0)
        numPage = 0;
    refreshDataProducts();
}

void FormProducts::goLastPage()
{
    numPage = int(countProducts / navigatorProducts->getLimit());
    refreshDataProducts();
}

void FormProducts::goFirstPage()
{
    numPage = 0;
    refreshDataProducts();
}

void FormProducts::reject()
{
    if (isSelectMode)
        QDialog::reject();
}

void FormProducts::showEvent(QShowEvent *)
{
    if (!isShown) {
        setSizeButtons();
        SEConfig::loadSplitterLayout(ui->splitterMainProducts);
        restoreStateTable();     
        initFilterBrands();
        refreshDataProducts();      
    };        
}

void FormProducts::hideEvent(QHideEvent *)
{
    if (isShown)
        saveStateTable();
}

void FormProducts::keyPressEvent(QKeyEvent *e)
{
    if (e->modifiers() & Qt::ControlModifier) {        
        switch (e->key()) {
        case 1042:
        case 'D':  if (ui->actionSetIsDiscount->isEnabled())
                ui->actionSetIsDiscount->trigger();
            break;
        case 1058:
        case 'N':  if (ui->actionSetNew->isEnabled())
                ui->actionSetNew->trigger();
            break;
        case 1056:
        case 'H':  if (ui->actionSetHit->isEnabled())
                ui->actionSetHit->trigger();
            break;
        case 1055:
        case 'G':  if (ui->actionChangeGroup->isEnabled())
                ui->actionChangeGroup->trigger();
            break;
        case 1068:
        case 'M':  if (ui->actionSetModifications->isEnabled())
                ui->actionSetModifications->trigger();
            break;
        case 1044:
        case 'L':  if (ui->actionSetSpecifications->isEnabled())
                ui->actionSetSpecifications->trigger();
            break;
        case 1048:
        case 'B':  if (ui->actionSetBrand->isEnabled())
                ui->actionSetBrand->trigger();
            break;
        case 1050:
        case 'R':  if (ui->actionSetAccompsProducts->isEnabled())
                ui->actionSetAccompsProducts->trigger();
            break;
        case 1067:
        case 'S':  if (ui->actionSetSimilarProducts->isEnabled())
                ui->actionSetSimilarProducts->trigger();
            break;
        case 1045:
        case 'P':  if (ui->actionAddPrice->isEnabled())
                ui->actionAddPrice->trigger();
            break;
        case 'Y':  if (ui->actionIsYAMarket->isEnabled())
                ui->actionIsYAMarket->trigger();
            break;        
        default:
            break;
        }
    }    

    if (e->key() == 16777265)
        onAddFromYMUrl();

    QDialog::keyPressEvent(e);
}

void FormProducts::setFilterSelectedProduct(DataProduct *product)
{
    product->setProperty("filter", requestProducts.filter());
}

void FormProducts::on_pushButtonGroups_clicked()
{
    ui->stackedWidgetMain->setCurrentWidget(ui->pageProducts);
    ui->widgetGroups->show();
    setGroupsMode(true);
}

void FormProducts::on_pushButtonProducts_clicked()
{
    ui->stackedWidgetMain->setCurrentWidget(ui->pageProducts);
    setGroupsMode(false);
}

void FormProducts::refreshDataProducts()
{
    if (!UserPermission::isRead(UserPermission::PRODUCTS))
        return;

    ui->actionEditProduct->setEnabled(false);
    ui->actionDeleteProducts->setEnabled(false);
    timerShowImage->disconnect();

    navigatorProducts->setNumRecord(0, 0);
    SEConfig::saveStateModel(ui->treeViewProducts);
    resetRequestProducts();
    modelImages->unBindItems();
    modelImages->resetModel();
    preloader->startAnimation();
    api->requestInThread(SEConsts::API_PRODUCTS, SEConsts::APIMETHOD_FETCH, requestProducts);
}

void FormProducts::on_pushButtonSubMenu()
{    
    QWidget *widget = 0;

    if (sender() == ui->pushButtonBrands) {
        if (!formBrands) {
            formBrands = new FormBrands(this);
            ui->stackedWidgetMain->addWidget(formBrands);
        }
        widget = formBrands;
    }
    if (sender() == ui->pushButtonFeatures) {
        if (!formFeatures) {
            formFeatures = new FormFeatures(this);
            ui->stackedWidgetMain->addWidget(formFeatures);
        }
        widget = formFeatures;
    }
    if (sender() == ui->pushButtonOptions) {
        if (!formOptions) {
            formOptions = new FormOptions(this);
            ui->stackedWidgetMain->addWidget(formOptions);
        }
        widget = formOptions;
    }
    if (sender() == ui->pushButtonGroupsModifications) {
        if (!formModificationsGroups) {
            formModificationsGroups = new FormModificationsGroups(this);
            ui->stackedWidgetMain->addWidget(formModificationsGroups);
        }
        widget = formModificationsGroups;
    }    
    if (sender() == ui->pushButtonLabels) {
        if (!formLabels) {
            formLabels = new FormCatalog(this, SEConsts::API_PRODUCTS_LABELS,
                                                UserPermission::PRODUCTS);

            formLabels->setSortingEnabled(true);
            formLabels->setWindowTitle("Ярлыки");
            formLabels->setTitleButtonAdd("Создать ярлык", "Создать новый ярлык для товаров");
            formLabels->addField(new Field("id", "Ид.", Qt::AlignRight | Qt::AlignVCenter));
            formLabels->addField(new Field("image", "Фото"));
            formLabels->addField(new Field("name", "Наименование"));
            DialogCardProductLabel *dialogCard = new DialogCardProductLabel(formLabels);
            formLabels->setDialogCard(dialogCard);
            ui->stackedWidgetMain->addWidget(formLabels);
        }
        widget = formLabels;
    }
    if (sender() == ui->pushButtonCoupons) {
        if (!formCoupons) {
            formCoupons = new FormCoupons(this);
            ui->stackedWidgetMain->addWidget(formCoupons);
        }
        widget = formCoupons;
    }
    if (sender() == ui->pushButtonDiscounts) {
        if (!formDiscounts) {
            formDiscounts = new FormDiscounts(this);
            ui->stackedWidgetMain->addWidget(formDiscounts);
        }
        widget = formDiscounts;
    }
    if (sender() == ui->pushButtonGoodsTypes) {
        if (!formProductsTypes) {
            formProductsTypes = new FormCatalog(this, SEConsts::API_PRODUCTS_TYPES,
                                                UserPermission::PRODUCTS);
            formProductsTypes->setWindowTitle("Типы товаров");
            formProductsTypes->setTitleButtonAdd("Создать тип", "Создать новый тип товара");
            formProductsTypes->addField(new Field("id", "Ид.", Qt::AlignRight | Qt::AlignVCenter));
            formProductsTypes->addField(new Field("name", "Наименование"));
            formProductsTypes->addField(new Field("features", "Параметры"));
            DialogCardProductType *dialogCard = new DialogCardProductType(formProductsTypes);
            formProductsTypes->setDialogCard(dialogCard);
            ui->stackedWidgetMain->addWidget(formProductsTypes);
        }
        widget = formProductsTypes;
    }
    if (widget) {
        ui->stackedWidgetMain->setCurrentWidget(widget);
        widget->show();
    }
}

void FormProducts::showContextMenuProducts(QPoint point)
{
    if (!isAddPermission && !isEditPermission && !isDeletePermission)
        return;

    onChangeProduct();
    QPoint globalPos;

    if (sender()->inherits("QAbstractScrollArea"))
        globalPos = ((QAbstractScrollArea*)sender())->viewport()->mapToGlobal(point);
    else globalPos = ((QWidget*)sender())->mapToGlobal(point);

    QMenu menu;

    if (isAddPermission) {
        menu.addAction(ui->actionAddProduct);
        menu.addAction(ui->actionCloneProduct);
    }
    if (isEditPermission)
        menu.addAction(ui->actionEditProduct);
    if (!isSelectMode && isDeletePermission)
        menu.addAction(ui->actionDeleteProducts);
    menu.addSeparator();
    menu.addAction(ui->actionCopy);
    if (isSelectMode) {
        menu.addSeparator();
        menu.addAction(ui->actionSelect);
    }
    if (isEditPermission) {
        menu.addSeparator();
        menu.addAction(ui->actionSetIsDiscount);
        menu.addAction(ui->actionIsYAMarket);
        menu.addSeparator();
        menu.addMenu(mSubMenuOnLabels);
        menu.addMenu(mSubMenuOffLabels);
        menu.addAction(ui->actionOffLabels);
        menu.addSeparator();
        menu.addAction(ui->actionChangeGroup);
        menu.addAction(ui->actionAddDescription);
        menu.addAction(ui->actionSetModifications);        
        if (SEConfig::getMainInfo()->isShowOptions()) {
            menu.addAction(ui->actionSetOptions);
        }
        menu.addAction(ui->actionSetBrand);
        menu.addAction(ui->actionSetAccompsProducts);
        menu.addAction(ui->actionSetSimilarProducts);
        menu.addAction(ui->actionSetCrossGroups);
        menu.addSeparator();
        menu.addAction(ui->actionSetCodeURL);
        menu.addAction(ui->actionSetArticle);
        menu.addSeparator();
        menu.addAction(ui->actionSetSpecifications);
        menu.addAction(ui->actionShowFeatures);
        menu.addAction(ui->actionHideFeatures);
        menu.addSeparator();
        menu.addAction(ui->actionSetPrice);
        menu.addAction(ui->actionAddPrice);
        menu.addAction(ui->actionSetRate);
    }
    menu.exec(globalPos);
}

void FormProducts::onSaveProduct(const DataProduct *product, const bool isNew)
{
    if (!isNew && ui->treeViewProducts->currentIndex().isValid()) {
         if (product->getIds().count() > 1 || isAllSelected())
             refreshDataProducts();
         else {
             DataProduct *editProduct = qobject_cast <DataProduct * > (modelProducts->dataItem(ui->treeViewProducts->currentIndex()));
             editProduct->copy(product);
             modelProducts->setData(modelProducts->index(0, ui->treeViewProducts->currentIndex().row()), product->getId(), Qt::EditRole);
         }
    }
    if (isNew) {        
        DataProduct *newProduct = new DataProduct(product);
        modelProducts->addItem(newProduct, false);
        countProducts++;
        ui->treeViewProducts->selectionModel()->clearSelection();
        goToRowById(0);
    }    
}

void FormProducts::onScrollProducts(int /*value*/)
{

}

void FormProducts::selectAllItems(bool isSelected)
{
//    navigatorProducts->disconnectSelected();
    if (isSelected) {
        ui->treeViewProducts->selectAll();
        navigatorProducts->setSelectedCount(countProducts);
    }
    else {
        ui->treeViewProducts->clearSelection();
        navigatorProducts->setSelectedCount(0);
    }
  //  navigatorProducts->connectSelected();
}

void FormProducts::setGroupsMode(const bool &isGroupMode)
{
    this->isGroupMode = isGroupMode;
    if (isGroupMode) {
        ui->widgetProducts->setHidden(1);
        if (isVisible())
            ui->widgetGroups->setVisibleCheckeds(0);
    }
    else {
       ui->widgetProducts->setHidden(0);
       if (isVisible())
           ui->widgetGroups->setVisibleCheckeds(1);
    }
}

void FormProducts::setVisibleColumns()
{
    if (!formSetColumns)
        formSetColumns = new FormSetVisibleHeaderColumns(
                    this, ui->treeViewProducts->header());
    formSetColumns->exec();
}

void FormProducts::resetRequestProducts()
{
    requestProducts.clear();

    requestProducts.setLimit(navigatorProducts->getLimit());
    requestProducts.setOffset(numPage * navigatorProducts->getLimit());

    requestProducts.clearFilter();
    QString filter;
    QStringList filterList;
    // группы
    if (ui->widgetGroups->getIdsGroupsChecked().size()) {
        QString idsGroups;        
        bool isNullGroup = false;
        for (int i = 0; i < ui->widgetGroups->getIdsGroupsChecked().size(); i++) {
            if (!ui->widgetGroups->getIdsGroupsChecked().at(i).isEmpty()) {
                if (!idsGroups.isEmpty())
                    idsGroups += ",";
                idsGroups += "'" + ui->widgetGroups->getIdsGroupsChecked().at(i) + "'";
            } else isNullGroup |= true;
        }
        if (!idsGroups.isEmpty() && !isNullGroup)
            filterList.append("([idGroup] IN (" + idsGroups + ") OR [idCrossGroup] IN (" + idsGroups + "))");
        if (isNullGroup && !idsGroups.isEmpty())
            filterList.append("(([idGroup] IS NULL) OR ([idGroup] IN (" + idsGroups + ") OR [idCrossGroup] IN (" + idsGroups + ")))");
        if (isNullGroup && idsGroups.isEmpty())
             filterList.append("([idGroup] IS NULL)");
    }
    // фильтры
    if (ui->checkBoxIsYAMarket->isChecked())
        filterList.append("[isYAMarket]=true");
    if (ui->checkBoxIsNew->isChecked())
        filterList.append("[isNew]=true");
    if (ui->checkBoxIsHit->isChecked())
        filterList.append("[isHit]=true");
    if (ui->checkBoxIsSpec->isChecked())
        filterList.append("[isSpecial]=true");
    if (ui->checkBoxIsActive->isChecked())
        filterList.append("[isActive]=true");
    if (ui->checkBoxIsNotActive->isChecked())
        filterList.append("[isActive]=false");
    if (ui->checkBoxIsDiscount->isChecked())
        filterList.append("[isDiscount]=true");
    if (ui->checkBoxIsNoDiscount->isChecked())
        filterList.append("[isDiscount]=false");
    if (ui->comboBoxFilterBrands->currentIndex() > 0)
        filterList.append(QString("[idBrand]=" + ui->comboBoxFilterBrands->currentData().toString()));
    if (ui->comboBoxFilterCount->currentIndex() > 0) {
        if (ui->comboBoxFilterCount->currentIndex() == 1)
            filterList.append("[isInfinitely]=true");
        if (ui->comboBoxFilterCount->currentIndex() == 2) {
            QString countFrom, countTo;
            countFrom = QString::number(ui->doubleSpinBoxFilterCountFrom->value(), 'f', 2).replace(",", ".");
            countTo = QString::number(ui->doubleSpinBoxFilterCountTo->value(), 'f', 2).replace(",", ".");
            filterList.append("[count]>='" + countFrom + "'");
            filterList.append("[count]<='" + countTo + "'");
        }
    }
    if (ui->comboBoxFilterLabels->currentIndex() > 0)
        filterList.append(QString("[idLabel]=" + ui->comboBoxFilterLabels->currentData().toString()));

    filter = filterList.join(" AND ");
    requestProducts.setFilter(filter);

    if (ui->treeViewProducts->header()->sortIndicatorSection() < fieldsProducts->count()) {
        QString sortField = fieldsProducts->at(ui->treeViewProducts->header()->sortIndicatorSection())->name();
        requestProducts.setSortBy(sortField);
        requestProducts.setAscendingOrder(ui->treeViewProducts->header()->
                                   sortIndicatorOrder() == Qt::AscendingOrder);
    }
    if (mSystemSearchText.isEmpty())
        requestProducts.setSearchText(ui->widgetSearchProducts->searchText().trimmed());
    else {
        requestProducts.setSearchText(mSystemSearchText);
        mSystemSearchText.clear();
    }
}

void FormProducts::initFieldsProducts()
{
    fieldsProducts = new QVector<Field *>();
    fieldsProducts->append(new Field("id", "Ид.", Qt::AlignRight | Qt::AlignVCenter));
    fieldsProducts->append(new Field("isActiveIco", "Вид", Qt::AlignCenter | Qt::AlignVCenter, Qt::DecorationRole));
    fieldsProducts->append(new Field("isNewIco", "Нв", Qt::AlignCenter | Qt::AlignVCenter, Qt::DecorationRole));
    fieldsProducts->append(new Field("isHitIco", "Хит", Qt::AlignCenter | Qt::AlignVCenter, Qt::DecorationRole));
    fieldsProducts->append(new Field("isDiscountIco", "Ск", Qt::AlignCenter | Qt::AlignVCenter, Qt::DecorationRole));
    fieldsProducts->append(new Field("isYAMarket", "Я.М.", Qt::AlignCenter | Qt::AlignVCenter, Qt::DecorationRole));
    fieldsProducts->append(new Field("code", "URL товара"));
    fieldsProducts->append(new Field("article", "Артикул"));
    fieldsProducts->append(new Field("name", "Наименование"));
    fieldsProducts->append(new Field("brand", "Бренд"));
    fieldsProducts->append(new Field("countModifications", "Модиф.", Qt::AlignRight | Qt::AlignVCenter));
    fieldsProducts->append(new Field("price", "Цена", Qt::AlignRight | Qt::AlignVCenter));
    fieldsProducts->append(new Field("count", "Кол-во", Qt::AlignRight | Qt::AlignVCenter));
    fieldsProducts->append(new Field("weight", "Вес", Qt::AlignRight | Qt::AlignVCenter));
    fieldsProducts->append(new Field("nameGroup", "Группа"));
    fieldsProducts->append(new Field("sortIndex", "Популярность", Qt::AlignRight | Qt::AlignVCenter));
    modelProducts->appendFields(fieldsProducts);
}

void FormProducts::initActions()
{
    ui->actionEditProduct->setEnabled(false);
    ui->toolButtonAddProduct->setDefaultAction(ui->actionAddProduct);
    ui->toolButtonEditProduct->setDefaultAction(ui->actionEditProduct);
    ui->toolButtonDeleteProduct->setDefaultAction(ui->actionDeleteProducts);
    ui->toolButtonRefresh->setDefaultAction(ui->actionRefreshProducts);
    ui->toolButtonSelect->setDefaultAction(ui->actionSelect);
    ui->actionSelect->setVisible(false);
    ui->toolButtonSelect->setVisible(false);

    initLabelsActions();
}

void FormProducts::initTreeProducts()
{    
    ui->treeViewProducts->setModel(modelProducts);
    ui->treeViewProducts->header()->setSortIndicator(0, Qt::DescendingOrder);
    ui->treeViewProducts->header()->setSectionsMovable(true);
    ui->treeViewProducts->setAlternatingRowColors(true);
    ui->treeViewProducts->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->treeViewProducts, SIGNAL(customContextMenuRequested(QPoint)),
            SLOT(showContextMenuProducts(QPoint)));

    connect(ui->treeViewProducts->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)), SLOT(onChangeProduct()));
    connect(ui->treeViewProducts->header(), SIGNAL(sectionClicked(int)), SLOT(onClickHeaderItem(int)));
    connect(ui->widgetSearchProducts, SIGNAL(runSearch()), SLOT(runSearchProducts()));
}

void FormProducts::initNavigator()
{
    navigatorProducts = new FramePageNavigator(ui->widgetProducts);
    navigatorProducts->setVisibleAllSelectButton(true);    
    ui->widgetProducts->layout()->addWidget(navigatorProducts);

    connect(navigatorProducts, SIGNAL(goNextPage()), this, SLOT(goNextPage()));
    connect(navigatorProducts, SIGNAL(goPreviosPage()), this, SLOT(goPreviosPage()));
    connect(navigatorProducts, SIGNAL(goLastPage()), this, SLOT(goLastPage()));
    connect(navigatorProducts, SIGNAL(goFirstPage()), this, SLOT(goFirstPage()));
    connect(navigatorProducts, SIGNAL(refreshDataStart()), this, SLOT(goFirstPage()));
    connect(navigatorProducts, SIGNAL(selectAll(bool)), this, SLOT(selectAllItems(bool)));
}

void FormProducts::initVariables()
{
    api = new Api;
    apiInfo = new Api;
    isShown = false;
    isRestoreStates = false;    
    formBrands = 0;
    formFeatures = 0;
    formOptions = 0;
    formModificationsGroups = 0;
    isAutoSelectModification = 0;
    formSetColumns = 0;
    formDiscounts = 0;
    formProductsTypes = 0;    
    formCoupons = 0;    
    formLabels = 0;
    dialogParseFromYM = 0;
    selectedProducts = new ListProducts();
    products = new ListProducts();    
    preloader = new Preloader(this);
    numPage = 0;
    countProducts = 0;
    timerShowImage = new QTimer(this);
    timerShowImage->setSingleShot(true);
    timerSearch = new QTimer(this);
    timerSearch->setSingleShot(true);
    isGroupMode = false;
    mSubMenuOnLabels = 0;
    mSubMenuOffLabels = 0;
    mListLabels = new ListDataItems;
    productInfo = new DataProduct;
    modelProducts = new NameTableModel(this,  reinterpret_cast<ListDataItems *> (products), isSelectMode);
    ui->widgetFilterCount->hide();    
    initFieldsProducts();
    initTreeProducts();    
    modelImages = new ModificationsImagesModel(this);    
    ui->tableViewImages->setModel(modelImages);
    ui->tableViewImages->horizontalHeader()->setDefaultSectionSize(64);
    ui->tableViewImages->verticalHeader()->setDefaultSectionSize(64);
    ui->tableViewImages->horizontalHeader()->hide();
    ui->widgetSearchProducts->setIsHotSearchMode();    
    ui->pushButtonOptions->setVisible(SEConfig::getMainInfo()->isShowOptions());
}

void FormProducts::initSignals()
{    
    connect(ui->widgetGroups, &FormProductsGroups::onItemTreeChanged, this, &FormProducts::onChangeProductGroup);
    connect(ui->pushButtonBrands, &QAbstractButton::clicked, this, &FormProducts::on_pushButtonSubMenu);
    connect(ui->pushButtonCoupons, &QAbstractButton::clicked, this, &FormProducts::on_pushButtonSubMenu);
    connect(ui->pushButtonFeatures, &QAbstractButton::clicked, this, &FormProducts::on_pushButtonSubMenu);
    connect(ui->pushButtonOptions, &QAbstractButton::clicked, this, &FormProducts::on_pushButtonSubMenu);
    connect(ui->pushButtonGroupsModifications, &QAbstractButton::clicked, this,
            &FormProducts::on_pushButtonSubMenu);    
    connect(ui->pushButtonLabels, &QAbstractButton::clicked, this, &FormProducts::on_pushButtonSubMenu);
    connect(ui->pushButtonDiscounts, &QAbstractButton::clicked, this, &FormProducts::on_pushButtonSubMenu);    
    connect(ui->pushButtonGoodsTypes, &QAbstractButton::clicked, this, &FormProducts::on_pushButtonSubMenu);        
    connect(ui->treeViewProducts, SIGNAL(doubleClicked(QModelIndex)), SLOT(onDoubleClicked()));
    connect(ui->actionRefreshProducts, &QAction::triggered, this, &FormProducts::refreshDataProducts);
    connect(ui->actionAddProduct, SIGNAL(triggered()), SLOT(onAddEditProductClicked()));
    connect(ui->actionCloneProduct, SIGNAL(triggered()), SLOT(onAddEditProductClicked()));
    connect(ui->actionEditProduct, SIGNAL(triggered()), SLOT(onAddEditProductClicked()));
    connect(ui->actionSelect, SIGNAL(triggered()), SLOT(selectProducts()));
    connect(ui->actionDeleteProducts, &QAction::triggered, this, &FormProducts::onDeleteProductClicked);
    connect(ui->toolButtonSetColumns, &QToolButton::clicked, this, &FormProducts::setVisibleColumns);   
    connect(ui->actionSetIsDiscount, &QAction::triggered, this, &FormProducts::setSimpleProperty);
    connect(ui->actionIsYAMarket, &QAction::triggered, this, &FormProducts::setSimpleProperty);
    connect(ui->actionShowFeatures, &QAction::triggered, this, &FormProducts::setSimpleProperty);
    connect(ui->actionHideFeatures, &QAction::triggered, this, &FormProducts::setSimpleProperty);
    connect(ui->actionChangeGroup, &QAction::triggered, this, &FormProducts::setGroup);
    connect(ui->actionSetArticle, &QAction::triggered, this, &FormProducts::setArticle);
    connect(ui->actionSetModifications, &QAction::triggered, this, &FormProducts::setModifications);
    connect(ui->actionSetSpecifications, &QAction::triggered, this, &FormProducts::setSpecifications);
    connect(ui->actionSetBrand, &QAction::triggered, this, &FormProducts::setBrand);
    connect(ui->actionSetAccompsProducts, &QAction::triggered, this, &FormProducts::setAccompsProducts);
    connect(ui->actionSetSimilarProducts, &QAction::triggered, this, &FormProducts::setAccompsProducts);
    connect(ui->actionSetCrossGroups, &QAction::triggered, this, &FormProducts::setCrossGroups);
    connect(ui->actionSetCodeURL, &QAction::triggered, this, &FormProducts::setCodeURL);
    connect(ui->actionAddPrice, &QAction::triggered, this, &FormProducts::addPrice);
    connect(ui->actionSetPrice, &QAction::triggered, this, &FormProducts::setPrices);
    connect(ui->actionSetRate, &QAction::triggered, this, &FormProducts::setRates);
    connect(ui->actionCopy, &QAction::triggered, this, &FormProducts::onCopyCellText);
    connect(ui->actionAddDescription, &QAction::triggered, this, &FormProducts::addDescription);
    connect(ui->actionSetOptions, &QAction::triggered, this, &FormProducts::setOptions);
    connect(ui->checkBoxIsNew, SIGNAL(clicked(bool)), SLOT(refreshDataProducts()));
    connect(ui->checkBoxIsHit, SIGNAL(clicked(bool)), SLOT(refreshDataProducts()));
    connect(ui->checkBoxIsSpec, SIGNAL(clicked(bool)), SLOT(refreshDataProducts()));
    connect(ui->checkBoxIsActive, SIGNAL(clicked(bool)), SLOT(refreshDataProducts()));
    connect(ui->checkBoxIsNotActive, SIGNAL(clicked(bool)), SLOT(refreshDataProducts()));
    connect(ui->checkBoxIsDiscount, SIGNAL(clicked(bool)), SLOT(refreshDataProducts()));
    connect(ui->checkBoxIsNoDiscount, SIGNAL(clicked(bool)), SLOT(refreshDataProducts()));
    connect(ui->checkBoxIsYAMarket, SIGNAL(clicked(bool)), SLOT(refreshDataProducts()));
    connect(ui->comboBoxFilterCount, SIGNAL(currentIndexChanged(int)), SLOT(onChangeTypeFilterCount()));
    connect(ui->doubleSpinBoxFilterCountFrom, SIGNAL(editingFinished()), SLOT(refreshDataProducts()));
    connect(ui->doubleSpinBoxFilterCountTo, SIGNAL(editingFinished()), SLOT(refreshDataProducts()));
    connect(ui->toolButtonExport, &QAbstractButton::clicked, this, &FormProducts::exportData);
    connect(ui->toolButtonImport, &QAbstractButton::clicked, this, &FormProducts::importData);
    connect(ui->treeViewProducts->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            SLOT(onChangeSelectedProducts()));
    connect(ui->treeViewProducts->verticalScrollBar(), &QAbstractSlider::valueChanged, this, &FormProducts::onScrollProducts);
    connect(api, SIGNAL(finishedRequest(bool,QString)), SLOT(finishedRefreshData(bool,QString)));
    connect(apiInfo, SIGNAL(finishedRequest(bool,QString)), SLOT(finishedShowInfoProduct(bool,QString)));    
    connect(timerSearch, &QTimer::timeout, this, &FormProducts::refreshDataProducts);    
}

void FormProducts::initPermissions()
{
    isAddPermission = UserPermission::isAdd(UserPermission::PRODUCTS);
    isEditPermission = UserPermission::isEdit(UserPermission::PRODUCTS);
    isDeletePermission = UserPermission::isDelete(UserPermission::PRODUCTS);

    ui->actionAddPrice->setEnabled(isEditPermission);
    ui->actionChangeGroup->setEnabled(isEditPermission);
    ui->actionAddProduct->setEnabled(isAddPermission);
    ui->actionCloneProduct->setEnabled(isAddPermission);
    ui->actionDeleteProducts->setEnabled(isDeletePermission);
    ui->actionEditProduct->setEnabled(isEditPermission);
    ui->actionIsYAMarket->setEnabled(isEditPermission);
    ui->actionSetAccompsProducts->setEnabled(isEditPermission);
    ui->actionSetArticle->setEnabled(isEditPermission);
    ui->actionSetBrand->setEnabled(isEditPermission);
    ui->actionSetCodeURL->setEnabled(isEditPermission);
    ui->actionSetCrossGroups->setEnabled(isEditPermission);
    ui->actionSetHit->setEnabled(isEditPermission);
    ui->actionSetIsDiscount->setEnabled(isEditPermission);
    ui->actionSetModifications->setEnabled(isEditPermission);
    ui->actionSetNew->setEnabled(isEditPermission);
    ui->actionSetPrice->setEnabled(isEditPermission);
    ui->actionSetSimilarProducts->setEnabled(isEditPermission);
    ui->actionSetSpecifications->setEnabled(isEditPermission);
    ui->actionSetOptions->setEnabled(isEditPermission);    
}

void FormProducts::initFilterBrands()
{
    ui->comboBoxFilterBrands->disconnect();
    QString idBrand = ui->comboBoxFilterBrands->currentData().toString();
    ListDataItems brands;
    ui->comboBoxFilterBrands->clear();
    if (ApiAdapter::getApi()->getListBrands(Request(), &brands)) {
        int index = -1;
        ui->comboBoxFilterBrands->addItem("Все");
        for (int i = 0; i < brands.size(); ++i) {
            ui->comboBoxFilterBrands->addItem(brands.at(i)->getName(), brands.at(i)->getId());
            if (!idBrand.isEmpty())
                index = i;
        }
        index++;
        if (index > 0)
            ui->comboBoxFilterBrands->setCurrentIndex(index);
    }
    connect(ui->comboBoxFilterBrands, SIGNAL(currentIndexChanged(int)), SLOT(refreshDataProducts()));
}

void FormProducts::restoreStateTable()
{
    isRestoreStates = SEConfig::restoreTableState(
                ui->treeViewProducts->header(),
                ui->treeViewProducts->objectName(), isSelectMode);
}

void FormProducts::setStateTableDefault()
{    
    for (int i = 0; i < modelProducts->columnCount() - 1; i++)
        ui->treeViewProducts->resizeColumnToContents(i);
    ui->treeViewProducts->header()->resizeSection(0, 30);
    ui->treeViewProducts->header()->resizeSection(1, 30);
    ui->treeViewProducts->header()->resizeSection(2, 30);
    ui->treeViewProducts->header()->resizeSection(3, 30);
    ui->treeViewProducts->header()->resizeSection(4, 30);
    ui->treeViewProducts->header()->resizeSection(5, 30);
    isRestoreStates = true;
}

void FormProducts::saveStateTable()
{    
    SEConfig::saveTableState(ui->treeViewProducts->header(),
                             ui->treeViewProducts->objectName(), isSelectMode);
    SEConfig::saveSplitterLayout(ui->splitterMainProducts, isSelectMode);
}

void FormProducts::goToRowById(QString id)
{   
    if (!id.isEmpty()) {
            for (int i = 0; i < products->size(); i ++)
                if (products->at(i)->getId() == id) {
                    ui->treeViewProducts->selectionModel()->setCurrentIndex(
                                modelProducts->index(i, 0), QItemSelectionModel::SelectCurrent |
                                QItemSelectionModel::Rows);
                    ui->treeViewProducts->setFocus();
                    return;
                }
    }
    if (products->size())
        ui->treeViewProducts->selectionModel()->setCurrentIndex(
                    modelProducts->index(0, 0), QItemSelectionModel::SelectCurrent |
                    QItemSelectionModel::Rows);
    else onChangeProduct();
    ui->treeViewProducts->setFocus();
}

void FormProducts::showInfoProduct()
{
    modelImages->unBindItems();
    modelImages->resetModel();
    if (!ui->treeViewProducts->currentIndex().isValid())
        return;

    productInfo->setId(modelProducts->dataItem(ui->treeViewProducts->currentIndex())->getId());
    Request request;
    request.addId(productInfo->getId());
    apiInfo->requestInThread(SEConsts::API_PRODUCTS, SEConsts::APIMETHOD_INFO, request);
}

void FormProducts::initLabelsActions()
{
    if (ApiAdapter::getApi()->getListData(SEConsts::API_PRODUCTS_LABELS, mListLabels)) {
        if (!mSubMenuOnLabels)
            mSubMenuOnLabels = new QMenu("Прикрепить ярлык", this);
        mSubMenuOnLabels->clear();

        if (!mSubMenuOffLabels)
            mSubMenuOffLabels = new QMenu("Открепить ярлык", this);

        mSubMenuOnLabels->addAction(ui->actionSetNew);
        mSubMenuOnLabels->addAction(ui->actionSetHit);
        mSubMenuOffLabels->addAction(ui->actionUnsetNew);
        mSubMenuOffLabels->addAction(ui->actionUnsetHit);

        if (!ui->comboBoxFilterLabels->count())
            ui->comboBoxFilterLabels->addItem("Все");

        for (int i = 0; i < mListLabels->size(); i++) {

            QAction *action = new QAction(mListLabels->at(i)->getName(), this);
            action->setProperty("idLabel", mListLabels->at(i)->getId());
            action->setProperty("on", true);
            mSubMenuOnLabels->addAction(action);

            action = new QAction(mListLabels->at(i)->getName(), this);
            action->setProperty("idLabel", mListLabels->at(i)->getId());
            action->setProperty("on", false);
            mSubMenuOffLabels->addAction(action);
            ui->comboBoxFilterLabels->addItem(mListLabels->at(i)->getName(), mListLabels->at(i)->getId());
        }

        for (int i = 0; i < mSubMenuOnLabels->actions().size(); i++) {
            connect(mSubMenuOnLabels->actions()[i], &QAction::triggered, this, &FormProducts::setSimpleProperty);
            connect(mSubMenuOffLabels->actions()[i], &QAction::triggered, this, &FormProducts::setSimpleProperty);
        }

        connect(ui->comboBoxFilterLabels, SIGNAL(currentIndexChanged(int)), SLOT(refreshDataProducts()));
    }
}

void FormProducts::addPrice()
{
    if (!ui->treeViewProducts->currentIndex().isValid())
        return;

    QModelIndexList list = ui->treeViewProducts->selectionModel()->selectedRows();
    if (!list.size())
        return;

    DataProduct *product = new DataProduct();
    bool isAll = isAllSelected();
    for (int i = 0; i < list.size(); i++)
        if (!isAll)
            product->addId(products->at(list.at(i).row())->getId());
    if (isAll) {
        product->addId("*");
        setFilterSelectedProduct(product);
    }

    DialogAddPrice dialog(this, product);

    if (dialog.exec() == QDialog::Accepted)
        refreshDataProducts();
}

void FormProducts::addDescription()
{
    if (!ui->treeViewProducts->currentIndex().isValid())
        return;

    DataProduct *product =
            dynamic_cast<DataProduct *> (modelProducts->dataItem(ui->treeViewProducts->currentIndex()));

    QModelIndexList list = ui->treeViewProducts->selectionModel()->selectedRows();
    if (!list.size())
        return;

    bool isAll = isAllSelected();
    for (int i = 0; i < list.size(); i++)
        if (!isAll)
            product->addId(products->at(list.at(i).row())->getId());
    if (isAll) {
        product->addId("*");
        setFilterSelectedProduct(product);
    }

    DialogAddDescription dialog(this, product);
    dialog.exec();
}

void FormProducts::setPrices()
{
    if (!ui->treeViewProducts->currentIndex().isValid())
        return;

    DataProduct *product =
            dynamic_cast<DataProduct *> (modelProducts->dataItem(ui->treeViewProducts->currentIndex()));

    QModelIndexList list = ui->treeViewProducts->selectionModel()->selectedRows();
    if (!list.size())
        return;

    bool isAll = isAllSelected();
    for (int i = 0; i < list.size(); i++)
        if (!isAll)
            product->addId(products->at(list.at(i).row())->getId());
    if (isAll) {
        product->addId("*");
        setFilterSelectedProduct(product);
    }

    DialogSetPrices dialogSetPrice(this, product);
    if (dialogSetPrice.exec() == QDialog::Accepted)
        refreshDataProducts();
}

void FormProducts::setRates()
{
    if (!ui->treeViewProducts->currentIndex().isValid())
        return;

    DataProduct *product =
            dynamic_cast<DataProduct *> (modelProducts->dataItem(ui->treeViewProducts->currentIndex()));

    QModelIndexList list = ui->treeViewProducts->selectionModel()->selectedRows();
    if (!list.size())
        return;

    bool isAll = isAllSelected();
    for (int i = 0; i < list.size(); i++)
        if (!isAll)
            product->addId(products->at(list.at(i).row())->getId());
    if (isAll) {
        product->addId("*");
        setFilterSelectedProduct(product);
    }

    DialogSetRates dialogSetRates(this, product);
    dialogSetRates.exec();
}

void FormProducts::exportData()
{
    DialogExport dlgExport(this);
    dlgExport.setApiCatalogName(SEConsts::API_PRODUCTS);
    dlgExport.setFilter(requestProducts);
    dlgExport.setFileName("catalog");
    dlgExport.setFormat("xlsx");
    dlgExport.exec();
}

void FormProducts::importData()
{
    DialogProductsMasterImport dialogMaster;
    if (dialogMaster.exec() == QDialog::Accepted)
        refreshData();
}

void FormProducts::setSizeButtons()
{
    QObjectList list = ui->frameButtons->children();

    int maxWidth(0);
    for (int i = 0; i < list.size(); i++) {
        if (QPushButton *button = qobject_cast <QPushButton *> (list.at(i)))
            if (button->width() > maxWidth)
                maxWidth = button->width();
    }
    for (int i = 0; i < list.size(); i++) {
        if (QPushButton *button = qobject_cast <QPushButton *> (list.at(i)))
            button->setMinimumWidth(maxWidth);
    }
}

bool FormProducts::isAllSelected() const
{
    return navigatorProducts->isCheckedAllSelected();
}

void FormProducts::setSimpleProperty()
{
    if (!ui->treeViewProducts->currentIndex().isValid())
        return;    

    QString id = modelProducts->dataItem(ui->treeViewProducts->currentIndex())->getId();
    bool isState;
    isState = false;
    DataProduct *product = new DataProduct(
                dynamic_cast <DataProduct *> (modelProducts->dataItem(ui->treeViewProducts->currentIndex())));
    if (sender() == ui->actionSetNew) {
        product->setIsNew(true);
        product->setModified("isNew");
    }
    if (sender() == ui->actionSetHit) {        
        product->setIsHit(true);
        product->setModified("isHit");
    }
    if (sender() == ui->actionUnsetNew) {
        product->setIsNew(false);
        product->setModified("isNew");
    }
    if (sender() == ui->actionUnsetHit) {
        product->setIsHit(false);
        product->setModified("isHit");
    }
    if (sender() == ui->actionSetIsDiscount) {
        isState = dynamic_cast <DataProduct *> (
                    modelProducts->dataItem(ui->treeViewProducts->currentIndex()))->isDiscount();
        product->setIsDiscount(!isState);        
    }
    if (sender() == ui->actionIsYAMarket) {
        isState = dynamic_cast <DataProduct *> (
                    modelProducts->dataItem(ui->treeViewProducts->currentIndex()))->isYAMarket();
        product->setIsYAMarket(!isState);
        product->setModified("isYAMarket");
    }
    if (sender() == ui->actionShowFeatures) {
        product->setIsShowFeatures(true);
        product->setModified("isShowFeatures");
    }
    if (sender() == ui->actionHideFeatures) {
        product->setIsShowFeatures(false);
        product->setModified("isShowFeatures");
    }

    if (sender() == ui->actionOffLabels) {
        isState = dynamic_cast <DataProduct *>(
                    modelProducts->dataItem(ui->treeViewProducts->currentIndex()))->isHit();
        product->setIsHit(false);
        product->setIsNew(false);
        product->listLabels()->deleteItems();
        product->listLabels()->setModified();
        product->setModified("isHit");
        product->setModified("isNew");
    }

    if (!sender()->property("idLabel").isNull()) {
        DataItem *label = new DataItem;
        label->setId(sender()->property("idLabel").toString());
        label->setIsChecked(sender()->property("on").toBool());
        product->listLabels()->append(label);
    }

    bool isAll = isAllSelected();
    modelProducts->beginReset();
    QModelIndexList list = ui->treeViewProducts->selectionModel()->selectedRows();
    for (int i = 0; i < list.size(); i++) {
        if (!isAll)
            product->addId(modelProducts->dataItem(list.at(i))->getId());
        if (sender() == ui->actionSetNew)
            dynamic_cast <DataProduct *> (modelProducts->dataItem(list.at(i)))->setIsNew(true);
        if (sender() == ui->actionSetHit)
            dynamic_cast <DataProduct *> (modelProducts->dataItem(list.at(i)))->setIsHit(true);
        if (sender() == ui->actionUnsetNew)
            dynamic_cast <DataProduct *> (modelProducts->dataItem(list.at(i)))->setIsNew(false);
        if (sender() == ui->actionUnsetHit)
            dynamic_cast <DataProduct *> (modelProducts->dataItem(list.at(i)))->setIsHit(false);
        if (sender() == ui->actionSetIsDiscount)
            dynamic_cast <DataProduct *> (modelProducts->dataItem(list.at(i)))->setIsDiscount(!isState);
        if (sender() == ui->actionIsYAMarket)
            dynamic_cast <DataProduct *> (modelProducts->dataItem(list.at(i)))->setIsYAMarket(!isState);
    }
    if (isAll) {
        product->addId("*");
        setFilterSelectedProduct(product);
    }
    ApiAdapter::getApi()->saveProduct(product);
    modelProducts->endReset();
    delete product;
    goToRowById(id);
}

void FormProducts::setGroup()
{
    if (!ui->treeViewProducts->currentIndex().isValid())
        return;

    FormProductsGroups *form = new FormProductsGroups(this);

    form->setWindowFlags(Qt::Dialog);
    form->setSelectMode(false);
    if (form->exec() == QDialog::Accepted) {
        QListIterator<DataProductsGroup *> it(form->listSelected());
        if (it.hasNext()) {
            DataProduct *product = new DataProduct;
            DataProductsGroup *gr = (it.next());
            product->setIdGroup(gr->getId());
            bool isAll = isAllSelected();
            modelProducts->beginReset();
            QModelIndexList list = ui->treeViewProducts->selectionModel()->selectedRows();
            for (int i = 0; i < list.size(); i++) {
                if (!isAll)
                    product->addId(products->at(list.at(i).row())->getId());
                products->at(list.at(i).row())->setIdGroup(gr->getId());
                products->at(list.at(i).row())->setNameGroup(gr->getName());
            }
            if (isAll) {
                product->addId("*");
                setFilterSelectedProduct(product);
            }
            ApiAdapter::getApi()->saveProduct(product);
            modelProducts->endReset();
            delete product;
        }
    }
    delete form;
}

void FormProducts::setArticle()
{
    if (!ui->treeViewProducts->currentIndex().isValid())
        return;

    DataProduct *product = new DataProduct();

    bool isAll = isAllSelected();
    QModelIndexList list = ui->treeViewProducts->selectionModel()->selectedRows();
    for (int i = 0; i < list.size(); i++)
        if (!isAll)
            product->addId(products->at(list.at(i).row())->getId());
    if (isAll) {
        product->addId("*");
        setFilterSelectedProduct(product);
    }
    setCursor(Qt::WaitCursor);
    ui->actionSetArticle->setEnabled(false);

    if (ApiAdapter::getApi()->setArticleProduct(product))
        refreshDataProducts();
    setCursor(Qt::ArrowCursor);
    ui->actionSetArticle->setEnabled(true);
    delete product;

}

void FormProducts::setSpecifications()
{
    if (!ui->treeViewProducts->currentIndex().isValid())
        return;

    QModelIndexList list = ui->treeViewProducts->selectionModel()->selectedRows();
    DataProduct *current = products->at(ui->treeViewProducts->currentIndex().row());
    DataProduct *product = new DataProduct();
    if (list.size() == 1) {
        product->copy(current);
        product->listSpecifications()->clear();
        ApiAdapter::getApi()->getInfoProduct(current);        
        for (int i = 0; i < current->listSpecifications()->size(); i++)
            product->listSpecifications()->append(
                        new DataFeature(current->listSpecifications()->at(i)));
        product->setUnModified();
    }

    FrameCardSpecifications *form = new FrameCardSpecifications(
                this, product, true);
    form->fillControls();

    if (form->exec() ==  QDialog::Accepted) {
        bool isAll = isAllSelected();        
        product->setProperty("isAddSpecifications", form->isAddSpecifications() && (list.size() > 1));
        for (int i = 0; i < list.size(); i++)
            if (!isAll)
                product->addId(products->at(list.at(i).row())->getId());
        if (isAll) {
            product->addId("*");
            setFilterSelectedProduct(product);
        }
        ApiAdapter::getApi()->saveProduct(product);
    }
    delete form;
    delete product;
}

void FormProducts::setModifications()
{
    if (!ui->treeViewProducts->currentIndex().isValid())
        return;

    setCursor(Qt::WaitCursor);
    ui->actionSetModifications->setEnabled(false);
    QModelIndexList list = ui->treeViewProducts->selectionModel()->selectedRows();
    DataProduct *selectedProducts = new DataProduct;
    for (int i = 0; i < list.size(); i++)
         selectedProducts->addId(products->at(list.at(i).row())->getId());
    ApiAdapter::getApi()->getInfoProduct(selectedProducts);

    DataProduct *product = new DataProduct();
    for (int i = 0; i < selectedProducts->listModificationsGroups()->size(); i++)
        product->listModificationsGroups()->append(
                    new DataModificationGroup(selectedProducts->listModificationsGroups()->at(i)));
    product->setUnModified();
    delete selectedProducts;

    FrameCardModifications *form = new FrameCardModifications(
                this, product->listModificationsGroups(), 0, true);
    form->fillControls();

    if (form->exec() ==  QDialog::Accepted) {
        bool isAll = isAllSelected();
        for (int i = 0; i < list.size(); i++)
            if (!isAll)
                product->addId(products->at(list.at(i).row())->getId());
        if (isAll) {
            product->addId("*");
            setFilterSelectedProduct(product);
        }
        ApiAdapter::getApi()->saveProduct(product);
        refreshDataProducts();
    }
    delete form;
    delete product;    

    ui->actionSetModifications->setEnabled(true);
    setCursor(Qt::ArrowCursor);

}

void FormProducts::setOptions()
{
    if (!ui->treeViewProducts->currentIndex().isValid())
        return;

    QModelIndexList list = ui->treeViewProducts->selectionModel()->selectedRows();
    DataProduct *current = products->at(ui->treeViewProducts->currentIndex().row());
    DataProduct *product = new DataProduct();
    if (list.size() == 1) {
        product->copy(current);
        product->listOptions()->clear();
        ApiAdapter::getApi()->getInfoProduct(current);
        for (int i = 0; i < current->listOptions()->size(); i++)
            product->listOptions()->append(
                        new DataOption(current->listOptions()->at(i)));
        product->setUnModified();
    }

    FrameCardOptions *form = new FrameCardOptions(this, product, true);
    form->fillControls();

    if (form->exec() ==  QDialog::Accepted) {
        bool isAll = isAllSelected();
        product->setProperty("isAddOptions", form->isAddOptions() && (list.size() > 1));
        for (int i = 0; i < list.size(); i++)
            if (!isAll)
                product->addId(products->at(list.at(i).row())->getId());
        if (isAll) {
            product->addId("*");
            setFilterSelectedProduct(product);
        }
        ApiAdapter::getApi()->saveProduct(product);
    }
    delete form;
    delete product;
}

void FormProducts::setBrand()
{
    if (!ui->treeViewProducts->currentIndex().isValid())
        return;

    DataProduct *product = new DataProduct();
    FormBrands form(this);
    form.setSelectMode(false);
    if (form.exec() == QDialog::Accepted) {
        bool isAll = isAllSelected();
        QModelIndexList list = ui->treeViewProducts->selectionModel()->selectedRows();
        for (int i = 0; i < list.size(); i++)
            if (!isAll)
                product->addId(products->at(list.at(i).row())->getId());
        if (isAll) {
            product->addId("*");
            setFilterSelectedProduct(product);
        }
        if (form.selectedBrands()->size()) {
            product->brand()->setId(form.selectedBrands()->at(0)->getId());
            product->brand()->setName(form.selectedBrands()->at(0)->getName());
        }
        if (ApiAdapter::getApi()->saveProduct(product))
            refreshDataProducts();
    }
    delete product;
}

void FormProducts::setAccompsProducts()
{
    if (!ui->treeViewProducts->currentIndex().isValid())
        return;

    DataProduct *current = products->at(ui->treeViewProducts->currentIndex().row());
    DataProduct *product = new DataProduct();
    ListProducts *listCurrent = current->listAccompProducts();
    ListProducts *listProduct = product->listAccompProducts();
    if (sender() == ui->actionSetSimilarProducts) {
        listCurrent = current->listSimilarProducts();
        listProduct = product->listSimilarProducts();
    }
    preloader->startAnimation();
    ApiAdapter::getApi()->getInfoProduct(current);
    for (int i = 0; i < listCurrent->size(); i++)
        listProduct->append(
                    new DataProduct(listCurrent->at(i)));
    product->setUnModified();

    FrameCardSimilar *form = new FrameCardSimilar(this, listProduct, true);
    form->setWindowTitle(tr("Сопутствующие товары"));
    if (sender() == ui->actionSetSimilarProducts)
        form->setWindowTitle(tr("Похожие товары"));
    form->fillControls();
    preloader->stopAnimation();

    if (form->exec() ==  QDialog::Accepted) {
        bool isAll = isAllSelected();
        QModelIndexList list = ui->treeViewProducts->selectionModel()->selectedRows();
        for (int i = 0; i < list.size(); i++)
            if (!isAll)
                product->addId(products->at(list.at(i).row())->getId());
        if (isAll) {
            product->addId("*");
            setFilterSelectedProduct(product);
        }
        ApiAdapter::getApi()->saveProduct(product);
    }
    delete form;
    delete product;
}

void FormProducts::setCrossGroups()
{
    if (!ui->treeViewProducts->currentIndex().isValid())
        return;

    DataProduct *current = products->at(ui->treeViewProducts->currentIndex().row());
    DataProduct *product = new DataProduct();
    ApiAdapter::getApi()->getInfoProduct(current);
    for (int i = 0; i < current->listCrossGroups()->size(); i++)
        product->listCrossGroups()->append(
                    new DataItem(current->listCrossGroups()->at(i)));
    product->setUnModified();

    FrameCardCrossGroups *form = new FrameCardCrossGroups(
                this, product->listCrossGroups(), true);
    form->fillControls();

    if (form->exec() ==  QDialog::Accepted) {
        bool isAll = isAllSelected();
        QModelIndexList list = ui->treeViewProducts->selectionModel()->selectedRows();
        for (int i = 0; i < list.size(); i++)
            if (!isAll)
                product->addId(products->at(list.at(i).row())->getId());
        if (isAll) {
            product->addId("*");
            product->setProperty("filter", requestProducts.filter());
            product->setProperty("search", requestProducts.search());
        }

        ApiAdapter::getApi()->saveProduct(product);
    }
    delete form;
    delete product;
}

void FormProducts::setCodeURL()
{    
    DataProduct *product = new DataProduct();

    bool isAll = isAllSelected();
    QModelIndexList list = ui->treeViewProducts->selectionModel()->selectedRows();
    for (int i = 0; i < list.size(); i++)
        if (!isAll)
            product->addId(products->at(list.at(i).row())->getId());
    if (isAll) {
        product->addId("*");
        setFilterSelectedProduct(product);
    }
    if (ApiAdapter::getApi()->setCodeUrlProduct(product))
        refreshDataProducts();
    delete product;
}

void FormProducts::onAddEditProductClicked()
{
    FormProductCard cardProduct(this);

    bool isNew = (sender() == ui->actionAddProduct) ||
            (sender() == ui->actionCloneProduct);    

    if (isNew && !isAddPermission)
        return;
    if (!isNew && !isEditPermission)
        return;

    if ((!isNew || (sender() == ui->actionCloneProduct)) &&
            ui->treeViewProducts->currentIndex().isValid()) {
        QModelIndex index = ui->treeViewProducts->currentIndex();
        QModelIndexList list = ui->treeViewProducts->selectionModel()->selectedRows();
        if (list.size() == 1) {
            cardProduct.setId(modelProducts->idData(index));
            cardProduct.setWindowTitle(tr("Редактирование") + " [" +
                                        modelProducts->dataItem(index)->getName() + "]");
        }
        else {
            QStringList ids;
            bool isAll = isAllSelected();                        
            if (isAll) {
                ids.append(products->at(list.at(0).row())->getId());
                ids.append("*");
                cardProduct.setFilterForSave(requestProducts.filter());
            }
            else {
                for (int i = 0; i < list.size(); i++)
                     ids.append(products->at(list.at(i).row())->getId());
            }
            cardProduct.setIds(ids);
            cardProduct.setWindowTitle(tr("Мультиредактирование"));            
        }
        if (sender() == ui->actionCloneProduct) {
            cardProduct.setWindowTitle(tr("Клонирование товара") + " [" +
                                        modelProducts->dataItem(index)->getName() + "]");
            cardProduct.setCloneMode();
        }
    } else {
        if (const DataProductsGroup *group = ui->widgetGroups->checkedGroup())
            cardProduct.setGroup(group);
        cardProduct.setWindowTitle(tr("Создание товара"));
    }
    connect(&cardProduct, SIGNAL(saved(const DataProduct*,bool)),
                SLOT(onSaveProduct(const DataProduct*,bool)));
    setCursor(Qt::WaitCursor);
    cardProduct.exec();
    setCursor(Qt::ArrowCursor);
    showInfoProduct();
}

void FormProducts::onDeleteProductClicked()
{
    if (!ui->treeViewProducts->selectionModel()->selectedRows().count() || !isDeletePermission)
        return;

    QString msg;
    if (ui->treeViewProducts->selectionModel()->selectedRows().count() > 1)
        msg = tr("Удалить выбранные товары?");
    else msg = tr("Удалить товар") + ": " +
            products->at(ui->treeViewProducts->currentIndex().row())->getName() + "?";

    QMessageBox::StandardButton mesRepl;

    mesRepl = QMessageBox::question(this, tr("Подтверждение удаления"),
                                    msg, QMessageBox::Yes | QMessageBox::No);
    if(mesRepl == QMessageBox::Yes)
    {
        Request request;
        bool isAll = isAllSelected();
        if (isAll) {
            request.setFilter(request.filter());
            request.addId("*");
        } else {
            QModelIndexList list = ui->treeViewProducts->selectionModel()->selectedRows();

            for (int i = 0; i < list.count(); i++)
                request.addId(products->at(list.at(i).row())->getId());
        }
        ApiAdapter::getApi()->deleteProducts(request);
        refreshDataProducts();
    }
}


void FormProducts::onChangeSelectedProducts()
{
    navigatorProducts->setSelectedCount(ui->treeViewProducts->selectionModel()->selectedRows().size());
    ui->actionCloneProduct->setEnabled(ui->treeViewProducts->selectionModel()->selectedRows().size() == 1);
    if (ui->treeViewProducts->selectionModel()->selectedRows().size() == 1) {
        ui->actionEditProduct->setText(tr("Редактирование товара"));
        navigatorProducts->setUnchekedSelectButton();
    }
    if (ui->treeViewProducts->selectionModel()->selectedRows().size() > 1)
        ui->actionEditProduct->setText(tr("Мультиредактирование товаров"));
}

void FormProducts::onChangeTypeFilterCount()
{
    ui->widgetFilterCount->setVisible(ui->comboBoxFilterCount->currentIndex() > 1);
    refreshDataProducts();
}

void FormProducts::onChangeProductGroup()
{
    numPage = 0;
    refreshDataProducts();
}

void FormProducts::onCopyCellText()
{
    if (!ui->treeViewProducts->currentIndex().isValid())
        return;

    QApplication::clipboard()->setText(
                modelProducts->data(ui->treeViewProducts->currentIndex(), Qt::DisplayRole).toString());
}

void FormProducts::onAddFromYMUrl()
{
    if (!dialogParseFromYM) {
        dialogParseFromYM = new DialogAddFromYMUrl;
        connect(dialogParseFromYM, SIGNAL(saved(const DataProduct*,bool)),
                SLOT(onSaveProduct(const DataProduct*,bool)));
    }
    dialogParseFromYM->showMaximized();
}

void FormProducts::selectProducts()
{
    bool isSelected = false;
    QModelIndexList list = ui->treeViewProducts->selectionModel()->selectedRows();
    for (int l = 0; l < list.size(); ++l) {
        DataProduct *product = dynamic_cast <DataProduct *> (modelProducts->dataItem(list.at(l)));

        if (!isSelectModification || !product->getCountModifications()) {
                selectedProducts->append(new DataProduct(products->at(list.at(l).row())));
                isSelected = true;
        }
        else {
            DialogProductModifications *dlgModif = new DialogProductModifications(this, product);
            if (dlgModif->exec() == QDialog::Accepted) {
                isSelected = true;
                for (int i = 0; i < dlgModif->selectedModifications()->size(); i++) {
                    DataProduct *offer = new DataProduct(dlgModif->selectedModifications()->at(i));
                    selectedProducts->append(offer);
                }
            }
            for (int i = 0; i < selectedProducts->size(); ++i)
                selectedProducts->at(i)->setId(product->getId());
            delete dlgModif;
        }
    }
    isAutoSelectModification = false;
    if (isSelected)
        accept();
    else refreshDataProducts();
}

void FormProducts::setAutoSelectModifications(bool isAuto)
{
    isAutoSelectModification = isAuto;
}

void FormProducts::finishedRefreshData(const bool status, const QString message)
{
    if (status &&
            ApiAdapter::getApi()->getListProducts(requestProducts, products, true)) {
        countProducts = modelProducts->getItems()->countInBase();
        modelProducts->resetModel();
        isShown = true;
    }
    preloader->stopAnimation();
    if (!isRestoreStates)
        setStateTableDefault();
    SEConfig::restoreStateModel(ui->treeViewProducts);
    connect(timerShowImage, &QTimer::timeout, this, &FormProducts::showInfoProduct);
    if (!ui->treeViewProducts->currentIndex().isValid() && modelProducts->getItems()->size())
        ui->treeViewProducts->selectionModel()->setCurrentIndex(
                    modelProducts->index(0, 0), QItemSelectionModel::SelectCurrent | QItemSelectionModel::Rows);    
    if (!status && !message.isEmpty())
        QMessageBox::critical(0, tr("Ой, ошибка запроса!"), message);
    else {
        if (isAutoSelectModification)
            selectProducts();
    }
}

void FormProducts::finishedShowInfoProduct(const bool status, const QString )
{
    if (status && ApiAdapter::getApi()->getInfoProduct(productInfo, true)) {
        modelImages->setListData(productInfo->listImages());
        modelImages->resetModel();
    }
}
