#include "formsettingsinterface.h"
#include "ui_formsettingsinterface.h"
#include "dialogdynfielditem.h"
#include "dialogdynfieldsgroup.h"

FormSettingsInterface::FormSettingsInterface(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormSettingsInterface)
{
    ui->setupUi(this);
    ui->tabWidget->setCurrentIndex(0);
}

FormSettingsInterface::~FormSettingsInterface()
{
    delete ui;
}
