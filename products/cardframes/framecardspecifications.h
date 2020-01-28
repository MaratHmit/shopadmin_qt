#ifndef FRAMECARDSPECIFICATIONS_H
#define FRAMECARDSPECIFICATIONS_H

#include <QDialog>

#include "dataproduct.h"
#include "groupspecifications.h"
#include "formfeatures.h"

namespace Ui {
class FrameCardSpecifications;
}

class FrameCardSpecifications : public QDialog
{
    Q_OBJECT

public:
    explicit FrameCardSpecifications(
            QWidget *parent, DataProduct *product,
            const bool &isDialogMode = false);
    ~FrameCardSpecifications();

    bool isAddSpecifications() const;
    void fillControls();    
    void correctDataBeforeSave();

signals:
    void modified();

public slots:
    void deleteGroupSpecification(GroupSpecifications *);

private slots:
    void onAddSpecificationClicked();
    void onDelSpecificationClicked();
    void showContextMenu(QPoint point);
    void onCopyFromOtherProduct();

    void selectTypeProduct();
    void onModified();

private:
    Ui::FrameCardSpecifications *ui;
    ListFeatures *mListSpecifications;        
    FormFeatures *mFormFeatures;
    ListDataItems *mProductTypes;
    bool mIsFilled;
    bool mIsDialogMode;
    QSpacerItem *mVerticalSpacer;   
    DataProduct *mProduct;

    bool existSpecification(DataFeature *feature) const;
    void initActions();
    void initSignals();
    void initProductTypes();
    void onSave();    

};

#endif // FRAMECARDSPECIFICATIONS_H
