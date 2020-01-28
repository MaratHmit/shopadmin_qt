#ifndef DIALOGADDFROMYMURL_H
#define DIALOGADDFROMYMURL_H

#include "dataproduct.h"
#include "preloader.h"

#include <QDialog>

namespace Ui {
class DialogAddFromYMUrl;
}

class DialogAddFromYMUrl : public QDialog
{
    Q_OBJECT

public:
    explicit DialogAddFromYMUrl(QWidget *parent = 0);
    ~DialogAddFromYMUrl();

signals:
    void saved(const DataProduct *product, const bool isNew);

private slots:
    void goToPage();
    void urlChanged(const QUrl&);
    void importProduct();

    void loadStarted();
    void loadFinished(bool);

private:
    Ui::DialogAddFromYMUrl *ui;
    Preloader *preloader;
    bool isShowed;

    void showEvent(QShowEvent *);
    void timerEvent(QTimerEvent *);

    void initSignals();
    void getCookies();
    void saveCookies();

};

#endif // DIALOGADDFROMYMURL_H
