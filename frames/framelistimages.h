#ifndef FRAMEListDataItems_H
#define FRAMEListDataItems_H

#include <QLabel>
#include <QRadioButton>
#include <QDialog>

#include "frameimagedescription.h"
#include "nametablemodel.h"
#include "listimagesdelegate.h"
#include "field.h"

namespace Ui {
class FrameListImages;
}

class FrameListImages : public QDialog
{
    Q_OBJECT

public:
    explicit FrameListImages(QWidget *parent, const int &section,
                             ListDataItems *listImages,
                             const bool &isSelectMode = false);
    ~FrameListImages();

    void fillControls();
    void clearInfo();
    void hideCheckedsColumn();
    void setDefaultAlt(const QString &defaultAlt);
    const ListDataItems *selectedImages();

signals:
    void modified();

private slots:    
    void onButtonOpenImageClicked();
    void onButtonSetImageFromURL();
    void onButtonLoadImageFromLibClicked();
    void onButtonLoadImageFromYF();
    void onButtonDeleteImageClicked();
    void onChange();

private:
    Ui::FrameListImages *ui;
    int sectionImage;
    bool mIsSelectMode;    
    ListDataItems *mListImages;
    ListDataItems *mSelectedImages;
    QLabel *mPreview;    
    NameTableModel *mModel;
    ListImagesDelegate *mDelegate;
    QString mDefaultAlt;

    void showEvent(QShowEvent *e);    

    void initSignals();
    QString getCurrentFolder();
    QString getCurrentUrl();
    void setSizeColumnsTable();    
    void showPreview(const QString &fileName, const bool &isFullName = false);
    void onCellClicked(QModelIndex index);
    void onModified();
    void onCheckedImage();
    void onSort();
    bool createImageFile(DataItem *item);
    void uploadImage(const QString &fileName);    
    void appendImageFromLib(const int section);

};

#endif // FRAMEListDataItems_H
