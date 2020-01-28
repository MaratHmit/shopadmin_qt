#include "frameliblistimages.h"
#include "ui_frameliblistimages.h"
#include "filewebuploader.h"
#include "filewebloader.h"
#include "seconfig.h"
#include "apiadapter.h"
#include "seutils.h"
#include "frameimage.h"
#include "field.h"

#include <QDir>
#include <QThread>
#include <QNetworkAccessManager>

FrameLibListDataItems::FrameLibListDataItems(QWidget *parent, const int &imageSection) :
  QWidget(parent),
  ui(new Ui::FrameLibListDataItems)
{
    ui->setupUi(this);        

    sectionImage = imageSection;

    initVariables();
    initControls();
    initSignals();
}

FrameLibListDataItems::~FrameLibListDataItems()
{
    delete ui;    
    delete preview;
    delete selectedItems;
}

void FrameLibListDataItems::showImages()
{
    if (isShown)
        return;

    refreshData();
    isShown = true;
}

void FrameLibListDataItems::resetImages()
{
    isShown = false;
    if (isVisible())
        refreshData();
}

ListDataItems *FrameLibListDataItems::getItems() const
{
    return items;
}

QString FrameLibListDataItems::getCurrentFolder()
{
    return SEConfig::getFolderImages(sectionImage);
}

void FrameLibListDataItems::resetRequest()
{
    request.clear();

    request.setSearchText(ui->widgetSearch->searchText().trimmed());
    request.setAscendingOrder(ui->treeViewImages->header()->sortIndicatorOrder() == Qt::AscendingOrder);

    request.setSectionImage(SEConfig::getSectionImage(sectionImage));
    request.setOffset(numPage * ui->widgetNavigator->getLimit());
    request.setLimit(ui->widgetNavigator->getLimit());
}

void FrameLibListDataItems::deleteSelectedFiles()
{
    QModelIndexList list = ui->treeViewImages->selectionModel()->selectedRows();
    modelItems->deleteItems(list);
    countItems -= list.count();

    QDir dir(SEConfig::getFolderCache());
    QStringList listFiles = dir.entryList(QDir::Files);
    foreach (QString entry, listFiles)
    {
        QString entryAbsPath = dir.absolutePath() + QDir::separator() + entry;
        QFile::setPermissions(entryAbsPath, QFile::ReadOwner | QFile::WriteOwner);
        QFile::remove(entryAbsPath);
    }

    dir.setPath(getCurrentFolder());
    listFiles = dir.entryList(QDir::Files);
    foreach (QString entry, listFiles)
    {
        QString entryAbsPath = dir.absolutePath() + QDir::separator() + entry;
        QFile::setPermissions(entryAbsPath, QFile::ReadOwner | QFile::WriteOwner);
        QFile::remove(entryAbsPath);
    }
}

void FrameLibListDataItems::addFiles(const QStringList listFiles)
{    

    QStringList listNames;
    foreach (QString fileName, listFiles) {
        QFileInfo fileInfo(fileName);
        listNames.append(fileInfo.fileName());
    }

    QStringList validNames = getValidNames(listNames);
    if (validNames.count() != listNames.count())
        return;

    QStringList listUploades;
    preloader->startAnimation();
    SEConfig::createDir(getCurrentFolder());
    foreach (QString fileName, listFiles) {
        QPixmap pixmap(fileName);
        if (pixmap.height() && pixmap.width()) {
            QFile file(fileName);
            QString newName = validNames.at(listFiles.indexOf(fileName));
            QString newPath = getCurrentFolder() + QDir::separator() + newName;
            file.copy(newPath);
            listUploades.append(newPath);
            QImage img(newPath);
            int w = 0, h = 0;
            bool isScaledOrigin = FrameImage::isScaledOriginImage(&w, &h);
            if (isScaledOrigin) {
                QImage origin = img.scaled(w, h, Qt::KeepAspectRatio, Qt::SmoothTransformation);
                origin.save(newPath);
            };
        } else QMessageBox::information(this, tr("Добавление изображения"),
                                        fileName + " не является изображением!", QMessageBox::Ok);
    }

    uploadImages(listUploades);
    preloader->stopAnimation();
}

void FrameLibListDataItems::deleteFiles()
{
    if (!ui->treeViewImages->currentIndex().isValid())
        return;

    if (ui->progressBar->isVisible()) {
        QMessageBox::information(this, "Удаление изображений",
                                 "Дождитесь завершения выполнения текущий операции!");
        return;
    }

    QString msg = tr("Удалить выбранные файлы изображения?");
    QMessageBox::StandardButton mesRepl;

    mesRepl = QMessageBox::question(this, tr("Подтверждение удаления"),
                                    msg, QMessageBox::Yes | QMessageBox::No);
    if(mesRepl == QMessageBox::Yes) {
      RequestImages request;
      request.setSectionImage(SEConfig::getSectionImage(sectionImage));

      QModelIndexList list = ui->treeViewImages->selectionModel()->selectedRows();
      for (int i = 0; i < list.size(); ++i)
            request.addFileName(modelItems->dataItem(list.at(i))->getName());

      preloader->startAnimation();
      ApiAdapter::getApi()->deleteImages(request) ? deleteSelectedFiles() : refreshData();
      ApiAdapter::getApi()->clearCache();
      preloader->stopAnimation();
    }
}

void FrameLibListDataItems::setSelectMode(const bool &isMultiSelect)
{
    isSelectMode = true;
    if (!isMultiSelect)
        ui->treeViewImages->setSelectionMode(QAbstractItemView::SingleSelection);
}

void FrameLibListDataItems::deleteUnusedFiles()
{
    if (ui->progressBar->isVisible()) {
        QMessageBox::information(this, "Удаление изображений",
                                 "Дождитесь завершения выполнения текущий операции!");
        return;
    }

    QStringList listImages;
    RequestImages request;
    request.addStringValue("isUnused", "1");
    request.setSectionImage(SEConfig::getSectionImage(sectionImage));
    QString msg = "Вы действительно хотите удалить неиспользуемые изображения?";
    QMessageBox::StandardButton mesRepl;
    mesRepl = QMessageBox::question(this, tr("Подтверждение удаления"),
                                    msg, QMessageBox::Yes | QMessageBox::No);
    if(mesRepl != QMessageBox::Yes)
        return;

    preloader->startAnimation();
    if (ApiAdapter::getApi()->deleteImages(request)) {
        numPage = 0;
        refreshData();
    }
    preloader->stopAnimation();
}

bool FrameLibListDataItems::isCurrentRow()
{
    return ui->treeViewImages->currentIndex().isValid();
}

QString FrameLibListDataItems::selectionFile()
{
    if (ui->treeViewImages->currentIndex().isValid())
        return modelItems->dataItem(ui->treeViewImages->currentIndex())->getName();
    else return QString();
}

QStringList FrameLibListDataItems::selectionFiles()
{
    QStringList result;
    QString fileName;
    QModelIndexList list = ui->treeViewImages->selectionModel()->selectedRows();
    for (int i = 0; i < list.size(); i++) {
        fileName = modelItems->dataItem(list.at(i))->getName();
        if (!result.contains(fileName))
            result << fileName;
    }
    return result;
}

const ListDataItems *FrameLibListDataItems::selectionItems()
{
    selectedItems->clear();

    QModelIndexList list = ui->treeViewImages->selectionModel()->selectedRows();
    for (int i = 0; i < list.size(); i++)
        selectedItems->append(modelItems->dataItem(list.at(i)), false);

    return selectedItems;
}

bool FrameLibListDataItems::uploadYandexImage(QWidget *widget, DataItem *item)
{
    widget->setCursor(Qt::WaitCursor);
    QString newFileName = ApiAdapter::getApi()->uploadImageToYandex(item->getImageFile());
    widget->setCursor(Qt::ArrowCursor);
    if (!newFileName.isEmpty()) {
        item->setImageFile(newFileName);
        return true;
    } else
        QMessageBox::critical(widget, "Выгрузка изображения",
                              "Не удаётся выгрузить изображение на сервис Яндекс.Фотки");
    return false;
}

void FrameLibListDataItems::uploadImages(QStringList list)
{    
    qint64 sizeFiles = getSizeFiles(&list);
    showProgressBar(sizeFiles);

    QThread* thread = new QThread;
    FileWebUploader *uploader = new FileWebUploader;
    uploader->moveToThread(thread);
    foreach (QString s, list)
        uploader->listFiles()->append(s);
    connect(thread, SIGNAL(started()), uploader, SLOT(process()));
    connect(uploader, SIGNAL(finished()), thread, SLOT(quit()));
    connect(uploader, SIGNAL(finished()), uploader, SLOT(deleteLater()));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    connect(uploader, SIGNAL(uploadBytes(int)), ui->progressBar, SLOT(setValue(int)));
    connect(ui->pushButtonBreakLoad, SIGNAL(clicked()), uploader, SLOT(stop()));
    connect(uploader, SIGNAL(finished()), SLOT(hideProgressBar()));
    connect(uploader, SIGNAL(uploadFinished(QJsonObject)), SLOT(onUploadFinished(QJsonObject)));
    uploader->setSection(sectionImage);
    thread->start();
}

void FrameLibListDataItems::showPreview()
{
    if (!ui->treeViewImages->currentIndex().isValid())
        return;

    QString imageUrl =
            modelItems->dataItem(ui->treeViewImages->currentIndex())->property("imageUrl").toString();
    if (imageUrl.isEmpty())
        imageUrl = SEConfig::getUrlImages(sectionImage) + "/" +
                modelItems->dataItem(ui->treeViewImages->currentIndex())->getName();

    preloader->startAnimation();
    preview->hide();
    preview->setPixmap(ApiAdapter::getApi()->getImageFromURL(imageUrl));
    preloader->stopAnimation();
    preview->setWindowModality(Qt::ApplicationModal);
    preview->show();
}

void FrameLibListDataItems::cellClicked(const QModelIndex &index)
{
    if (!index.column())
        showPreview();
}

void FrameLibListDataItems::cellDoubleClicked(const QModelIndex &)
{
    if (isSelectMode)
        emit doubleClickedItem();
    else showPreview();
}

void FrameLibListDataItems::onUploadFinished(const QJsonObject object)
{
    modelItems->addItem(new DataItem(object), false);
    countItems++;
    currentItemChaned();
}

void FrameLibListDataItems::search(const QString &text)
{
    QString s = text.toLower();
    for (int i = 0; i < modelItems->rowCount(); ++i) {
         QString fileName = modelItems->dataItem(modelItems->index(i, 0))->getName().toLower();
         ui->treeViewImages->setRowHidden(i, QModelIndex(), !fileName.contains(s));
    }
}

const QStringList FrameLibListDataItems::getValidNames(const QStringList &names)
{
    DataItem item;
    foreach (QString name, names)
        item.addInListValues(name);
    item.setProperty("section", SEConfig::getSectionImage(sectionImage));

    if (ApiAdapter::getApi()->getInfoData(SEConsts::API_FILESIMAGES, &item)) {
        if (item.property("newNames").isValid())
            return item.property("newNames").toStringList();
    }
    return QStringList();
}

void FrameLibListDataItems::initSignals()
{
    connect(ui->widgetNavigator, SIGNAL(goNextPage()), this, SLOT(goNextPage()));
    connect(ui->widgetNavigator, SIGNAL(goPreviosPage()), this, SLOT(goPreviosPage()));
    connect(ui->widgetNavigator, SIGNAL(goLastPage()), this, SLOT(goLastPage()));
    connect(ui->widgetNavigator, SIGNAL(goFirstPage()), this, SLOT(goFirstPage()));
    connect(ui->widgetNavigator, SIGNAL(refreshDataStart()), this, SLOT(goFirstPage()));

    connect(ui->treeViewImages->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
            SLOT(currentItemChaned()));
    connect(ui->treeViewImages, SIGNAL(doubleClicked(QModelIndex)), SLOT(cellDoubleClicked(QModelIndex)));
    connect(ui->treeViewImages, SIGNAL(clicked(QModelIndex)), SLOT(cellClicked(QModelIndex)));

    connect(ui->widgetSearch, SIGNAL(runSearch()), SLOT(runSearch()));
    connect(timerSearch, &QTimer::timeout, this, &FrameLibListDataItems::goFirstPage);
}

void FrameLibListDataItems::initVariables()
{
    isSelectMode = false;
    numPage = 0;
    countItems = 0;
    isRestoreStates = false;
    items = new ListDataItems;
    selectedItems = new ListDataItems;
    modelItems = new NameTableModel(this, items);
    modelItems->clearFields();
    modelItems->appendField(new Field("image", "Фото"));    
    modelItems->appendField(new Field("title", "Наименование"));
    modelItems->appendField(new Field("sizeDisplay", "Размер (px)", Qt::AlignCenter));
    modelItems->appendField(new Field("weight", "Вес (байт)", Qt::AlignCenter));
    ui->treeViewImages->setModel(modelItems);
}

void FrameLibListDataItems::initControls()
{
    ui->widgetProgress->setHidden(true);    
    ui->widgetSearch->setIsHotSearchMode();
    ui->widgetNavigator->setLimit(100);
    if (sectionImage == 8) {        
        ui->widgetNavigator->setEnabledLimit(false);
        ui->widgetNavigator->setVisibleButtons(false, true, true, false);
        ui->widgetSearch->hide();
    };
    preview = new QLabel;
    preloader = new Preloader(this);

    timerSearch = new QTimer(this);
    timerSearch->setSingleShot(true);

    restoreStateTree();
}

void FrameLibListDataItems::refreshData()
{    
    preloader->startAnimation();
    resetRequest();
    SEConfig::saveStateModel(ui->treeViewImages);
    if (ApiAdapter::getApi()->getListData(SEConsts::API_FILESIMAGES, items, request)) {
        countItems = items->countInBase();
        modelItems->resetModel();
    }
    setStateTree();
    preloader->stopAnimation();
}

void FrameLibListDataItems::currentItemChaned()
{
    if (items->size() > 0 && ui->treeViewImages->currentIndex().isValid())
        ui->widgetNavigator->setNumRecord(numPage *  ui->widgetNavigator->getLimit() +
                                          ui->treeViewImages->currentIndex().row() + 1, countItems);
    else ui->widgetNavigator->setNumRecord(0, countItems);

    emit selected();
}

void FrameLibListDataItems::on_pushButtonBreakLoad_clicked()
{
    hideProgressBar();
    refreshData();
}

void FrameLibListDataItems::goNextPage()
{
    numPage++;
    if (numPage > int(countItems / ui->widgetNavigator->getLimit()))
        numPage = int(countItems / ui->widgetNavigator->getLimit());
    refreshData();
}

void FrameLibListDataItems::goPreviosPage()
{
    numPage--;
    if (numPage < 0)
        numPage = 0;
    refreshData();
}

void FrameLibListDataItems::goLastPage()
{
    numPage = int(countItems / ui->widgetNavigator->getLimit());
    refreshData();
}

void FrameLibListDataItems::goFirstPage()
{
    numPage = 0;
    refreshData();
}

void FrameLibListDataItems::runSearch()
{
    if (timerSearch->isActive())
        timerSearch->stop();
    timerSearch->start(300);
}

void FrameLibListDataItems::showProgressBar(const qint64 &max)
{
    if (!max || ui->widgetProgress->isVisible())
        return;

    ui->widgetProgress->setVisible(true);
    ui->progressBar->setMaximum(0);
    ui->progressBar->setValue(0);
    ui->progressBar->setMaximum(max);
}

void FrameLibListDataItems::restoreStateTree()
{
    isRestoreStates = SEConfig::restoreTableState(ui->treeViewImages->header(),
                                                  ui->treeViewImages->objectName(), isSelectMode);
}

void FrameLibListDataItems::setStateTree()
{
    if (!isRestoreStates)
        setStateTreeDefault();
    SEConfig::restoreStateModel(ui->treeViewImages);
    if (!ui->treeViewImages->currentIndex().isValid() && items->size())
        ui->treeViewImages->selectionModel()->setCurrentIndex(
                    modelItems->index(0, 0), QItemSelectionModel::SelectCurrent | QItemSelectionModel::Rows);
}

void FrameLibListDataItems::setStateTreeDefault()
{
    for (int i = 1; i < modelItems->columnCount() - 1; i++)
        ui->treeViewImages->resizeColumnToContents(i);
    isRestoreStates = true;
}

void FrameLibListDataItems::saveStateTree()
{
    SEConfig::saveTableState(ui->treeViewImages->header(), ui->treeViewImages->objectName(), isSelectMode);
}

void FrameLibListDataItems::hideProgressBar()
{
    ui->widgetProgress->setHidden(true);
}
