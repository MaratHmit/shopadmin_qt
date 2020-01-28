#include "framecardoptions.h"
#include "ui_framecardoptions.h"

#include "formoptionsparams.h"
#include "formproducts.h"

FrameCardOptions::FrameCardOptions(QWidget *parent, DataProduct *item, const int &typeDialog) :
    QDialog(parent),
    ui(new Ui::FrameCardOptions)
{
    ui->setupUi(this);

    product = item;
    mTypeDialog = typeDialog;
    mOptions = item->listOptions();
    mListPages = new QList<WidgetPageOptions *>();
    mIsFilled = false;
    ui->widgetOrder->hide();

    if (typeDialog) {
        setWindowFlags(Qt::Window);
        connect(ui->buttonOK, &QPushButton::clicked, this, &FrameCardOptions::onOk);
        connect(ui->buttonCancel, &QPushButton::clicked, this, &QDialog::reject);
        if (typeDialog == 2) {
            ui->widgetButtons->hide();
            ui->tabWidget->setTabsClosable(false);
            ui->widgetOrder->show();
            ui->lineEditName->setText(item->getName());
            ui->lineEditPrice->setText(item->priceTitle());
        }
    }
    else {
        setWindowFlags(Qt::Widget);
        ui->buttonOK->hide();
        ui->buttonCancel->hide();
    }
    initSignals();
}

FrameCardOptions::~FrameCardOptions()
{
    delete ui;
    delete mListPages;
}

bool FrameCardOptions::isAddOptions() const
{
    return false;
}

void FrameCardOptions::fillControls()
{
    if (mIsFilled)
        return;

    initOptions();
    mOptions->setUnModified();
    mIsFilled = true;
}

void FrameCardOptions::onAddOptions()
{
    FormOptionsParams formParams(this, true);

    if (mOptions->size()) {
        ListOptions list;
        for (int i = 0; i < mOptions->size(); i++)
            list.append(mOptions->at(i));
        formParams.setExludingOptions(&list);
    }


    if (formParams.exec() == QDialog::Accepted) {
        for (int i = 0; i < formParams.selectedList()->size(); i++)
            addPage(formParams.selectedList()->at(i), true);        
    }
}

void FrameCardOptions::onCopyFromOtherProduct()
{
    FormProducts *form = new FormProducts(this, false);
    if (form->exec() == QDialog::Accepted && form->getSelectedProducts()->size()) {
        DataProduct *cProduct = new DataProduct(form->getSelectedProducts()->at(0));
        setCursor(Qt::WaitCursor);
        if (ApiAdapter::getApi()->getInfoProduct(cProduct)) {
            for (int i = 0; i < cProduct->listOptions()->size(); ++i)
                product->listOptions()->append(new DataOption(cProduct->listOptions()->at(i)), false);
            mIsFilled = false;
            fillControls();
            product->listOptions()->setModified();
            emit modified();
        }
        delete cProduct;
        setCursor(Qt::ArrowCursor);
    }
    delete form;
}

void FrameCardOptions::onRemoveOption(const int &index)
{
    WidgetPageOptions *page = 0;
    for (int i = 0; i < mListPages->size(); i++)
        if (ui->tabWidget->indexOf(mListPages->at(i)) == index) {
            page = mListPages->takeAt(i);
            break;
        }

    if (page) {
        ui->tabWidget->removeTab(index);
        mOptions->deleteItem(page->optionInfo());
    }

    emit modified();
}

void FrameCardOptions::onChecked()
{
    QString originalName = product->getName();
    QString nameOptions = QString();
    qreal basePrice = product->price();
    qreal price = 0;
    for (int i = 0; i < product->listOptions()->size(); i++) {
        DataOption *option = product->listOptions()->at(i);
        int type = option->property("type").toInt();
        int typePrice = option->property("typePrice").toInt();
        QString optionValue = QString();
        for (int j = 0; j < option->optionValues()->size(); j++) {
            if (option->optionValues()->at(j)->getIsChecked()) {
                if (!optionValue.isEmpty())
                    optionValue = optionValue + ", ";
                optionValue = optionValue + option->optionValues()->at(j)->getName();
                int count = 1;
                if (option->optionValues()->at(j)->count() > 1)
                    count = option->optionValues()->at(j)->count();
                if (typePrice == 0)
                    price += option->optionValues()->at(j)->price() * count;
                else price += basePrice * option->optionValues()->at(j)->price() / 100;
                if (type != 2)
                    break;
            }
        }
        if (!optionValue.isEmpty()) {
            if (!nameOptions.isEmpty())
                nameOptions = nameOptions + "; ";
            nameOptions = nameOptions + option->getName() + ": " + optionValue;
        }
    }
    if (!nameOptions.isEmpty()) {
        ui->lineEditName->setText(originalName + " (" + nameOptions + ")");
        price += basePrice;
        ui->lineEditPrice->setText(QString::number(price, 'f', 2));
    }
    else {
        ui->lineEditName->setText(originalName);
        ui->lineEditPrice->setText(QString::number(basePrice, 'f', 2));
    }
}

void FrameCardOptions::onOk()
{
    product->setName(ui->lineEditName->text());
    product->setPrice(ui->lineEditPrice->text().toDouble());
    accept();
}

void FrameCardOptions::addPage(DataOption *option, const bool &isNew)
{

    if (WidgetPageOptions *page = getPageById(option->getId())) {        
        ui->tabWidget->addTab(page, option->getName());
        if (!option->property("position").isNull())
            page->setPosition(option->property("position").toInt());
        for (int i = 0; i < mOptions->size(); i++)
            if (mOptions->at(i)->getId() == option->getId()) {
                mOptions->at(i)->setIsChecked(true);
                break;
            }
        return;
    }

    DataOption *newOption;
    if (isNew) {
        newOption = new DataOption(option);
        mOptions->append(newOption);
    } else newOption = option;

    WidgetPageOptions *page = new WidgetPageOptions(this, newOption, mTypeDialog == 2);
    if (!newOption->property("position").isNull())
        page->setPosition(newOption->property("position").toInt());

    connect(page, SIGNAL(modified()), SIGNAL(modified()));
    if (mTypeDialog == 2)
        connect(page, SIGNAL(checked()), SLOT(onChecked()));

    ui->tabWidget->addTab(page, newOption->getName());
    mListPages->push_back(page);
}

WidgetPageOptions *FrameCardOptions::getPageById(const QString &id) const
{
    for (int i = 0; i < mListPages->size(); i++) {
        if (mListPages->at(i)->optionInfo()->getId() == id)
            return mListPages->at(i);
    }
    return 0;
}

void FrameCardOptions::initOptions()
{
    if (!mOptions)
        return;

    for (int i = 0; i < mOptions->size(); i++) {
        mOptions->at(i)->setIsChecked(true);
        mOptions->at(i)->setUnModified();
        addPage(mOptions->at(i));
    }
}

void FrameCardOptions::initSignals()
{
    connect(ui->toolButtonAddOption, SIGNAL(clicked()), SLOT(onAddOptions()));
    connect(ui->tabWidget, &QTabWidget::tabCloseRequested, this, &FrameCardOptions::onRemoveOption);
    connect(ui->toolButtonCopy, SIGNAL(clicked(bool)), SLOT(onCopyFromOtherProduct()));
}
