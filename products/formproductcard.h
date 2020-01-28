#ifndef FORMPRODUCTCARD_H
#define FORMPRODUCTCARD_H

#include <QDialog>

#include "dataproduct.h"
#include "dataproductsgroup.h"
#include "framecardmaininfo.h"
#include "framecardmoreinfo.h"
#include "framelistimages.h"
#include "framecardspecifications.h"
#include "framecardmodifications.h"
#include "framecardsimilar.h"
#include "framecarddiscounts.h"
#include "formcatalog.h"
#include "framecardcrossgroups.h"
#include "frameseo.h"
#include "framecardfiles.h"
#include "framecardoptions.h"

namespace Ui {
class FormProductCard;
}

class FormProductCard : public QDialog
{
    Q_OBJECT

public:
    explicit FormProductCard(QWidget *parent = 0);
    ~FormProductCard();

    void setId(const QString &id);
    void setIds(const QStringList &ids);
    void setGroup(const DataProductsGroup *group);
    void setFilterForSave(const QString filter);
    void setCloneMode();    
    const DataProduct *product() const;

signals:
    void saved(const DataProduct *product, const bool isNew);

private slots:    
    void onMainInfoClicked();
    void onMoreInfoClicked();
    void onListDataImagesClicked();
    void onSpecificationsClicked();
    void onModificationsClicked();
    void onSimilarClicked();    
    void onAccompanyingClicked();
    void onDiscountsClicked();
    void onCommentsClicked();
    void onCrossGroupsClicked();
    void onSEOClicked();
    void onFilesClicked();
    void onOptionsClicked();
    void onButtonMenuClicked();    
    void onModified();    

    void onSave();
    void onOK();

private:
    Ui::FormProductCard *ui;    
    FrameCardMainInfo *mFrameMainInfo;
    FrameCardMoreInfo *mFrameMoreInfo;
    FrameListImages *mFrameListImages;
    FrameCardSpecifications *mFrameSpecifications;
    FrameCardModifications *mFrameModifications;
    FrameCardSimilar *mFrameSimilar;
    FrameCardSimilar *mFrameAccompanying;
    FrameCardDiscounts *mFrameDiscounts;
    FrameCardFiles *mFrameFiles;
    FormCatalog *mFrameComments;
    FrameCardCrossGroups *mFrameCrossGroups;
    FrameSEO *mFrameSEO;    
    FrameCardOptions *mFrameOptions;

    DataProduct *mProduct;
    DataProduct *mProductSaved;
    bool mIsCloneMode;
    bool mIsMultiMode;

    void showEvent(QShowEvent *e);

    void initSignals();    
    void getData();        
    bool save();
};

#endif // FORMPRODUCTCARD_H
