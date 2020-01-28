#ifndef DIALOGEXPORT_H
#define DIALOGEXPORT_H

#include "preloader.h"
#include "request.h"

#include <QDialog>
#include <QByteArray>
#include <QTime>
#include <QTimer>

namespace Ui {
class DialogExport;
}

class DialogExport : public QDialog
{
    Q_OBJECT

public:
    explicit DialogExport(QWidget *parent = 0);    
    ~DialogExport();

    void setFilter(const Request &request);
    void setFileName(const QString &fileName);
    void setApiCatalogName(const QString &apiCatalogName);
    void setFormat(const QString &format);

private:
    Ui::DialogExport *ui;
    Preloader *mPreloader;
    Request mRequest;
    QString mApiCatalogName;
    QString mFileName;
    QString mFolderExport;
    QString mFormat;
    QByteArray *mData;
    QTime mTime;
    QTimer *mTimer;
    bool mIsModifiedFileName;

    void onChangeFileName(const QString &);
    void onSetFolder();
    void initVariables();
    void initValidators();
    void initSignals();
    void startExport();
    void updateTimeTitle();
};

#endif // DIALOGEXPORT_H
