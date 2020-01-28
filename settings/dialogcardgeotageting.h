#ifndef DIALOGCARDGEOTAGETING_H
#define DIALOGCARDGEOTAGETING_H

#include <QDialog>

#include "dataitem.h"
#include "dialogcard.h"
#include "dialogdelivregion.h"
#include "nametablemodel.h"

namespace Ui {
class DialogCardGeotageting;
}

class DialogCardGeotageting : public DialogCard
{
    Q_OBJECT

public:
    explicit DialogCardGeotageting(QWidget *parent = 0);
    ~DialogCardGeotageting();

protected:
    void initSignals();
    void initVariables();

    virtual void initControls();
    virtual void fillControls();

    bool setData();
    bool save();

private slots:
    void selectGeoInfo();

private:
    Ui::DialogCardGeotageting *ui;

    ListDataItems *seoVariables;
    NameTableModel *modelSeoVariables;
};

#endif // DIALOGCARDGEOTAGETING_H
