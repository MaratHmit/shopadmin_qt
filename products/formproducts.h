#ifndef PRODUCTS_H
#define PRODUCTS_H

#include <QTreeWidgetItem>
#include <QWidget>

#include "dataproduct.h"
#include "framepagenavigator.h"
#include "preloader.h"
#include "formproductcard.h"
#include "formbrands.h"
#include "formfeatures.h"
#include "formoptions.h"
#include "formmodificationsgroups.h"
#include "formcoupons.h"
#include "formdiscounts.h"
#include "formcatalog.h"
#include "request.h"
#include "formsetvisibleheadercolumns.h"
#include "dialogcardproducttype.h"
#include "dialogcardproductlabel.h"
#include "dialogaddfromymurl.h"
#include "field.h"
#include "api.h"
#include "dialogadddescription.h"

namespace Ui {
class FormProducts;
}

class FormProducts : public QDialog
{
    Q_OBJECT

public:
    explicit FormProducts(QWidget *parent = 0);
    FormProducts(QWidget *parent, const bool &isMultiSelect, const bool &isSelectModification = false);
    ~FormProducts();

    void refreshData();
    void setSystemSearchText(const QString &searchText);
    void setAutoSelectModifications(bool isAuto);

    const ListProducts *getSelectedProducts() const;

public slots:
    void onChangeProduct();        

    void goNextPage();
    void goPreviosPage();
    void goLastPage();
    void goFirstPage();
    void selectProducts();

protected slots:
    void reject();

private slots:
    void refreshDataProducts();    
    void runSearchProducts();
    void onClickHeaderItem(int index);    
    void onDoubleClicked();
    void onAddEditProductClicked();
    void onDeleteProductClicked();        
    void onChangeSelectedProducts();
    void onChangeTypeFilterCount();
    void onChangeProductGroup();    
    void onCopyCellText();
    void onAddFromYMUrl();
    void finishedRefreshData(const bool status, const QString message);
    void finishedShowInfoProduct(const bool status, const QString message);

    void on_pushButtonGroups_clicked();
    void on_pushButtonProducts_clicked();        
    void on_pushButtonSubMenu();

    void showContextMenuProducts(QPoint point);
    void onSaveProduct(const DataProduct *product, const bool isNew);

    void onScrollProducts(int value);
    void selectAllItems(bool isSelected);    

private:
    void showEvent(QShowEvent *);
    void hideEvent(QHideEvent *);   
    void keyPressEvent(QKeyEvent *);

    Ui::FormProducts *ui;
    FramePageNavigator *navigatorProducts;
    NameTableModel *modelProducts;    
    Preloader *preloader;
    FormBrands *formBrands;
    FormFeatures *formFeatures;
    FormOptions *formOptions;
    FormModificationsGroups *formModificationsGroups;
    FormCoupons *formCoupons;
    FormDiscounts *formDiscounts;
    FormCatalog *formProductsTypes;
    FormCatalog *formLabels;
    ListProducts *products;
    ListProducts *selectedProducts;    
    ListDataItems *mListLabels;
    DataProduct *productInfo;
    QVector<Field *> *fieldsProducts;    
    Request requestProducts;    
    FormSetVisibleHeaderColumns *formSetColumns;
    ModificationsImagesModel *modelImages;
    DialogAddFromYMUrl *dialogParseFromYM;  

    bool isGroupMode;
    bool isShown;
    bool isRestoreStates;
    bool isSelectMode;
    bool isSelectModification;
    bool isAutoSelectModification;    
    int countProducts;
    int numPage;
    int scrollEventIndex;
    QTimer *timerShowImage;
    QTimer *timerSearch;
    Api *api;
    Api *apiInfo;
    QString mSystemSearchText;
    QMenu *mSubMenuOnLabels;
    QMenu *mSubMenuOffLabels;

    bool isAddPermission;
    bool isEditPermission;
    bool isDeletePermission;

    void setFilterSelectedProduct(DataProduct *product);
    void resetRequestProducts();    
    void initFieldsProducts();    
    void initActions();
    void initTreeProducts();
    void initNavigator();
    void initVariables();
    void initSignals();
    void initPermissions();
    void initFilterBrands();
    void restoreStateTable();
    void setStateTableDefault();
    void saveStateTable();
    void goToRowById(QString id);
    void showInfoProduct();
    void initLabelsActions();

    void addPrice();
    void addDescription();
    void setPrices();
    void setRates();
    void exportData();
    void importData();
    void setSizeButtons();
    bool isAllSelected() const;
    void setSimpleProperty();
    void setGroup();
    void setArticle();
    void setSpecifications();
    void setModifications();
    void setBrand();
    void setAccompsProducts();
    void setCrossGroups();
    void setCodeURL();
    void setOptions();

    void setGroupsMode(const bool &isGroupMode);
    void setVisibleColumns();

};

#endif // PRODUCTS_H
