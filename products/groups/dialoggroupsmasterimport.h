#ifndef DIALOGGROUPSMASTERIMPORT_H
#define DIALOGGROUPSMASTERIMPORT_H

#include "preloader.h"
#include "request.h"
#include "frameimportcolumn.h"
#include "dataitem.h"

#include <QDialog>

namespace Ui {
class DialogGroupsMasterImport;
}

class DialogGroupsMasterImport : public QDialog
{
    Q_OBJECT

public:
    explicit DialogGroupsMasterImport(QWidget *parent = 0);
    ~DialogGroupsMasterImport();

private slots:
    void getFile();
    void run();
    void setProfile();

private:
    Ui::DialogGroupsMasterImport *ui;

    int step;
    DataItem *currentProfile;
    ListDataItems *profiles;
    QByteArray *mData;
    QString mParams;
    Preloader *mPreloader;
    QVector <FrameImportColumn *> framesColumns;

    void initSignals();
    void initVariables();
    void createFields(QByteArray bytes);

    void loadProfiles();
    void runAnalysis();
    void runImport();
    void saveProfile();
};

#endif // DIALOGGROUPSMASTERIMPORT_H
