#ifndef DIALOGPRODUCTSMASTERIMPORT_H
#define DIALOGPRODUCTSMASTERIMPORT_H

#include <QDialog>

#include "preloader.h"
#include "request.h"
#include "frameimportcolumn.h"
#include "dataitem.h"

namespace Ui {
class DialogProductsMasterImport;
}

class DialogProductsMasterImport : public QDialog
{
    Q_OBJECT

public:
    explicit DialogProductsMasterImport(QWidget *parent = 0);
    ~DialogProductsMasterImport();

private slots:
    void getFile();
    void run();
    void setProfile();

private:
    Ui::DialogProductsMasterImport *ui;

    int step;
    DataItem *currentProfile;
    ListDataItems *profiles;
    QByteArray *mData;
    QString mParams;
    Preloader *mPreloader;
    QVector <FrameImportColumn *> framesColumns;
    bool isXml;

    void initSignals();
    void initVariables();
    void createFields(QByteArray bytes);

    void loadProfiles();
    void runAnalysis();
    void runImport();
    void saveProfile();
};

#endif // DIALOGPRODUCTSMASTERIMPORT_H
