#ifndef FORMLIBIMAGES_H
#define FORMLIBIMAGES_H

#include <QDialog>
#include <QLabel>

#include "api.h"
#include "frameliblistimages.h"
#include "seconfig.h"

namespace Ui {
class FormLibImages;
}

class FormLibImages : public QDialog
{
    Q_OBJECT

public:
    explicit FormLibImages(QWidget *parent = 0);
    ~FormLibImages();

    void setSelectMode(const int &section, const bool &isMultiSelect);
    void setWindowTitleBySection(const int &section);
    QString selectionFile();
    QStringList selectionFiles();
    const ListDataItems *selectionItems();
    void resetImages();
    void refreshImages();

    void showImages();

signals:
    void sigSetImage(QLabel *lbl, const QString &fileName);    

protected:
    void showEvent(QShowEvent *event);
    void timerEvent(QTimerEvent *event);

private slots:
    void on_toolButtonSelectSection();    
    void on_toolButtonAdd_clicked();
    void on_toolButtonDelete_clicked();
    void on_toolButtonSelect_clicked();    
    void on_toolButtonRemoveUnused_clicked();

    void checkEnabled();

private:
    Ui::FormLibImages *ui;    
    QList <QPushButton *> *mListButtons;
    FrameLibListDataItems *libListDataItems();

    bool mIsSelectionMode;    
    bool isAddPermission;
    bool isDeletePermission;
    bool isRefreshed;

    void initSectionsButtons();
    void initPermissions();
};

#endif // FORMLIBIMAGES_H
