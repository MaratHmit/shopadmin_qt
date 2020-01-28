#ifndef FRAMECARDCROSSGROUPS_H
#define FRAMECARDCROSSGROUPS_H

#include <QDialog>

#include "dataitem.h"
#include "nametablemodel.h"

namespace Ui {
class FrameCardCrossGroups;
}

class FrameCardCrossGroups : public QDialog
{
    Q_OBJECT

public:
    explicit FrameCardCrossGroups(QWidget *parent,
                                  ListDataItems *listGroups,
                                  const bool &isDialogMode = false);
    ~FrameCardCrossGroups();

    void fillControls();    

signals:
    void modified();

private slots:
    void onAddGroups();
    void onDeleteGroups();
    void onChange();

private:
    Ui::FrameCardCrossGroups *ui;

    ListDataItems *mListGroups;
    NameTableModel *mModel;

    void initSignals();
    bool mIsDialogMode;
};

#endif // FRAMECARDCROSSGROUPS_H
