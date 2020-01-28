#ifndef DIALOGCARDPRODUCTTYPE_H
#define DIALOGCARDPRODUCTTYPE_H

#include <QDialog>

#include "dialogcard.h"
#include "nametablemodel.h"
#include "datafeature.h"

namespace Ui {
class DialogCardProductType;
}

class DialogCardProductType : public DialogCard
{
    Q_OBJECT

public:
    explicit DialogCardProductType(QWidget *parent = 0);
    ~DialogCardProductType();    

protected:
    void initSignals();
    void initVariables();

    virtual void initControls();
    virtual void fillControls();

    bool setData();
    bool save();

private slots:
    void selectFeatures();
    void deleteFeatures();

    void currentRowChanged();

private:
    Ui::DialogCardProductType *ui;    

    ListFeatures *mFeatures;
    bool mIsModifiedFeatures;
    NameTableModel *mModelFeatures;

};

#endif // DIALOGCARDPRODUCTTYPE_H
