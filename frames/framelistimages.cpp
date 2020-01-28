#include "framelistimages.h"
#include "ui_framelistimages.h"
#include "seconfig.h"
#include "filewebloader.h"
#include "filewebuploader.h"
#include "activelabel.h"
#include "formlibimages.h"
#include "apiadapter.h"
#include "dialogsetimageurl.h"
#include "frameimage.h"

#include <QSettings>
#include <QLabel>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QThread>
#include <QFileDialog>
#include <QCheckBox>
#include <QDebug>

FrameListImages::FrameListImages(QWidget *parent, const int &section,
                                 ListDataItems *listImages, const bool &isSelectMode) :
    QDialog(parent),
    ui(new Ui::FrameListImages)
{
    sectionImage = section;
    ui->setupUi(this);    
    mPreview = new QLabel();
    mListImages = listImages;
    mSelectedImages = new ListDataItems();
    mIsSelectMode = isSelectMode;
    if (mIsSelectMode)
        setWindowFlags(Qt::Dialog);
    else setWindowFlags(Qt::Widget);
    mPreview->setWindowTitle("Просмотр изображения");
    mPreview->setStyleSheet("background-image: url(:/16/icons/16x16/image_background.png);");
    mPreview->setScaledContents(true);
    mModel = new NameTableModel(this, listImages, isSelectMode);    
    mModel->setEditableMode(!isSelectMode);
    mModel->prependField(new Field("isMain", tr("Главн.")));
    mModel->prependField(new Field("image", tr("Изображ.")));
    mDelegate = new ListImagesDelegate(mModel);
    connect(mDelegate, &ListImagesDelegate::modified, this, &FrameListImages::modified);
    ui->tableViewImages->setModel(mModel);
    ui->tableViewImages->setItemDelegate(mDelegate);
    ui->tableViewImages->setEditTriggers(QAbstractItemView::AllEditTriggers);
    ui->toolButtonOpenImageFromYF->setVisible(!SEConfig::yandexLogin.isEmpty() &&
                                              !SEConfig::yandexToken.isEmpty());
    if (!isSelectMode) {
        ui->toolButtonSelect->hide();
    } else {        
        ui->toolButtonDeleteImage->hide();
        ui->line->hide();
        ui->toolButtonDown->hide();
        ui->toolButtonUp->hide();
    }
    setSizeColumnsTable();
    initSignals();
}

FrameListImages::~FrameListImages()
{    
    mModel->unBindItems();
    delete ui;
    delete mPreview;
}

void FrameListImages::fillControls()
{ 
    for (int i = 0; i < ui->tableViewImages->model()->rowCount(); i++)
        ui->tableViewImages->verticalHeader()->resizeSection(i, 64);
}

void FrameListImages::clearInfo()
{

}

void FrameListImages::hideCheckedsColumn()
{
    ui->tableViewImages->hideColumn(1);
}

void FrameListImages::setDefaultAlt(const QString &defaultAlt)
{
    mDefaultAlt = defaultAlt;
}

const ListDataItems *FrameListImages::selectedImages()
{
    if (mSelectedImages->size())
        return mSelectedImages;

    QModelIndexList list =
        ui->tableViewImages->selectionModel()->selectedRows();
    for (int i = 0; i < list.size(); i++)
        mSelectedImages->append(mListImages->at(list.at(i).row()));
    return mSelectedImages;
}

void FrameListImages::onCellClicked(QModelIndex index)
{
    if (!mIsSelectMode) {
       if (index.column() == 0)
               showPreview(mListImages->at(index.row())->getImageFile());
       if (index.column() == 1) {
           mModel->beginReset();
           QModelIndex index = ui->tableViewImages->currentIndex();
           DataItem *item = mModel->dataItem(index);
           item->setIsMain(!item->getIsMain());
           mModel->endReset();
           ui->tableViewImages->selectionModel()->setCurrentIndex(
                       index, QItemSelectionModel::SelectCurrent | QItemSelectionModel::Rows);
           emit modified();
       }
    }
}

void FrameListImages::onButtonOpenImageClicked()
{
    QString pictureFolder;
    QSettings settings;
    pictureFolder = settings.value("pictureFolder", SEConfig::getFolderImages(sectionImage)).toString();

    QStringList files =
           QFileDialog::getOpenFileNames(this, tr("Добавить изображения"), pictureFolder,
                                         tr("Изображения") + "(*.png *.xpm *.jpg)");
    if (files.size())
        pictureFolder = QFileInfo(files.at(0)).absolutePath();
    for (int i = 0; i < files.size(); i++) {
        QPixmap pm(files.at(i));
        if (pm.height() && pm.width()) {
            DataItem *image = new DataItem;
            image->setId("-2");
            image->setIsMain(!mListImages->size());
            image->setImageFile(files.at(i));
            image->setImageAlt(mDefaultAlt);
            if (createImageFile(image))
                mModel->addItem(image);
            else delete image;
            fillControls();
            onModified();
        } else QMessageBox::information(this, tr("Добавление изображения"),
                                        files.at(i) + " не является изображением!", QMessageBox::Ok);
    }
     mListImages->setModified();
     if (!pictureFolder.isEmpty()) {
         settings.setValue("pictureFolder", pictureFolder);
         settings.sync();
     }
}

void FrameListImages::onButtonSetImageFromURL()
{
    DialogSetImageUrl dialogImage(this);
    if (dialogImage.exec() == QDialog::Accepted) {
        DataItem *image = new DataItem;        
        image->setId("-1");
        image->setIsMain(mListImages->size() == 1);
        image->setImageFile(dialogImage.imageURL());
        image->setProperty("imageUrlPreview", dialogImage.imageURL());
        mModel->addItem(image);
        fillControls();
        onModified();
    }
}

void FrameListImages::onButtonLoadImageFromLibClicked()
{
    appendImageFromLib(sectionImage);
}

void FrameListImages::onButtonLoadImageFromYF()
{
    appendImageFromLib(SEConsts::Section_YandexFotos);
}

void FrameListImages::onButtonDeleteImageClicked()
{
    QModelIndexList list = ui->tableViewImages->selectionModel()->selectedRows();
    mModel->deleteItems(list);
    onModified();
    if (mModel->getItems()->size())
        ui->tableViewImages->selectionModel()->setCurrentIndex(
                    mModel->index(0, 0), QItemSelectionModel::SelectCurrent |
                    QItemSelectionModel::Rows);
}

void FrameListImages::onChange()
{
    ui->toolButtonSelect->setEnabled(
                ui->tableViewImages->currentIndex().isValid());
}

void FrameListImages::showEvent(QShowEvent *)
{
    mModel->resetModel();
    mSelectedImages->clear();
}

void FrameListImages::showPreview(const QString &fileName, const bool &isFullName)
{
    mPreview->hide();

    if (fileName.indexOf("http:") == 0) {
        setCursor(Qt::WaitCursor);
        QPixmap pm(ApiAdapter::getApi()->getImageFromURL(fileName));
        mPreview->setPixmap(pm);
        setCursor(Qt::ArrowCursor);
    } else {
        QString folder = getCurrentFolder();
        QString fullName;
        if (!isFullName)
            fullName = folder + QDir::separator() + fileName;
        else fullName = fileName;
        QFile fileImage(fullName);
        if (fileImage.exists()) {
            QPixmap pm(fileImage.fileName());
            mPreview->setPixmap(pm);
        } else {
            setCursor(Qt::WaitCursor);
            mPreview->setPixmap(ApiAdapter::getApi()->
                               getImageFromURL(SEConfig::getUrlImages(sectionImage) + "/" + fileName));
            setCursor(Qt::ArrowCursor);
        }
    }

    mPreview->setWindowModality(Qt::ApplicationModal);
    mPreview->show();
}

void FrameListImages::onModified()
{
    emit modified();
}

void FrameListImages::onCheckedImage()
{
    QCheckBox *box = dynamic_cast <QCheckBox *> (sender());
    for (int i = 0; i < mListImages->size(); i++)
        if (mListImages->at(i)->getId() == box->windowTitle()) {
            mListImages->at(i)->setIsMain(box->isChecked());
            return;
        }
}

void FrameListImages::onSort()
{
    if (!ui->tableViewImages->currentIndex().isValid())
        return;

    bool isUp = ui->toolButtonUp == sender();    
    DataItem *item = mModel->dataItem(ui->tableViewImages->currentIndex());
    int index = ui->tableViewImages->currentIndex().row();
    int newIndex = index;
    if (isUp)
        newIndex--;
    else newIndex++;
    if (newIndex < 0)
        newIndex = 0;
    if (newIndex == mListImages->size())
        newIndex = mListImages->size() - 1;
    mModel->beginReset();
    if (isUp)
        mListImages->moveUp(item);
    else mListImages->moveDown(item);
    mModel->endReset();
    ui->tableViewImages->selectRow(newIndex);    
    emit modified();
}

bool FrameListImages::createImageFile(DataItem *item)
{
    QString oldFileName(item->getImageFile());
    QFile file(oldFileName);
    if (!file.exists())
        return false;

    QFileInfo fileInfo(oldFileName);
    QString folder = getCurrentFolder();
    SEConfig::createDir(folder);    

    QString imageName;
    QString newFileName;
    QString addSuffix;
    QString suffix = fileInfo.suffix();
    if (!suffix.isEmpty())
        suffix = "." + suffix;
    imageName = fileInfo.completeBaseName().replace(QString(" "), QString("_"));
    do {
        newFileName = folder + QDir::separator() + imageName + addSuffix + suffix;
        addSuffix += "_";
    } while (QFile::exists(newFileName));
    if (!addSuffix.isEmpty())
        addSuffix.remove(addSuffix.length() - 1, 1);
    imageName = imageName + addSuffix + suffix;
    item->setImageFile(imageName);
    if (!file.copy(newFileName)) {
        QMessageBox::critical(this, tr("Вставка изображения"),
                tr("Не удаётся скопировать файл в папку проекта!"));
        return false;
    }

    int w = 0, h = 0;
    bool isScaledOrigin = FrameImage::isScaledOriginImage(&w, &h);
    if (isScaledOrigin) {
        QImage img(newFileName);
        QImage origin = img.scaled(w, h, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        origin.save(newFileName);
    }

    QPixmap pixmap(newFileName);
    *item->getPixmap() = pixmap.scaled(QSize(SEConsts::IMAGE_SIZE_PREVIEW, SEConsts::IMAGE_SIZE_PREVIEW),
                                    Qt::KeepAspectRatio, Qt::SmoothTransformation);
    uploadImage(newFileName);    
    return true;
}

void FrameListImages::uploadImage(const QString &fileName)
{
    QThread* thread = new QThread;
    FileWebUploader *uploader = new FileWebUploader;
    uploader->moveToThread(thread);
    uploader->listFiles()->append(fileName);
    connect(thread, SIGNAL(started()), uploader, SLOT(process()));
    connect(uploader, SIGNAL(finished()), thread, SLOT(quit()));
    connect(uploader, SIGNAL(finished()), uploader, SLOT(deleteLater()));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    uploader->setSection(sectionImage);
    thread->start();
}

void FrameListImages::appendImageFromLib(const int section)
{
    FormLibImages *libImages = new FormLibImages();

    libImages->setSelectMode(section, true);
    libImages->setWindowTitleBySection(section);

    if (libImages->exec() == QDialog::Accepted) {
        const ListDataItems *items = libImages->selectionItems();
        for (int i = 0; i < items->size(); i++) {
            DataItem *item = new DataItem(items->at(i));
            item->setId("-1");
            item->setIsMain(mListImages->size() == 1);
            item->setImageAlt(mDefaultAlt);
            item->setImageFile(item->getName());
            mModel->addItem(item);
            fillControls();
            onModified();
        }
    }
    mListImages->setModified();
    delete libImages;
}

void FrameListImages::initSignals()
{
    connect(ui->tableViewImages, &QTableView::clicked, this,
            &FrameListImages::onCellClicked);
    connect(ui->toolButtonOpenImage, &QAbstractButton::clicked, this,
            &FrameListImages::onButtonOpenImageClicked);
    connect(ui->toolButtonOpenImageFromLib, &QAbstractButton::clicked, this,
            &FrameListImages::onButtonLoadImageFromLibClicked);
    connect(ui->toolButtonDeleteImage, &QAbstractButton::clicked,
            this, &FrameListImages::onButtonDeleteImageClicked);
    connect(ui->toolButtonSetURLImageImage, &QAbstractButton::clicked, this,
            &FrameListImages::onButtonSetImageFromURL);
    connect(ui->toolButtonOpenImageFromYF, &QAbstractButton::clicked, this,
            &FrameListImages::onButtonLoadImageFromYF);
    connect(ui->toolButtonUp, &QToolButton::clicked, this, &FrameListImages::onSort);
    connect(ui->toolButtonDown, &QToolButton::clicked, this, &FrameListImages::onSort);
    if (mIsSelectMode) {
        connect(ui->tableViewImages->selectionModel(),
                SIGNAL(currentChanged(QModelIndex,QModelIndex)),
                SLOT(onChange()));
        connect(ui->toolButtonSelect, &QToolButton::clicked, this,
                &FrameListImages::accept);
        connect(ui->tableViewImages, SIGNAL(doubleClicked(QModelIndex)),
                SLOT(accept()));
    }
}

QString FrameListImages::getCurrentFolder()
{
    return SEConfig::getFolderImages(sectionImage);
}

QString FrameListImages::getCurrentUrl()
{
    return SEConfig::getUrlImages(sectionImage);
}

void FrameListImages::setSizeColumnsTable()
{   ui->tableViewImages->verticalHeader()->setDefaultSectionSize(64);
    ui->tableViewImages->horizontalHeader()->resizeSection(0, 65);
    ui->tableViewImages->horizontalHeader()->resizeSection(1, 60);
    ui->tableViewImages->horizontalHeader()->setStretchLastSection(true);
    ui->tableViewImages->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->tableViewImages->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->tableViewImages->setSelectionBehavior(QAbstractItemView::SelectRows);    
    if (mIsSelectMode) {
        ui->tableViewImages->verticalHeader()->hide();
        ui->tableViewImages->hideColumn(1);
    }
}
