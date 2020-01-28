#ifndef FORMPRODUCTGROUPCARD_H
#define FORMPRODUCTGROUPCARD_H

#include <QDialog>

#include "dataproductsgroup.h"
#include "nametablemodel.h"
#include "framewysiwyg.h"
#include "framelistimages.h"

namespace Ui {
class FormProductGroupCard;
}

class FormProductGroupCard : public QDialog
{
    Q_OBJECT

public:
    explicit FormProductGroupCard(QWidget *parent = 0);
    ~FormProductGroupCard();

    void setId(const QString &);
    void setIds(const QStringList &);
    void setIdParentGroup(const QString &idParent,
                          const QString &titleParent);

    DataProductsGroup *group() const;

private slots:
    void on_buttonSave_clicked();
    void on_pushButtonMainInfo_clicked();
    void on_pushButtonDescription_clicked();
    void on_pushButtonPicture_clicked();
    void on_pushButtonDiscounts_clicked();
    void on_pushButtonSEO_clicked();
    void on_pushButtonLinksGroups_clicked();
    void on_pushButtonFilters_clicked();    
    void on_toolButtonAddGroupd_clicked();
    void on_toolButtonDelGroup_clicked();
    void on_pushButtonAdditionalSubgroups_clicked();

    void on_toolButtonParentGroup_clicked();
    void on_toolButtonClearParentGroup_clicked();
    void on_lineEditName_textChanged(const QString &arg1);

    void on_toolButtonAddFilter_clicked();
    void on_toolButtonDelFilter_clicked();

    void onSort();
    void onShowListPictures();

    void on_pushButtonSimilarGroups_clicked();
    void on_toolButtonAddSimilar_clicked();
    void on_toolButtonDelSimilar_clicked();    

    void on_toolButtonAddSubgroup_clicked();
    void on_toolButtonDelSubgroup_clicked();

    void changeSourcePrices(int index);

private:
    Ui::FormProductGroupCard *ui;

    DataProductsGroup *mGroup;   
    NameTableModel *mLinksModel;
    NameTableModel *mSimilarsModel;
    NameTableModel *mSubgroupsModel;
    NameTableModel *mDiscountsModel;
    NameTableModel *mFiltersModel;    
    bool mIsMultiMode;
    FrameWYSIWYG *mDescriptionEditor;
    FrameListImages *mFrameListImages;

    bool save();    

    void initLineEditParentGroup();
    void initDiscounstModel();
    void initLinksModel();
    void initSimilarsModel();
    void initSubgroupsModel();
    void initFiltersModel();
    void initSignals();
    void initGoodsTypes();
    void addDiscount();
    void deleteDiscounts();

    void onChangeFilterItem();
    void onCheckedFilter();

    void showEvent(QShowEvent *e);    

    void fillControls();
    void setData();
    void checkEnabled();
};

#endif // FORMPRODUCTGROUPCARD_H
