#include "formmeasures.h"
#include "ui_formmeasures.h"

#include "userpermission.h"

#include <QVBoxLayout>

FormMeasures::FormMeasures(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormMeasures)
{
    ui->setupUi(this);

    mFormMeasuresVolume = 0;
    mFormMeasuresWeight = 0;

    initCatalogs();
    initSignals();
}

FormMeasures::~FormMeasures()
{
    delete ui;
}

void FormMeasures::initSignals()
{

}

void FormMeasures::initCatalogs()
{
    QVBoxLayout *layout = new QVBoxLayout;
    mFormMeasuresWeight = new FormCatalog(this, SEConsts::API_MEASURES_WEIGHT,
                                        UserPermission::SETTINGS);
    mFormMeasuresWeight->setRootIsDecorated(false);
    mFormMeasuresWeight->setWindowTitle("Мера веса");
    mFormMeasuresWeight->setTitleButtonAdd("Добавить", "Создать новую меру веса");
    mFormMeasuresWeight->addField(new Field("id", "Ид.", Qt::AlignRight | Qt::AlignVCenter));
    mFormMeasuresWeight->addField(new Field("code", "Код ОКЕИ"));
    mFormMeasuresWeight->addField(new Field("name", "Наименование"));
    mFormMeasuresWeight->addField(new Field("designation", "Обозначение"));
    mFormMeasuresWeight->addField(new Field("precision", "Точность"));
    mFormMeasuresWeight->addField(new Field("resultValue", "Мера"));
    mFormMeasuresWeight->setParent(ui->tabWeightMeasures);
    mFormMeasuresWeight->setRefreshModeAfterChange();
    layout->addWidget(mFormMeasuresWeight);
    layout->setMargin(0);
    ui->tabWeightMeasures->setLayout(layout);

    DialogCardMeasure *dialogCard = new DialogCardMeasure(mFormMeasuresWeight);
    mFormMeasuresWeight->setDialogCard(dialogCard);

    layout = new QVBoxLayout;
    mFormMeasuresVolume = new FormCatalog(this, SEConsts::API_MEASURES_VOLUME,
                                        UserPermission::SETTINGS);
    mFormMeasuresVolume->setRootIsDecorated(false);
    mFormMeasuresVolume->setRefreshModeAfterChange();
    mFormMeasuresVolume->setWindowTitle("Меры объема");
    mFormMeasuresVolume->setTitleButtonAdd("Добавить", "Создать новую меру объема");
    mFormMeasuresVolume->addField(new Field("id", "Ид.", Qt::AlignRight | Qt::AlignVCenter));
    mFormMeasuresVolume->addField(new Field("code", "Код ОКЕИ"));
    mFormMeasuresVolume->addField(new Field("name", "Наименование"));
    mFormMeasuresVolume->addField(new Field("designation", "Обозначение"));
    mFormMeasuresVolume->addField(new Field("precision", "Точность"));
    mFormMeasuresVolume->addField(new Field("resultValue", "Мера"));
    mFormMeasuresVolume->setParent(ui->tabVolumeMeasures);
    layout->addWidget(mFormMeasuresVolume);
    layout->setMargin(0);
    ui->tabVolumeMeasures->setLayout(layout);
    dialogCard = new DialogCardMeasure(mFormMeasuresVolume);
    mFormMeasuresVolume->setDialogCard(dialogCard);
    dialogCard->setVolumeMode();

}
