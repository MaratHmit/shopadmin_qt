#ifndef DIALOGIMPORT_H
#define DIALOGIMPORT_H

#include "preloader.h"
#include "request.h"

#include <QDialog>
#include <QByteArray>
#include <QTime>
#include <QTimer>

namespace Ui {
class DialogImport;
}

class DialogImport : public QDialog
{
    Q_OBJECT

public:
    explicit DialogImport(QWidget *parent = 0);
    ~DialogImport();

    void setApiCatalogName(const QString &apiCatalogName);

private:
    Ui::DialogImport *ui;
    Preloader *mPreloader;
    QString mApiCatalogName;
    QString mParams;
    QTime mTime;
    QTimer *mTimer;
    QByteArray *mData;

    void initVariables();
    void initSignals();
    void onGetFile();
    void onChangeTypeImport();
    void startImport();
    void updateTimeTitle();
};

#endif // DIALOGIMPORT_H
