#include "formlibimages.h"
#include "ui_formlibimages.h"

#include <QFile>
#include <QDir>
#include <QPixmap>
#include <QImage>
#include <QStandardPaths>
#include <QFileDialog>
#include <QSettings>

#include "filewebloader.h"
#include "filewebuploader.h"
#include "userpermission.h"

FormLibImages::FormLibImages(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FormLibImages),    
    mListButtons(new QList<QPushButton *>)
{
    ui->setupUi(this);    
    setWindowFlags(Qt::Window);
    mIsSelectionMode = false;

    ui->toolButtonSelect->setHidden(true);
    ui->stackedWidget->setCurrentIndex(0);        
    initSectionsButtons();
    initPermissions();
}

FormLibImages::~FormLibImages()
{
    delete ui;
    delete mListButtons;
}

void FormLibImages::resetImages()
{
    for (int i = 0; i < ui->stackedWidget->count(); ++i) {
        qobject_cast<FrameLibListDataItems *>
                     (ui->stackedWidget->widget(i))->resetImages();
    }
}

void FormLibImages::refreshImages()
{    
    libListDataItems()->refreshData();
}

void FormLibImages::showEvent(QShowEvent *)
{
    isRefreshed = true;
    ui->pushButtonYandexPhotos->setVisible(!SEConfig::yandexLogin.isEmpty() &&
                                           !SEConfig::yandexToken.isEmpty());
    ui->lineYandexPhotos->setVisible(ui->pushButtonYandexPhotos->isVisible());
    startTimer(100);
}

void FormLibImages::timerEvent(QTimerEvent *event)
{    
    killTimer(event->timerId());    
    if (isRefreshed) {
        refreshImages();
        isRefreshed = false;
    }
}

void FormLibImages::on_toolButtonSelectSection()
{
    if (QAbstractButton *button = qobject_cast<QAbstractButton *> (sender())) {
        if (button->autoRepeatInterval() < ui->stackedWidget->count())
            ui->stackedWidget->setCurrentIndex(button->autoRepeatInterval());        
        ui->toolButtonRemoveUnused->setVisible(button != ui->pushButtonYandexPhotos);
        ui->toolButtonDelete->setVisible(button != ui->pushButtonYandexPhotos);
        ui->line->setVisible(ui->toolButtonRemoveUnused->isVisible());
        refreshImages();
    }
}


void FormLibImages::setSelectMode(const int &section, const bool &isMultiSelect)
{
    mIsSelectionMode = true;
    ui->widgetSections->setHidden(true);
    ui->toolButtonSelect->setVisible(true);
    ui->toolButtonDelete->hide();
    ui->stackedWidget->setCurrentIndex(section);

    libListDataItems()->setSelectMode(isMultiSelect);
    libListDataItems()->refreshData();

    connect(libListDataItems(), SIGNAL(doubleClickedItem()), SLOT(accept()));
}

void FormLibImages::setWindowTitleBySection(const int &section)
{
    QString caption;
    switch (section) {
    case SEConsts::Section_Products:
        caption = tr("товары");
        break;
    case SEConsts::Section_GroupsProducts:
        caption = tr("группы товаров");
        break;
    case SEConsts::Section_News:
        caption = tr("новости");
        break;
    case SEConsts::Section_Brands:
        caption = tr("бренды");
        break;
    case SEConsts::Section_Features:
        caption = tr("параметры товаров");
        break;
    case SEConsts::Section_ShopPayment:
        caption = tr("платежные системы");
        break;
    case SEConsts::Section_Contacts:
        caption = tr("контакты");
        break;
    case SEConsts::Section_ShopSections:
        caption = tr("разделы");
        break;
    case SEConsts::Section_YandexFotos:
        caption = tr("Яндекс.Фотки");
        break;
    default:
        break;
    }
    setWindowTitle(windowTitle() + " [" + caption + "]");
}

QStringList FormLibImages::selectionFiles()
{
    return libListDataItems()->selectionFiles();
}

const ListDataItems *FormLibImages::selectionItems()
{
    return libListDataItems()->selectionItems();
}

QString FormLibImages::selectionFile()
{
    return libListDataItems()->selectionFile();
}

void FormLibImages::showImages()
{
    startTimer(10);
}

void FormLibImages::initSectionsButtons()
{
    mListButtons->append(ui->pushButtonProducts);
    mListButtons->append(ui->pushButtonGroupsProducts);
    mListButtons->append(ui->pushButtonNews);
    mListButtons->append(ui->pushButtonBrands);
    mListButtons->append(ui->pushButtonFeatures);
    mListButtons->append(ui->pushButtonShopPayment);
    mListButtons->append(ui->pushButtonContacts);
    mListButtons->append(ui->pushButtonSESections);
    mListButtons->append(ui->pushButtonYandexPhotos);

    for (int i = 0; i < mListButtons->size(); i++) {
        // использум свойство AutoRepeatDelay не по назначению, а для метки кнопки
        mListButtons->at(i)->setAutoRepeatInterval(i);
        FrameLibListDataItems *f = new FrameLibListDataItems(ui->stackedWidget, i);
        ui->stackedWidget->addWidget(f);
        connect(mListButtons->at(i), SIGNAL(clicked()), SLOT(on_toolButtonSelectSection()));        
    }

    ui->pushButtonYandexPhotos->hide();
}

void FormLibImages::initPermissions()
{
    isAddPermission = UserPermission::isAdd(UserPermission::IMAGES);
    isDeletePermission = UserPermission::isDelete(UserPermission::IMAGES);

    ui->toolButtonAdd->setEnabled(isAddPermission);
    ui->toolButtonRemoveUnused->setEnabled(isDeletePermission);
}

void FormLibImages::checkEnabled()
{    
    ui->toolButtonAdd->setEnabled(libListDataItems()->isCurrentRow());
    ui->toolButtonDelete->setEnabled(libListDataItems()->isCurrentRow());
}

void FormLibImages::on_toolButtonAdd_clicked()
{
    if (!isAddPermission)
        return;

    QString pictureFolder;
    QSettings settings;
    pictureFolder = settings.value("pictureFolder").toString();

    QStringList files =
            QFileDialog::getOpenFileNames(this, tr("Открыть изображения"),
                                          pictureFolder, tr("Изображения") + "(*.png *.xpm *.jpg)");
    if (files.size()) {
        pictureFolder = QFileInfo(files.at(0)).absolutePath();
        libListDataItems()->addFiles(files);
    }    

    if (!pictureFolder.isEmpty()) {
        settings.setValue("pictureFolder", pictureFolder);
        settings.sync();
    }

}

void FormLibImages::on_toolButtonDelete_clicked()
{
    if (!isDeletePermission)
        return;

    libListDataItems()->deleteFiles();
}

void FormLibImages::on_toolButtonSelect_clicked()
{
    if (libListDataItems()->isCurrentRow())
        accept();
}

FrameLibListDataItems *FormLibImages::libListDataItems()
{
    return qobject_cast<FrameLibListDataItems *>
             (ui->stackedWidget->currentWidget());
}

void FormLibImages::on_toolButtonRemoveUnused_clicked()
{
    libListDataItems()->deleteUnusedFiles();
}
