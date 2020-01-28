#ifndef DIALOGCARDPRODUCTLABEL_H
#define DIALOGCARDPRODUCTLABEL_H

#include <QDialog>

#include "dialogcard.h"
#include "nametablemodel.h"
#include "dataitem.h"
#include "frameimage.h"


namespace Ui {
class DialogCardProductLabel;
}

class DialogCardProductLabel : public DialogCard
{
    Q_OBJECT

public:
    explicit DialogCardProductLabel(QWidget *parent = 0);
    ~DialogCardProductLabel();

signals:
    void saved(const QString &id);

protected:
    void initSignals();

    virtual void initControls();
    virtual void fillControls();

    bool setData();
    bool save();

private slots:
    void onModified();


    void on_pushButtonTranslit_clicked();

private:
    Ui::DialogCardProductLabel *ui;

    FrameImage *widgetImage();

};


#endif // DIALOGCARDPRODUCTLABEL_H
