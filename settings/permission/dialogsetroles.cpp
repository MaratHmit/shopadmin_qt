#include "dialogsetroles.h"
#include "ui_dialogsetroles.h"
#include "apiadapter.h"
#include "field.h"

DialogSetRoles::DialogSetRoles(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogSetRoles)
{
    ui->setupUi(this);        
    model = new NameTableModel(this, new ListDataItems);
    model->setCheckedMode(true);    
    ui->treeView->setModel(model);
}

void DialogSetRoles::setModel(const NameTableModel &model, const QStringList &idsCheckedRoles)
{
    for (int i = 0; i < model.getItems()->size(); ++i) {
        DataItem *item = new DataItem(model.getItems()->at(i));
        this->model->addItem(item);
        item->setIsChecked(idsCheckedRoles.contains(model.getItems()->at(i)->getId()));
    }

}

DialogSetRoles::~DialogSetRoles()
{
    delete ui;
}

const QStringList DialogSetRoles::selectedIdsRoles() const
{
    QStringList result;

    for (int i = 0; i < model->getItems()->size(); ++i)
        if (model->getItems()->at(i)->getIsChecked())
            result.append(model->getItems()->at(i)->getId());

    return result;
}

void DialogSetRoles::showEvent(QShowEvent *)
{
    if (model->getItems()->size())
        ui->treeView->selectionModel()->setCurrentIndex(model->index(0, 0),
                                                        QItemSelectionModel::SelectCurrent |
                                                        QItemSelectionModel::Rows);
}

