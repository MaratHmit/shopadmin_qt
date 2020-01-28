#include <QDebug>

#include "formproductcard.h"
#include "formcommentcard.h"
#include "ui_formproductcard.h"

#include "apiadapter.h"
#include "seconfig.h"

FormProductCard::FormProductCard(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FormProductCard)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Window);    

    mFrameMainInfo = 0;
    mFrameMoreInfo = 0;
    mFrameListImages = 0;
    mFrameSpecifications = 0;
    mFrameModifications = 0;
    mFrameSimilar = 0;
    mFrameAccompanying = 0;
    mFrameComments = 0;
    mFrameCrossGroups = 0;
    mFrameDiscounts = 0;
    mFrameSEO = 0;
    mFrameFiles = 0;
    mFrameOptions = 0;
    mIsCloneMode = false;
    mIsMultiMode = false;

    ui->pushButtonFeedback->hide();
    ui->pushButtonLicenses->hide();    
    ui->pushButtonOptions->setVisible(SEConfig::getMainInfo()->isShowOptions());

    mProduct = new DataProduct;
    mProductSaved = new DataProduct;
    initSignals();
}

FormProductCard::~FormProductCard()
{
    SEConfig::saveStateGeometry(this);    
    delete ui;
    mProduct->deleteLater();
}

void FormProductCard::setId(const QString &id)
{
    mProduct->setId(id);
}

void FormProductCard::setIds(const QStringList &ids)
{
    foreach (QString id, ids)
        mProduct->addId(id);
    mIsMultiMode = true;
    if (mIsMultiMode)
        ui->buttonSave->hide();
}

void FormProductCard::setGroup(const DataProductsGroup *group)
{
    mProduct->setIdGroup(group->getId());
    mProduct->setNameGroup(group->getName());
    mProduct->setIdModificationGroupDef(group->idModificationGroupDef());
}

void FormProductCard::setFilterForSave(const QString filter)
{
    mProduct->setProperty("filter", filter);
}

void FormProductCard::setCloneMode()
{
    mIsCloneMode = true;
}

const DataProduct *FormProductCard::product() const
{
    return mProduct;
}

void FormProductCard::showEvent(QShowEvent *)
{
    SEConfig::restoreStateGeometry(this);
    getData();
    ui->pushButtonMainInfo->click();
}

void FormProductCard::onMainInfoClicked()
{
    if (!mFrameMainInfo) {
        mFrameMainInfo = new FrameCardMainInfo(this);
        mFrameMainInfo->setProduct(mProduct);        
        ui->stackedWidget->addWidget(mFrameMainInfo);
        connect(mFrameMainInfo, SIGNAL(modified()), SLOT(onModified()));
    }
    bool f = ui->buttonSave->isEnabled();
    mFrameMainInfo->setMultiMode(mIsMultiMode);
    ui->stackedWidget->setCurrentWidget(mFrameMainInfo);        
    mFrameMainInfo->fillControls();
    ui->buttonSave->setEnabled(f);
}

void FormProductCard::onMoreInfoClicked()
{
    if (!mFrameMoreInfo) {
        setCursor(Qt::WaitCursor);
        mFrameMoreInfo = new FrameCardMoreInfo(this, mProduct);
        ui->stackedWidget->addWidget(mFrameMoreInfo);
        connect(mFrameMoreInfo, SIGNAL(modified()), SLOT(onModified()));
    }
    bool f = ui->buttonSave->isEnabled();
    ui->stackedWidget->setCurrentWidget(mFrameMoreInfo);
    mFrameMoreInfo->fillControls();
    ui->buttonSave->setEnabled(f);    
    setCursor(Qt::ArrowCursor);
}

void FormProductCard::onListDataImagesClicked()
{
    if (!mFrameListImages) {
        mFrameListImages = new FrameListImages(this, SEConsts::Section_Products,
                                                  mProduct->listImages());
        ui->stackedWidget->addWidget(mFrameListImages);
        if (!mProduct->getName().isEmpty())
            mFrameListImages->setDefaultAlt(mProduct->getName());
        connect(mFrameListImages, SIGNAL(modified()), SLOT(onModified()));
    }
    bool f = ui->buttonSave->isEnabled();
    ui->stackedWidget->setCurrentWidget(mFrameListImages);
    mFrameListImages->fillControls();
    ui->buttonSave->setEnabled(f);
}

void FormProductCard::onSpecificationsClicked()
{
    if (!mFrameSpecifications) {
        mFrameSpecifications = new
                FrameCardSpecifications(this, mProduct);
        connect(mFrameSpecifications, SIGNAL(modified()), SLOT(onModified()));
        ui->stackedWidget->addWidget(mFrameSpecifications);
    }
    bool f = ui->buttonSave->isEnabled();
    ui->stackedWidget->setCurrentWidget(mFrameSpecifications);
    mFrameSpecifications->fillControls();
    ui->buttonSave->setEnabled(f);
}

void FormProductCard::onModificationsClicked()
{
    if (!mFrameModifications) {
        mFrameModifications = new
                FrameCardModifications(this, mProduct->listModificationsGroups(),
                                       mProduct->listImages());        
        connect(mFrameModifications, SIGNAL(modified()), SLOT(onModified()));
        ui->stackedWidget->addWidget(mFrameModifications);
    }
    bool f = ui->buttonSave->isEnabled();
    ui->stackedWidget->setCurrentWidget(mFrameModifications);    
    mFrameModifications->setIdModificationGroupDef(mProduct->idModificationGroupDef());
    mFrameModifications->fillControls();
    ui->buttonSave->setEnabled(f);
}

void FormProductCard::onSimilarClicked()
{
    if (!mFrameSimilar) {
        mFrameSimilar = new
                FrameCardSimilar(this, mProduct->listSimilarProducts());
        connect(mFrameSimilar, SIGNAL(modified()), SLOT(onModified()));
        ui->stackedWidget->addWidget(mFrameSimilar);
    }
    mFrameSimilar->setIdProduct(mProduct->getId());
    ui->stackedWidget->setCurrentWidget(mFrameSimilar);
    mFrameSimilar->fillControls();
}

void FormProductCard::onAccompanyingClicked()
{
    if (!mFrameAccompanying) {
        mFrameAccompanying = new
                FrameCardSimilar(this, mProduct->listAccompProducts());
        connect(mFrameAccompanying, SIGNAL(modified()), SLOT(onModified()));
        ui->stackedWidget->addWidget(mFrameAccompanying);
    }
    mFrameAccompanying->setIdProduct(mProduct->getId());
    ui->stackedWidget->setCurrentWidget(mFrameAccompanying);
    mFrameAccompanying->fillControls();
}

void FormProductCard::onDiscountsClicked()
{
    if (!mFrameDiscounts) {
        mFrameDiscounts = new
                FrameCardDiscounts(this, mProduct);
        connect(mFrameDiscounts, SIGNAL(modified()), SLOT(onModified()));
        ui->stackedWidget->addWidget(mFrameDiscounts);
    }
    mFrameDiscounts->setIdProduct(mProduct->getId());
    ui->stackedWidget->setCurrentWidget(mFrameDiscounts);
    mFrameDiscounts->fillControls();
}

void FormProductCard::onCommentsClicked()
{
    if (!mFrameComments) {
        mFrameComments = new FormCatalog(this, mProduct->listComments());
        mFrameComments->setVisibleSearch(false);
        mFrameComments->setRootIsDecorated(false);
        mFrameComments->setSortingEnabled(false);
        mFrameComments->setWindowTitle("Комментарии к товарам");
        mFrameComments->setTitleButtonAdd("Добавить комментарий", "Добавить новый комментарий");
        mFrameComments->addField(new Field("date", "Дата"));
        mFrameComments->addField(new Field("contactTitle", "Имя пользователя"));
        mFrameComments->addField(new Field("commentary", "Комментарий"));
        mFrameComments->addField(new Field("response", "Ответ администратора"));
        FormCommentCard *cardComment = new FormCommentCard(mFrameComments);
        cardComment->setLocalMode();
        mFrameComments->setDialogCard(cardComment);

        connect(mFrameComments, SIGNAL(modified()), SLOT(onModified()));
        ui->stackedWidget->addWidget(mFrameComments);
    }
    ui->stackedWidget->setCurrentWidget(mFrameComments);
}

void FormProductCard::onCrossGroupsClicked()
{
    if (!mFrameCrossGroups) {
        mFrameCrossGroups = new FrameCardCrossGroups(this, mProduct->listCrossGroups());
        connect(mFrameCrossGroups, &FrameCardCrossGroups::modified, this,
                &FormProductCard::onModified);
        ui->stackedWidget->addWidget(mFrameCrossGroups);

    }
    ui->stackedWidget->setCurrentWidget(mFrameCrossGroups);
    mFrameCrossGroups->fillControls();
}

void FormProductCard::onSEOClicked()
{
    if (!mFrameSEO) {
        mFrameSEO = new FrameSEO(this, mProduct);
        connect(mFrameSEO, &FrameSEO::modified, this, &FormProductCard::onModified);
        ui->stackedWidget->addWidget(mFrameSEO);
    }
    ui->stackedWidget->setCurrentWidget(mFrameSEO);
    mFrameSEO->fillControls();
}

void FormProductCard::onFilesClicked()
{
    if (!mFrameFiles) {
        mFrameFiles = new FrameCardFiles(this, mProduct);
        connect(mFrameFiles, &FrameCardFiles::modified, this, &FormProductCard::onModified);
        ui->stackedWidget->addWidget(mFrameFiles);
    }
    ui->stackedWidget->setCurrentWidget(mFrameFiles);
    mFrameFiles->fillControls();
}

void FormProductCard::onOptionsClicked()
{
    if (!mFrameOptions) {
        mFrameOptions = new FrameCardOptions(this, mProduct);
        connect(mFrameOptions, &FrameCardOptions::modified, this, &FormProductCard::onModified);
        ui->stackedWidget->addWidget(mFrameOptions);
    }
    ui->stackedWidget->setCurrentWidget(mFrameOptions);
    mFrameOptions->fillControls();
}

void FormProductCard::onButtonMenuClicked()
{
    QPushButton *button = static_cast <QPushButton *> (sender());
    if (button)
        ui->stackedWidget->setCurrentIndex(button->autoRepeatInterval());
}

void FormProductCard::onModified()
{    
    ui->buttonSave->setEnabled(true);
}

void FormProductCard::onSave()
{    
    if (save()) {
        if (!mIsMultiMode)
            mFrameMainInfo->refreshData();        
        ui->buttonSave->setEnabled(false);    
    }
}

void FormProductCard::onOK()
{    
    if (save())
        accept();
}

void FormProductCard::initSignals()
{
    connect(ui->buttonCancel, SIGNAL(clicked()), SLOT(close()));
    connect(ui->pushButtonMainInfo, SIGNAL(clicked()), SLOT(onMainInfoClicked()));
    connect(ui->pushButtonMore, SIGNAL(clicked()), SLOT(onMoreInfoClicked()));
    connect(ui->pushButtonPictures, SIGNAL(clicked()), SLOT(onListDataImagesClicked()));
    connect(ui->pushButtonSpecifications, SIGNAL(clicked()), SLOT(onSpecificationsClicked()));
    connect(ui->pushButtonModifications, SIGNAL(clicked()), SLOT(onModificationsClicked()));
    connect(ui->pushButtonAccompGoods, SIGNAL(clicked()), SLOT(onAccompanyingClicked()));
    connect(ui->pushButtonDiscounts, SIGNAL(clicked()), SLOT(onDiscountsClicked()));
    connect(ui->pushButtonSameGoods, SIGNAL(clicked()), SLOT(onSimilarClicked()));
    connect(ui->pushButtonComment, SIGNAL(clicked()), SLOT(onCommentsClicked()));
    connect(ui->pushButtonCrossGroups, SIGNAL(clicked()), SLOT(onCrossGroupsClicked()));
    connect(ui->pushButtonSEO, SIGNAL(clicked()), SLOT(onSEOClicked()));
    connect(ui->pushButtonFiles, SIGNAL(clicked()), SLOT(onFilesClicked()));
    connect(ui->pushButtonOptions, SIGNAL(clicked()), SLOT(onOptionsClicked()));
    connect(ui->buttonSave, SIGNAL(clicked()), SLOT(onSave()));
    connect(ui->buttonOK, SIGNAL(clicked()), SLOT(onOK()));
    connect(mProduct, SIGNAL(modified()), SLOT(onModified()));
}

void FormProductCard::getData()
{
    if (mProduct->getId().isEmpty() || mProduct->getIds().size() > 1)
        return;

    setEnabled(false);
    ApiAdapter::getApi()->getInfoProduct(mProduct); 
    if (mIsCloneMode) {
        mProduct->setCode(QString());    
        mProduct->setArticle(QString());
        for (int i = 0; i < mProduct->listModificationsGroups()->size(); i++) {
            for (int j = 0; j < mProduct->listModificationsGroups()->at(i)->modifications()->size(); j++)
                mProduct->listModificationsGroups()->at(i)->modifications()->at(j)->setArticle(QString());
        }
    }
    setEnabled(true);
}


bool FormProductCard::save()
{
    if (!mIsMultiMode && mProduct->getName().trimmed().isEmpty()) {
        QMessageBox::warning(this, tr("Сохранение товара"), tr("Не указано наименование товара!"));
        ui->pushButtonMainInfo->click();
        return false;
    }

    if (mIsCloneMode)
        mProduct->setIsSetAllFields(true);
    mProduct->setCloneMode(mIsCloneMode);

    mFrameMainInfo->correctDataBeforeSave();
    if (mFrameMoreInfo)
        mFrameMoreInfo->correctDataBeforeSave();
    if (mFrameSpecifications)
        mFrameSpecifications->correctDataBeforeSave();

    if (!mProduct->isModified())
        return true;   

    bool isNew = mProduct->getId().isEmpty();
    if (isNew)
        mProduct->setIsSetAllFields(true);

    setCursor(Qt::WaitCursor);    
    ui->buttonSave->setEnabled(false);
    ui->buttonOK->setEnabled(false);
    ui->buttonCancel->setEnabled(false);            
    bool result = ApiAdapter::getApi()->saveProduct(mProduct);
    DataProduct product(mProduct);
    if (result) {        
        if (ApiAdapter::getApi()->getInfoProduct(&product)) {
            mProduct->setArticle(product.getArticle());
            mProduct->setCode(product.getCode());
            mProduct->setCountModifications(product.getCountModifications());
            mProduct->setCurrency(product.getCurrency());
        }
        mProduct->setUnModified();
    }
    ui->buttonOK->setEnabled(true);
    ui->buttonSave->setEnabled(!result);
    ui->buttonCancel->setEnabled(true);
    setCursor(Qt::ArrowCursor);
    if (result) {
        emit saved(&product, isNew || mIsCloneMode);
        mIsCloneMode = false;
    }

    return result;
}
