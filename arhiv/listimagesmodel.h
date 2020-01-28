#ifndef LISTIMAGESMODEL_H
#define LISTIMAGESMODEL_H

#include "nametablemodel.h"

class ListImagesModel : public NameTableModel
{
public:
    ListImagesModel(QObject *parent, ListDataItems *listImages,
                    const bool &isSelectMode = false);
    ~ListImagesModel();

    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;    
    Qt::ItemFlags flags(const QModelIndex &index) const;

private:
    void downloadImages() const;
    bool mIsSelectMode;    

    QStringList *mListLoadImages;

    enum HEADER
    {
        HEADER_IMAGE = 0,
        HEADER_ISMAIN = 1,
        HEADER_WIDGET = 2
    };



};

#endif // LISTIMAGESMODEL_H
