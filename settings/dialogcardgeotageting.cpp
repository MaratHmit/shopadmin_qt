#include "dialogcardgeotageting.h"
#include "ui_dialogcardgeotageting.h"

#include "apiadapter.h"
#include "field.h"

#include <QMessageBox>

DialogCardGeotageting::DialogCardGeotageting(QWidget *parent) :
    DialogCard(parent),
    ui(new Ui::DialogCardGeotageting)
{
    ui->setupUi(this);
    initVariables();
    initSignals();
}

DialogCardGeotageting::~DialogCardGeotageting()
{
    delete ui;
}

void DialogCardGeotageting::initSignals()
{
    connect(ui->toolButtonGeo, SIGNAL(clicked(bool)), SLOT(selectGeoInfo()));
}

void DialogCardGeotageting::initVariables()
{
    seoVariables = new ListDataItems;
    modelSeoVariables = new NameTableModel(this, seoVariables);
    modelSeoVariables->appendField(new Field("value", "Значение"));
    modelSeoVariables->setEditableMode(true);
}

void DialogCardGeotageting::initControls()
{
    ui->lineEditName->clear();
    ui->lineEditAddress->clear();
    ui->lineEditPhone->clear();
    ui->lineEditAdditionalPhones->clear();
    ui->lineEditCity->clear();
    ui->lineEditDomain->clear();
    ui->plainTextEditDescription->clear();
    ui->spinBoxSortIndex->clear();
    ui->checkBoxIsActive->setChecked(true);
    ui->lineEditName->setFocus();
    ui->treeViewSeoVariables->setModel(modelSeoVariables);
}

void DialogCardGeotageting::fillControls()
{
    if (item->getId().isEmpty() || !ApiAdapter::getApi()->getInfoData(SEConsts::API_GEOTARGETING_CONTACTS, item))
        return;

    QList<QVariant> variables = item->property("variables").toList();
    seoVariables->deleteItems();

    for (int i = 0; i < variables.count(); i++) {
        DataItem *seoVariable = new DataItem;
        seoVariable->setIsValueEditableMode();
        QMap<QString, QVariant> map = variables.at(i).toMap();
        seoVariable->setId(map.value("id").toString());
        seoVariable->setIdValue(map.value("idVariable").toString());
        seoVariable->setName(map.value("name").toString());
        seoVariable->setValue(map.value("value").toString());
        seoVariables->append(seoVariable, false);
    }
    modelSeoVariables->resetModel();

    ui->lineEditName->setText(item->getName());
    ui->lineEditAddress->setText(item->property("address").toString());
    ui->lineEditPhone->setText(item->property("phone").toString());
    ui->lineEditAdditionalPhones->setText(item->property("additionalPhones").toString());
    ui->lineEditCity->setText(item->property("city").toString());
    ui->lineEditDomain->setText(item->property("domain").toString());
    ui->plainTextEditDescription->setPlainText(item->property("description").toString());
    ui->spinBoxSortIndex->setValue(item->getSortIndex());
    ui->checkBoxIsActive->setChecked(item->getIsActive());
}

bool DialogCardGeotageting::setData()
{
    if (ui->lineEditName->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Сохранение контакта", "Не заполнено обязателное поле: <Наименование>!");
        ui->lineEditName->setFocus();
        return false;
    }
    if (ui->lineEditCity->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Сохранение контакта", "Не выбран город привязки!");
        ui->toolButtonGeo->click();
        return false;
    }    

    item->setName(ui->lineEditName->text().trimmed());
    item->setDescription(ui->plainTextEditDescription->toPlainText());
    item->setSortIndex(ui->spinBoxSortIndex->value());
    item->setIsActive(ui->checkBoxIsActive->isChecked());
    item->setProperty("domain", ui->lineEditDomain->text().trimmed());
    item->setProperty("address", ui->lineEditAddress->text().trimmed());    
    item->setProperty("phone", ui->lineEditPhone->text().trimmed());
    item->setProperty("additionalPhones", ui->lineEditAdditionalPhones->text().trimmed());    

    QList<QVariant> variables;
    for (int i = 0; i < seoVariables->size(); ++i) {
        QMap<QString, QVariant> map;
        if (!seoVariables->at(i)->getId().isEmpty())
            map["id"] = seoVariables->at(i)->getId();
        map["idVariable"] = seoVariables->at(i)->getIdValue();
        map["value"] = seoVariables->at(i)->getValue();
        variables.append(map);
    }
    item->setProperty("variables", variables);

    return true;
}

bool DialogCardGeotageting::save()
{
    return ApiAdapter::getApi()->save(SEConsts::API_GEOTARGETING_CONTACTS, item);
}

void DialogCardGeotageting::selectGeoInfo()
{
    DataDelivery *region = new DataDelivery;
    DialogDelivRegion dialog(this, region);
    dialog.setGeoMode();
    if (dialog.exec() == QDialog::Accepted)  {
        if (!region->idCountryTo().isEmpty()) {
            item->setProperty("idCity", region->idCityTo());
            ui->lineEditCity->setText(region->countryTo() + ", " + region->cityTo());
        }
    }
    delete region;
}
