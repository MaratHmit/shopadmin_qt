#ifndef FRAMECARDMODIFICATIONS_H
#define FRAMECARDMODIFICATIONS_H

#include <QDialog>

#include "datamodificationgroup.h"
#include "widgetpagemodifications.h"

namespace Ui {
class FrameCardModifications;
}

class FrameCardModifications : public QDialog
{
    Q_OBJECT

public:
    explicit FrameCardModifications(QWidget *parent, ListModificationsGroups *modificationsGroups,
                                    ListDataItems *imagesProduct, const bool &isDialogMode = false);
    FrameCardModifications(QWidget *parent, ListModificationsGroups *modificationsGroups,
                           const bool &isMultiSelect);
    ~FrameCardModifications();

    void setIdModificationGroupDef(const QString &idModificationGroupDef);
    ListModifications *selectedModifications() const;   

    void fillControls();    

signals:
    void modified();

private slots:
    void onAddGroups();
    void onRemoveGroup(const int &index);
    void onModified();
    void onChangeItemModification(const QModelIndex &index);

private:
    Ui::FrameCardModifications *ui;    
    bool mIsFilled;
    bool mIsDialogMode;
    bool mIsSelectMode;
    bool mIsMultiSelect;
    QString mIdModificationGroupDef;
    QList<WidgetPageModifications *> *mListPages;
    ListDataItems *mImagesProduct;

    ListModificationsGroups *mModificationsGroups;
    ListModifications *mSelectedModifications;

    void addPage(DataModificationGroup *group, const bool &isNew = false);
    WidgetPageModifications *getPageById(const QString &id) const;
    void initSignals();
    void initGroups();
    void initVariables();    
};

#endif // FRAMECARDMODIFICATIONS_H
