#include "framecardfiles.h"
#include "ui_framecardfiles.h"

#include "field.h"
#include "filewebuploader.h"

#include <QFileDialog>
#include <QFileInfo>

FrameCardFiles::FrameCardFiles(QWidget *parent, DataProduct *item) :
    QWidget(parent),
    ui(new Ui::FrameCardFiles)
{
    ui->setupUi(this);
    product = item;
    initVariables();
    initSignals();
}

FrameCardFiles::~FrameCardFiles()
{
    delete ui;
}

void FrameCardFiles::fillControls()
{

}

void FrameCardFiles::onAddFile()
{
    QString filePath = QFileDialog::getOpenFileName(this, tr("Выбор файла"),
                                         "", tr("Документы") +
                                         "(*.pdf *.doc *.docx *.xls *.xlsx *.rar *.zip *.jpeg *.jpg *.png *.txt)");
    if (!filePath.isEmpty()) {
        QFileInfo fileInfo(filePath);
        DataItem *item = new DataItem();
        item->setProperty("file", fileInfo.fileName());
        item->setName(QString("Скачать " + fileInfo.baseName()));
        model->addItem(item);
        ui->treeViewFiles->resizeColumnToContents(0);
        ui->treeViewFiles->selectionModel()->
                setCurrentIndex(model->index(model->rowCount() - 1, 0),
                                QItemSelectionModel::SelectCurrent | QItemSelectionModel::Rows);
        emit modified();
        uploadFile(filePath);
    }
}

void FrameCardFiles::onDeleteFile()
{
    QModelIndexList list = ui->treeViewFiles->selectionModel()->selectedRows();
    if (!list.size())
        return;

    model->deleteItems(list);
    emit modified();
}

void FrameCardFiles::onChange()
{
    ui->toolButtonDelete->setEnabled(ui->treeViewFiles->selectionModel()->currentIndex().isValid());
}

void FrameCardFiles::initVariables()
{
    model = new NameTableModel(this, product->listFiles());
    model->setEditableMode(true);
    model->clearFields();
    model->appendField(new Field("file", "Файл"));
    model->appendField(new Field("name", "Текст ссылки"));
    ui->treeViewFiles->setModel(model);
    ui->treeViewFiles->resizeColumnToContents(0);
}

void FrameCardFiles::initSignals()
{
    connect(ui->toolButtonAdd, SIGNAL(clicked(bool)), SLOT(onAddFile()));
    connect(ui->toolButtonDelete, SIGNAL(clicked(bool)), SLOT(onDeleteFile()));
    connect(ui->treeViewFiles->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
            SLOT(onChange()));
}

void FrameCardFiles::uploadFile(const QString fileName)
{
    QThread* thread = new QThread;
    FileWebUploader *uploader = new FileWebUploader;
    uploader->moveToThread(thread);
    uploader->setImageMode(false);
    uploader->listFiles()->append(fileName);
    connect(thread, SIGNAL(started()), uploader, SLOT(process()));
    connect(uploader, SIGNAL(finished()), thread, SLOT(quit()));
    connect(uploader, SIGNAL(finished()), uploader, SLOT(deleteLater()));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    thread->start();
}
