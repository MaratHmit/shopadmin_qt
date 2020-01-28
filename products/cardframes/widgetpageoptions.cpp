#include "widgetpageoptions.h"
#include "ui_widgetpageoptions.h"

WidgetPageOptions::WidgetPageOptions(QWidget *parent, DataOption *option, bool isSelectMode) :
    QWidget(parent),
    ui(new Ui::WidgetPageOptions)
{
    ui->setupUi(this);

    mOption = option;
    mFormOptions = 0;
    mIsSelectMode = isSelectMode;
    if (isSelectMode)
        ui->widgetButtons->hide();

    initTreeView();
}

WidgetPageOptions::~WidgetPageOptions()
{
    delete ui;
    if (mFormOptions)
        delete mFormOptions;
}

DataOption *WidgetPageOptions::optionInfo() const
{
    return mOption;
}

QString WidgetPageOptions::currentId() const
{
    QModelIndex index = ui->tableView->currentIndex();
    if (index.isValid())
        return mModel->idData(index);
    else return 0;
}

void WidgetPageOptions::setPosition(int pos)
{
   ui->comboBox->setCurrentIndex(pos);
   connect(ui->comboBox, SIGNAL(currentIndexChanged(int)), SLOT(changePosition(int)));
}

void WidgetPageOptions::onUpDown()
{
    QToolButton *button = qobject_cast<QToolButton *> (sender());
    bool isMoveUp = button == ui->toolButtonUp;

    QString id = currentId();
    mModel->beginReset();
    if (isMoveUp)
        mOption->optionValues()->moveUp(mModel->dataItem((ui->tableView->selectionModel()->currentIndex())));
    else mOption->optionValues()->moveDown(mModel->dataItem((ui->tableView->selectionModel()->currentIndex())));
    mModel->endReset();
    mOption->optionValues()->setModified();
    emit modified();

    goToRowById(id);
}

void WidgetPageOptions::on_toolButtonSelectOptionValues_clicked()
{
    if (!mFormOptions) {
        mFormOptions = new FormOptions(this, true);
        mFormOptions->setIdOption(mOption->getId());
    }

    if (mFormOptions->exec() == QDialog::Accepted) {
        mModel->beginReset();
        const ListOptionValues *values = mFormOptions->selectedList();
        for (int i = 0; i < values->size(); i++) {
            bool isExist = false;
            for (int j = 0; j < mOption->optionValues()->size(); j++) {
                if (mOption->optionValues()->at(j)->getId() == values->at(i)->getId()) {
                    isExist = true;
                    break;
                }
            }
            if (isExist)
                continue;

            DataOptionValue *value = new DataOptionValue;
            value->setId(values->at(i)->getId());
            value->setName(values->at(i)->getName());
            value->setPrice(values->at(i)->price());
            value->setDefault(false);
            mOption->optionValues()->append(value);
        }
        mModel->endReset();

        emit modified();
    }
}

void WidgetPageOptions::onChange()
{
    QString id = currentId();

    ui->toolButtonDelete->setEnabled(!id.isEmpty());
    ui->toolButtonUp->setEnabled(!id.isEmpty() && ui->tableView->selectionModel()->currentIndex().row());
    ui->toolButtonDown->setEnabled(!id.isEmpty() && mModel->rowCount() - 1 -
                                        ui->tableView->selectionModel()->currentIndex().row());
}

void WidgetPageOptions::initTreeView()
{
    mModel = new NameTableModel(this, reinterpret_cast <ListDataItems *> (mOption->optionValues()));
    if (mIsSelectMode)
        mModel->setCheckedMode(true);

    mModel->clearFields();
    mModel->appendField(new Field("name", "Наименование"));
    mModel->appendField(new Field("price", "Цена", Qt::AlignRight | Qt::AlignVCenter));
    if (!mIsSelectMode)
        mModel->appendField(new Field("isDefault", "Умолчан.", Qt::AlignRight | Qt::AlignVCenter));
    if (mOption->property("isCounted").toBool())
        mModel->appendField(new Field("count", "Кол-во", Qt::AlignRight | Qt::AlignVCenter));
    mModel->setEditableMode(true);

    ui->tableView->setModel(mModel);
    connect(mModel, SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)), SIGNAL(modified()));
    connect(ui->tableView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), SLOT(onChange()));
    connect(ui->toolButtonUp, SIGNAL(clicked(bool)), SLOT(onUpDown()));
    connect(ui->toolButtonDown, SIGNAL(clicked(bool)), SLOT(onUpDown()));
    connect(mModel, SIGNAL(checked(QString,bool)), SIGNAL(checked()));

    ui->tableView->setColumnWidth(0, 250);
    ui->tableView->setColumnWidth(1, 90);
    ui->tableView->setColumnWidth(2, 60);
}

void WidgetPageOptions::goToRowById(QString id)
{
    QModelIndex modelIndex = mModel->getModelIndexById(id);

    if (modelIndex.isValid()) {
        ui->tableView->selectionModel()->setCurrentIndex(
                    modelIndex, QItemSelectionModel::SelectCurrent | QItemSelectionModel::Rows);
        return;
    }

    if (mModel->rowCount() > 0)
        ui->tableView->selectionModel()->
                setCurrentIndex(mModel->index(0, 0), QItemSelectionModel::SelectCurrent |
                                QItemSelectionModel::Rows);
    else onChange();
    ui->tableView->setFocus();
}

void WidgetPageOptions::on_toolButtonDelete_clicked()
{
    QModelIndexList list = ui->tableView->selectionModel()->selectedIndexes();
    if (!list.size())
        return;

    for (int i = 0; i < list.size() ; i++)
        mOption->optionValues()->at(list.at(i).row())->setId("#");
    mModel->beginReset();
    for (int i = mOption->optionValues()->size() - 1; i >= 0; i--)
        if (mOption->optionValues()->at(i)->getId() == "#")
            delete mOption->optionValues()->takeAt(i);
    mModel->endReset();
    mOption->optionValues()->setModified();
    onChange();
    emit modified();
}

void WidgetPageOptions::changePosition(int pos)
{
    mOption->setProperty("position", pos);
    mOption->setModified("position");
    emit modified();
}
