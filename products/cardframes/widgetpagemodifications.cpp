#include "widgetpagemodifications.h"
#include "ui_widgetpagemodifications.h"
#include "seconsts.h"

#include <QMenu>
#include <QDebug>
#include <QSortFilterProxyModel>

WidgetPageModifications::WidgetPageModifications(
        QWidget *parent, DataModificationGroup *group,
        ListDataItems *imagesProduct, const bool &isSelectMode,
        const bool &isMultiSelect) :
    QWidget(parent), ui(new Ui::WidgetPageModifications)
{    
    mGroup = group;    
    mImagesMode = imagesProduct != 0;
    mImagesProduct = imagesProduct;
    mIsSelectMode = isSelectMode;
    mIsMultiSelect = isMultiSelect;
    initialization();
}

WidgetPageModifications::~WidgetPageModifications()
{    
    delete ui;
    delete mSelectedModifications;
}

void WidgetPageModifications::initialization()
{
    ui->setupUi(this);
    mModel = new ModificationsModel(this, mGroup);
    QSortFilterProxyModel *proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setSourceModel(mModel);

    mIsAutoFilledModifications = 0;
    mModelImages = 0;
    ui->tableViewModifications->setModel(proxyModel);
    if (!mIsSelectMode)
        ui->tableViewModifications->setEditTriggers(QAbstractItemView::AllEditTriggers);
    else {
        ui->toolButtonSelectModifications->hide();
        ui->toolButtonDelete->hide();
        ui->actionDelete->setVisible(false);
        ui->actionSelectModifications->setVisible(false);
        ui->tableViewModifications->setSelectionBehavior(QAbstractItemView::SelectRows);
        if (mIsMultiSelect)
            ui->tableViewModifications->setSelectionMode(QAbstractItemView::ExtendedSelection);
        else ui->tableViewModifications->setSelectionMode(QAbstractItemView::SingleSelection);
    }

    mFormSelect = 0;
    if (!mImagesMode)
        ui->groupBoxImages->hide();
    else {
        mModelImages = new ModificationsImagesModel(this);        
        ui->tableViewImages->setModel(mModelImages);
        ui->tableViewImages->horizontalHeader()->setDefaultSectionSize(64);
        ui->tableViewImages->verticalHeader()->setDefaultSectionSize(64);
        ui->tableViewImages->horizontalHeader()->hide();
        mFormImages = new FrameListImages(this, SEConsts::Section_Products,
                                          mImagesProduct, true);
        mFormImages->setWindowTitle("Фото-галерея товара");
        connect(ui->tableViewImages->selectionModel(),
                &QItemSelectionModel::currentChanged, this,
                &WidgetPageModifications::onChangeImage);
    }
    mSelectedModifications = new ListModifications();
    initActions();
    initSignals();
}

void WidgetPageModifications::initSignals()
{
    ui->tableViewModifications->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tableViewModifications, SIGNAL(customContextMenuRequested(QPoint)),
            SLOT(showContextMenu(QPoint)));
    connect(ui->actionSelectModifications,
            SIGNAL(triggered()), SLOT(onSelectModifications()));
    connect(ui->actionDelete, SIGNAL(triggered()), SLOT(onDelete()));
    connect(ui->tableViewModifications->selectionModel(),
            SIGNAL(currentChanged(QModelIndex,QModelIndex)), SLOT(onChange()));
    connect(mModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)), SLOT(onModifiedValue(QModelIndex)));
    connect(ui->toolButtonAddImage, &QToolButton::clicked, this,
            &WidgetPageModifications::addImage);
    connect(ui->toolButtonDeleteImage, &QToolButton::clicked, this,
            &WidgetPageModifications::deleteImage);
    connect(ui->toolButtonMoveBackImage, &QToolButton::clicked, this,
            &WidgetPageModifications::sortImage);
    connect(ui->toolButtonMoveForwardImage, &QToolButton::clicked, this,
            &WidgetPageModifications::sortImage);
    if (mIsSelectMode)
        connect(ui->tableViewModifications, SIGNAL(doubleClicked(QModelIndex)),
                SIGNAL(doubleClicked()));
}

void WidgetPageModifications::initActions()
{
    ui->toolButtonSelectModifications->setDefaultAction(ui->actionSelectModifications);
    ui->toolButtonDelete->setDefaultAction(ui->actionDelete);
    ui->actionDelete->setEnabled(false);
}

bool WidgetPageModifications::existModification(DataModification *modification)
{
    for (int i = 0; i < mGroup->modifications()->size(); i++)
        if (mGroup->modifications()->at(i)->isEqualValues(
                modification))
            return true;
    return false;
}

void WidgetPageModifications::addImage()
{
    if (!mFormImages)
        return;

    if (mFormImages->exec() == QDialog::Accepted) {
        QModelIndexList list = ui->tableViewModifications->selectionModel()->selectedIndexes();
        QVector<int> listRows;
        for (int i = 0; i < list.size(); i++)
            if (!listRows.contains(list.at(i).row()))
                listRows.append(list.at(i).row());

        mModelImages->beginReset();
        const ListDataItems *selectedItems =
                mFormImages->selectedImages();
        for (int i = 0; i < selectedItems->size(); i++) {
            for (int k = 0; k < listRows.size(); k++) {
                DataModification *item = mGroup->modifications()->at(listRows.at(k));
                bool isExist = false;
                for (int j = 0; j < item->images()->size(); ++j)
                    if (item->images()->at(j)->getImageFile() == selectedItems->at(i)->getImageFile()) {
                        isExist = true;
                        break;
                    }

                if (!isExist) {
                    DataItem *image = new DataItem(selectedItems->at(i));
                    item->images()->append(image);
                }
            }
        }
        mModelImages->endReset();
        onModified();
    }
}

void WidgetPageModifications::deleteImage()
{
    if (!mFormImages ||
            !ui->tableViewImages->selectionModel()->currentIndex().isValid())
        return;

    QModelIndexList listM = ui->tableViewModifications->selectionModel()->selectedIndexes();
    QVector<int> listRows;
    for (int i = 0; i < listM.size(); i++)
        if (!listRows.contains(listM.at(i).row()))
            listRows.append(listM.at(i).row());

    mModelImages->beginReset();

    QModelIndexList list = ui->tableViewImages->selectionModel()->selectedColumns();
    QStringList namesImages;
    for (int i = 0; i < list.size(); i++)
       namesImages.append(mModelImages->getItems()->at(list.at(i).column())->getImageFile());

    for (int k = 0; k < listRows.size(); k++) {
        DataModification *item = mGroup->modifications()->at(listRows.at(k));

        for (int i = 0; i < item->images()->size(); i++)
            if (namesImages.contains(item->images()->at(i)->getImageFile()))
                item->images()->at(i)->setIsChecked(true);

        for (int i = item->images()->size() - 1; i >= 0; i--)
            if (item->images()->at(i)->getIsChecked())
                delete item->images()->takeAt(i);
    }

    mModelImages->endReset();
    onChangeImage();
    onModified();
}

void WidgetPageModifications::sortImage()
{
    bool isUp = sender() == ui->toolButtonMoveBackImage;
    int index = ui->tableViewImages->currentIndex().column();
    int newIndex = index;
    if (isUp)
        newIndex--;
    else newIndex++;
    if (newIndex < 0)
        newIndex = 0;
    if (newIndex == mModelImages->getItems()->size())
        newIndex = mModelImages->getItems()->size() - 1;
    mModelImages->beginReset();    
    if (isUp)
        mModelImages->getItems()->moveUp(mModelImages->dataItem(ui->tableViewImages->currentIndex()));
    else mModelImages->getItems()->moveDown(mModelImages->dataItem(ui->tableViewImages->currentIndex()));
    mModelImages->endReset();
    ui->tableViewImages->selectColumn(newIndex);
    onModified();
}

void WidgetPageModifications::onChangeImage()
{
    ui->toolButtonDeleteImage->setEnabled(ui->tableViewImages->currentIndex().isValid());
    ui->toolButtonMoveBackImage->setEnabled(
                ui->tableViewImages->currentIndex().isValid() &&
                ui->tableViewImages->currentIndex().column());
    ui->toolButtonMoveForwardImage->setEnabled(
                ui->tableViewImages->currentIndex().isValid() &&
                (ui->tableViewImages->currentIndex().column() <
                 mModelImages->getItems()->size() - 1));
}

void WidgetPageModifications::filledModifications()
{
    FormSelectModifications *formSelect = new FormSelectModifications(this, mGroup, true);

    setCursor(Qt::WaitCursor);
    if (formSelect->setSelectedList() && !mGroup->modifications()->size()) {
        mModel->beginReset();
        for (int i = 0; i < formSelect->selectedList()->size(); i++)
                mGroup->modifications()->append(new DataModification(formSelect->selectedList()->at(i)), false);
        mModel->endReset();
    }
    setCursor(Qt::ArrowCursor);
    onModified();

    delete formSelect;
}

DataModificationGroup *WidgetPageModifications::groupInfo() const
{
    return mGroup;
}

ListModifications *WidgetPageModifications::selectedModifications() const
{
    mSelectedModifications->clear();
    QModelIndexList list = ui->tableViewModifications->selectionModel()->selectedRows();
    for (int i = 0; i < list.size(); ++i)
        mSelectedModifications->append(mGroup->modifications()->at(list.at(i).row()));
    return mSelectedModifications;
}

void WidgetPageModifications::setIsAutoFilledModifications()
{
    mIsAutoFilledModifications = true;
}

void WidgetPageModifications::onSelectModifications()
{
    mFormSelect = new FormSelectModifications(this, mGroup);

    if (mFormSelect->exec() == QDialog::Accepted) {
        setCursor(Qt::WaitCursor);
        mModel->beginReset();        
        for (int i = 0; i < mFormSelect->selectedList()->size(); i++) {
            if (!existModification(mFormSelect->selectedList()->at(i)))
                mGroup->modifications()->append(
                            new DataModification(mFormSelect->selectedList()->at(i)));
        }        
        mModel->endReset();
        setCursor(Qt::ArrowCursor);
        onModified();
    }
    delete mFormSelect;
}

void WidgetPageModifications::onDelete()
{    
    QModelIndexList list = ui->tableViewModifications->
            selectionModel()->selectedIndexes();
    if (!list.size())
        return;

    for (int i = 0; i < list.size() ; i++)
        mGroup->modifications()->at(list.at(i).row())->setId("#");
    mModel->beginReset();
    for (int i = mGroup->modifications()->size() - 1; i >= 0; i--)
        if (mGroup->modifications()->at(i)->getId() == "#")
            delete mGroup->modifications()->takeAt(i);    
    mModel->endReset();
    onModified();
    onChange();
}

void WidgetPageModifications::onChange()
{
    ui->actionDelete->setEnabled(ui->tableViewModifications->
                                 currentIndex().isValid());
    emit changeItem(ui->tableViewModifications->currentIndex());
    if (ui->tableViewModifications->
         currentIndex().isValid() && mImagesMode) {
        mModelImages->beginReset();
        mModelImages->setListData(
                    mGroup->modifications()->at(
                        (ui->tableViewModifications->currentIndex().row()))->images());
        mModelImages->endReset();
        ui->toolButtonAddImage->setEnabled(true);
    } else {
        if (mModelImages) {
            mModelImages->unBindItems();
            mModelImages->resetModel();
        }
    }
}

void WidgetPageModifications::onModifiedValue(const QModelIndex &index)
{
    QModelIndexList list = ui->tableViewModifications->selectionModel()->selectedIndexes();
    if (list.size() > 1) {
        mModel->beginReset();
        QVariant value = mModel->data(index, Qt::EditRole);
        QModelIndex indexEdit;
        for (int i = 0; i < list.size(); ++i)
        {
            indexEdit = list.at(i);
            if (index.column() == indexEdit.column() && index.row() != indexEdit.row())
            {
                if (index.column() >= mGroup->columns()->size())
                switch (index.column() - mGroup->columns()->size())
                {
                case 0: {
                    if (value.toString().isEmpty())
                        mGroup->modifications()->at(
                                    indexEdit.row())->setCount(-1);
                    else mGroup->modifications()->at(
                                indexEdit.row())->setCount(value.toReal());
                    }
                    break;
                case 1: mGroup->modifications()->at(
                                indexEdit.row())->setPrice(value.toReal());
                        break;
                case 2: mGroup->modifications()->at(
                                indexEdit.row())->setPricePurscase(value.toReal());
                        break;
                case 3: mGroup->modifications()->at(
                                indexEdit.row())->setPriceSmallOpt(value.toReal());
                        break;
                case 4: mGroup->modifications()->at(
                                indexEdit.row())->setPriceOpt(value.toReal());
                }
            }
        }

        mModel->endReset();        
    }
    onModified();
}

void WidgetPageModifications::onModified()
{
    emit modified();
}

void WidgetPageModifications::showEvent(QShowEvent *)
{
    for (int i = ui->tableViewModifications->horizontalHeader()->count() - 4; i <
         ui->tableViewModifications->horizontalHeader()->count(); i++)
        ui->tableViewModifications->horizontalHeader()->resizeSection(i, 80);
    if (mModelImages)
        mModelImages->resetModel();
    if (mGroup->modifications()->size()) {
        ui->tableViewModifications->selectionModel()->
                setCurrentIndex(mModel->index(0, 0),
                                QItemSelectionModel::SelectCurrent | QItemSelectionModel::Rows);
    }
    if (mIsAutoFilledModifications)
        startTimer(100);
}

void WidgetPageModifications::timerEvent(QTimerEvent *e)
{
    killTimer(e->timerId());
    if (mIsAutoFilledModifications)
        filledModifications();
}

void WidgetPageModifications::showContextMenu(QPoint point)
{
    QPoint globalPos;

    if (sender()->inherits("QAbstractScrollArea"))
        globalPos = ((QAbstractScrollArea*)sender())->viewport()->mapToGlobal(point);
    else globalPos = ((QWidget*)sender())->mapToGlobal(point);

    QMenu menu;
    if (ui->tableViewModifications->currentIndex().column() <
            mGroup->columns()->size()) {
        menu.addAction(ui->actionSelectModifications);
        menu.addAction(ui->actionDelete);
        menu.exec(globalPos);
    }
}

