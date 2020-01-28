#include "framecardcrossgroups.h"
#include "ui_framecardcrossgroups.h"
#include "formproductsgroups.h"

FrameCardCrossGroups::FrameCardCrossGroups(
        QWidget *parent, ListDataItems *listGroups,
        const bool &isDialogMode) :
    QDialog(parent),
    ui(new Ui::FrameCardCrossGroups)
{
    ui->setupUi(this);
    mListGroups = listGroups;
    mIsDialogMode = isDialogMode;
    if (isDialogMode) {
        setWindowFlags(Qt::Window);
        connect(ui->buttonOK, &QPushButton::clicked, this,
                &QDialog::accept);
        connect(ui->buttonCancel, &QPushButton::clicked, this,
                &QDialog::reject);
    }
    else {
        setWindowFlags(Qt::Widget);
        ui->buttonOK->hide();
        ui->buttonCancel->hide();
    }
    mModel = new NameTableModel(this, mListGroups);
    ui->listViewSimilarProducts->setModel(mModel);
    ui->listViewSimilarProducts->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->listViewSimilarProducts->setSelectionMode(QAbstractItemView::ExtendedSelection);
    initSignals();
}

FrameCardCrossGroups::~FrameCardCrossGroups()
{
    delete ui;
}

void FrameCardCrossGroups::fillControls()
{
    mModel->resetModel();
}

void FrameCardCrossGroups::onAddGroups()
{

    FormProductsGroups *form = new FormProductsGroups(this);    
    form->setSelectMode(true);
    if (form->exec() == QDialog::Accepted) {
        mModel->beginReset();
        QListIterator<DataProductsGroup *> it(form->listSelected());
        while (it.hasNext()) {
            DataProductsGroup *gr = (it.next());
            if (!mListGroups->existById(gr))
                mListGroups->append(new DataItem(gr));
        }
        mModel->endReset();
        mListGroups->setModified();
        emit modified();
    }
    delete form;
}

void FrameCardCrossGroups::onDeleteGroups()
{
    if (!ui->listViewSimilarProducts->
            currentIndex().isValid())
        return;

    mModel->beginReset();
    QModelIndexList list = ui->listViewSimilarProducts->
            selectionModel()->selectedRows();
    for (int i = 0; i < list.size(); i++)
        mListGroups->at(list.at(i).row())->setIsSelected(true);
    for (int i = mListGroups->size() - 1; i >= 0; i--)
        if (mListGroups->at(i)->getIsSelected())
            delete mListGroups->takeAt(i);
    mModel->endReset();
    mListGroups->setModified();
    emit modified();
    onChange();
}

void FrameCardCrossGroups::onChange()
{
    ui->toolButtonDelete->setEnabled(ui->listViewSimilarProducts->
                                  currentIndex().isValid());
}

void FrameCardCrossGroups::initSignals()
{
    connect(ui->toolButtonAdd, SIGNAL(clicked()), SLOT(onAddGroups()));
    connect(ui->toolButtonDelete, SIGNAL(clicked()), SLOT(onDeleteGroups()));
    connect(ui->listViewSimilarProducts->selectionModel(),
            SIGNAL(currentChanged(QModelIndex,QModelIndex)),
            SLOT(onChange()));
}
