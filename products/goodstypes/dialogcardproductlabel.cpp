#include "dialogcardproductlabel.h"
#include "ui_dialogcardproductlabel.h"
#include "apiadapter.h"

DialogCardProductLabel::DialogCardProductLabel(QWidget *parent) :
    DialogCard(parent),
    ui(new Ui::DialogCardProductLabel)
{
    ui->setupUi(this);    

    widgetImage()->setSection(SEConsts::Section_Labels);
    widgetImage()->hideImageAlt();

    initSignals();
}

DialogCardProductLabel::~DialogCardProductLabel()
{
    delete ui;
}

void DialogCardProductLabel::initSignals()
{
    connect(widgetImage(), SIGNAL(modified()), SLOT(onModified()));
    connect(ui->lineEditName, SIGNAL(textEdited(QString)), SLOT(onModified()));
    connect(ui->lineEditCode, SIGNAL(textEdited(QString)), SLOT(onModified()));
}
void DialogCardProductLabel::initControls()
{
    ui->lineEditName->clear();
    if (item->getId().isEmpty())
        setWindowTitle("Новый ярлык");
    else setWindowTitle(QString("Редактирование ярлыка: " + item->getName()));
    ui->lineEditName->setFocus();    
}

void DialogCardProductLabel::fillControls()
{
    if (item->getId().isEmpty() || !ApiAdapter::getApi()->getInfoData(SEConsts::API_PRODUCTS_LABELS, item))
        return;

    ui->lineEditName->setText(item->getName());
    ui->lineEditCode->setText(item->getCode());
    widgetImage()->setImage(item->getImageFile());

}

bool DialogCardProductLabel::setData()
{
    if (ui->lineEditName->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Сохранение ярлыка", "Не заполнено обязателное поле: <Наименование>!");
        ui->lineEditName->setFocus();
        return false;
    }

    item->setName(ui->lineEditName->text().trimmed());
    item->setCode(ui->lineEditCode->text());
    item->setImageFile(widgetImage()->fileImageName());

    return true;
}

bool DialogCardProductLabel::save()
{

    item->setIsSetAllFields(true);
    widgetImage()->saveImage();
    if (ApiAdapter::getApi()->save(SEConsts::API_PRODUCTS_LABELS, item)) {
        emit saved(item->getId());
        ui->buttonCancel->setEnabled(true);
        ui->buttonOK->setEnabled(true);
        return true;
    }

    return false;
}

void DialogCardProductLabel::onModified()
{
    ui->buttonSave->setEnabled(true);
}

FrameImage *DialogCardProductLabel::widgetImage()
{
    return qobject_cast<FrameImage *>
             (ui->widgetImage);
}

void DialogCardProductLabel::on_pushButtonTranslit_clicked()
{
    setCursor(Qt::WaitCursor);
    QStringList list;
    list.append(ui->lineEditName->text());
    list = ApiAdapter::getApi()->translit(list);
    if (list.size())
        ui->lineEditCode->setText(list.at(0));
    setCursor(Qt::ArrowCursor);

}
