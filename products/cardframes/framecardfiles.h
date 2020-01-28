#ifndef FRAMECARDFILES_H
#define FRAMECARDFILES_H

#include <QWidget>

#include "dataproduct.h"
#include "nametablemodel.h"

namespace Ui {
class FrameCardFiles;
}

class FrameCardFiles : public QWidget
{
    Q_OBJECT

public:
    FrameCardFiles(QWidget *parent, DataProduct *item);
    ~FrameCardFiles();

    void fillControls();

signals:
    void modified();

private slots:
    void onAddFile();
    void onDeleteFile();
    void onChange();

private:
    Ui::FrameCardFiles *ui;
    DataProduct *product;
    NameTableModel *model;

    void initVariables();
    void initSignals();

    void uploadFile(const QString fileName);

};

#endif // FRAMECARDFILES_H
