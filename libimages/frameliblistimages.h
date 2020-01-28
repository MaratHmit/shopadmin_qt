#ifndef FRAMELIBListDataItems_H
#define FRAMELIBListDataItems_H

#include <QWidget>
#include <QLabel>
#include <QTimer>

#include "requestimages.h"
#include "dataitem.h"
#include "preloader.h"
#include "nametablemodel.h"

namespace Ui {
class FrameLibListDataItems;
}

class FrameLibListDataItems : public QWidget
{
    Q_OBJECT

public:    
    explicit FrameLibListDataItems(QWidget *parent, const int &sectionImage);
    ~FrameLibListDataItems();

    void showImages();
    void resetImages();
    void refreshData();    
    void addFiles(const QStringList listFiles);    
    void deleteFiles();
    void setSelectMode(const bool &isMultiSelect);
    void deleteUnusedFiles();

    bool isCurrentRow();

    QString selectionFile();
    QStringList selectionFiles();
    const ListDataItems *selectionItems();

    static bool uploadYandexImage(QWidget *widget, DataItem *item);

    ListDataItems *getItems() const;

signals:   
    void selected();
    void doubleClickedItem();

private slots:    
    void currentItemChaned();
    void showPreview();
    void cellClicked(const QModelIndex &index);
    void cellDoubleClicked(const QModelIndex &index);
    void onUploadFinished(const QJsonObject object);

    void on_pushButtonBreakLoad_clicked();

    void hideProgressBar();

    void goNextPage();
    void goPreviosPage();
    void goLastPage();
    void goFirstPage();

    void runSearch();

private:
    Ui::FrameLibListDataItems *ui;
    RequestImages request;
    ListDataItems *items;
    ListDataItems *selectedItems;
    NameTableModel *modelItems;
    bool isShown;
    int sectionImage;
    int numPage;
    int countItems;
    QLabel *preview;
    bool isSelectMode;
    Preloader *preloader;
    QTimer *timerSearch;
    bool isRestoreStates;

    QString getCurrentFolder();
    void resetRequest();

    void deleteSelectedFiles();

    void uploadImages(QStringList list);    
    void search(const QString &text);

    const QStringList getValidNames(const QStringList &names);

    void initSignals();
    void initVariables();
    void initControls();

    void showProgressBar(const qint64 &max);

    void restoreStateTree();
    void setStateTree();
    void setStateTreeDefault();
    void saveStateTree();

};

#endif // FRAMELIBListDataItems_H
