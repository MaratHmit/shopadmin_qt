#ifndef FORMPUBLICATIONCARD_H
#define FORMPUBLICATIONCARD_H

#include <QDialog>

#include "datanew.h"
#include "framelistimages.h"

namespace Ui {
class FormPublicationCard;
}

class FormPublicationCard : public QDialog
{
    Q_OBJECT

public:
    explicit FormPublicationCard(QWidget *parent = 0);
    ~FormPublicationCard();

    void setNew(const DataNew *news);

private:
    Ui::FormPublicationCard *ui;    
    DataNew *mNews;
    FrameListImages *mFrameImages;

    void showEvent(QShowEvent *e);

    void onClickMenu();    
    void onOk();
    void onSelectGroup();

    bool save();
    void initVariables();
    void fillControls();
    void getData();
    void initSignals();
    void setModified();
};

#endif // FORMPUBLICATIONCARD_H
