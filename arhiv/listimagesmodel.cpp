#include "listimagesmodel.h"
#include "seconsts.h"
#include "seconfig.h"
#include "threadloadimage.h"

#include <QThread>
#include <QPixmap>
#include <QFile>
#include <QDir>
#include <QCheckBox>

ListImagesModel::ListImagesModel(
        QObject *parent, ListDataItems *listImages,
        const bool &isSelectMode):
    NameTableModel(parent, listImages)
{
    mIsSelectMode = isSelectMode;
    mListLoadImages = new QStringList;
}

ListImagesModel::~ListImagesModel()
{
    delete mListLoadImages;
}

int ListImagesModel::columnCount(const QModelIndex &) const
{
    return 3;
}

QVariant ListImagesModel::data(const QModelIndex &index, int role) const
{
    QVariant result;

    if (!listData() || !index.isValid())
        return result;

    if (role == Qt::TextAlignmentRole) {
        if (index.column() == 1)
            return int(Qt::AlignHCenter | Qt::AlignVCenter);
    }

    DataItem *image = listData()->at(index.row());    

    if (mIsCheckable && Qt::CheckStateRole == role && index.row() >= 0)
        return listData()->at(index.row())->isChecked();

    if (role == Qt::SizeHintRole  && index.column() == HEADER_IMAGE)
        result = QSize(64, 64);
    if (role == Qt::DecorationRole  && index.column() == HEADER_IMAGE &&
            !image->imageFile().isEmpty()) {
        QString fileName = SEConfig::folderImages(SEConsts::Section_Products) +
                QDir::separator() + image->imageFile();
        QFile f(fileName);
        if (f.exists()) {
            QPixmap ico(fileName);
            result = ico.scaled(QSize(64, 64));
        } else
        {           
            if (!mListLoadImages->contains(image->imageFile()))
                mListLoadImages->append(image->imageFile());
        }
    }

    if (index.row() == listData()->size() - 1 && !mListLoadImages->empty())
        downloadImages();

    return result;
}

QVariant ListImagesModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    QVariant result;

    if (role == Qt::TextAlignmentRole)
        return (int) Qt::AlignLeft | Qt::AlignVCenter;

    if (orientation == Qt::Vertical && role == Qt::DisplayRole) {
        return section + 1;
    }

    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        switch (section) {
        case HEADER_IMAGE:
            return tr("Изображ.");
        case HEADER_ISMAIN:
            return tr("Главн.");
        case HEADER_WIDGET:
            return tr("Наименование/описание");
        }

    return result;
}

Qt::ItemFlags ListImagesModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flags = NameTableModel::flags(index);

    if (index.isValid() && index.column() > 1 && !mIsSelectMode)
        flags |=  Qt::ItemIsEditable;
    return flags;
}

void ListImagesModel::downloadImages() const
{
    QString address = SEConfig::urlImages(SEConsts::Section_Products);
    QString folder = SEConfig::folderImages(SEConsts::Section_Products);

    QThread* thread = new QThread;
    ThreadLoadImage *loader = new ThreadLoadImage;
    loader->moveToThread(thread);
    foreach (QString s, *mListLoadImages)
        loader->listFiles()->append(s);
    mListLoadImages->clear();
    loader->setUrl(address);
    loader->setFolder(folder);
    connect(thread, SIGNAL(started()), loader, SLOT(process()));
    connect(loader, SIGNAL(finished()), thread, SLOT(quit()));
    connect(loader, &ThreadLoadImage::finished, this, &ListImagesModel::resetModel);
    connect(loader, SIGNAL(finished()), loader, SLOT(deleteLater()));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    thread->start();
}



