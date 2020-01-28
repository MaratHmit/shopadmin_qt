#ifndef FORMBRANDCARD_H
#define FORMBRANDCARD_H

#include <QDialog>

#include "dataitem.h"
#include "frameimage.h"
#include "framewysiwyg.h"

namespace Ui {
class FormBrandCard;
}

class FormBrandCard : public QDialog
{
    Q_OBJECT

public:
    explicit FormBrandCard(QWidget *parent = 0);
    ~FormBrandCard();

    void setId(const QString &id);
    void showEvent(QShowEvent *e);

signals:
    void saved(const QString &id);

private slots:
    void on_buttonCancel_clicked();
    void on_pushButtonMainInfo_clicked();
    void on_pushButtonImage_clicked();
    void on_pushButtonDocInfo_clicked();
    void on_buttonSave_clicked();    
    void on_pushButtonTranslit_clicked();
    void on_buttonOK_clicked();
    void onModified();

    void on_pushButtonDescription_clicked();

private:
    Ui::FormBrandCard *ui;
    DataItem *brand;
    FrameWYSIWYG *widgetFullDescription;

    void initRegValidators();

    void initSignals();
    void setData();
    void getData();
    void fillControls();
    bool save();

    FrameImage *widgetImage();
};

#endif // FORMBRANDCARD_H
