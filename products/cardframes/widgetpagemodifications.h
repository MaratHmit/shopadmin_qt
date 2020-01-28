#ifndef WIDGETPAGEMODIFICATIONS_H
#define WIDGETPAGEMODIFICATIONS_H

#include <QWidget>

#include "datamodificationgroup.h"
#include "modificationsmodel.h"
#include "formselectmodifications.h"
#include "modificationsimagesmodel.h"
#include "framelistimages.h"

namespace Ui {
class WidgetPageModifications;
}

class WidgetPageModifications : public QWidget
{
    Q_OBJECT

public:
    WidgetPageModifications(
            QWidget *parent, DataModificationGroup *group,
            ListDataItems *imagesProduct, const bool &isSelectMode,
            const bool &isMultiSelect);
    ~WidgetPageModifications();
    DataModificationGroup *groupInfo() const;
    ListModifications *selectedModifications() const;
    void setIsAutoFilledModifications();

signals:
    void modified();
    void changeItem(const QModelIndex &index);
    void doubleClicked();

private slots:
    void showContextMenu(QPoint point);
    void onSelectModifications();
    void onDelete();
    void onChange();
    void onModifiedValue(const QModelIndex &index);
    void onModified();

private:
    void showEvent(QShowEvent *e);    
    void timerEvent(QTimerEvent *e);

    Ui::WidgetPageModifications *ui;
    ModificationsModel *mModel;
    ModificationsImagesModel *mModelImages;
    FormSelectModifications *mFormSelect;
    FrameListImages *mFormImages;
    ListDataItems *mImagesProduct;

    bool mImagesMode;
    bool mIsSelectMode;
    bool mIsMultiSelect;
    bool mIsAutoFilledModifications;

    DataModificationGroup *mGroup;
    ListModifications *mSelectedModifications;

    void initialization();
    void initSignals();
    void initActions();
    void initColumns();
    bool existModification(DataModification *modification);
    void addImage();
    void deleteImage();
    void sortImage();
    void onChangeImage();
    void filledModifications();
};

#endif // WIDGETPAGEMODIFICATIONS_H
