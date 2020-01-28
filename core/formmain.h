#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QStackedWidget>

#include "formcontacts.h"
#include "formorders.h"
#include "formproducts.h"
#include "formpublications.h"
#include "formsettings.h"
#include "formpayments.h"
#include "formsesections.h"
#include "formreviews.h"
#include "formlibimages.h"
#include "formcatalog.h"
#include "dialogabout.h"

class FormMain : public QMainWindow
{
    Q_OBJECT

public:
    FormMain();

signals:
    void windowShowed();

public slots:
    void reloadProject();

private slots:    
    void showProducts();
    void showContacts();
    void showOrders();    
    void showNews();
    void showSettings();
    void showPayments();
    void showReviews();
    void showComments();
    void showLibImages();
    void showSESections();
    void showSupport();

    void relogin();
    void showAboutInfo();

private:        
    void mousePressEvent(QMouseEvent *event);        
    void closeEvent(QCloseEvent *event);

    void createActions();
    void createMenus();
    void createToolBars();
    void createStatusBar();
    void initVariables();
    void startDefaultModule();
    void updateParametersForCurrentVersion();
    void updateUpdater();
    void updateApiExt();

    bool isInitConfig;

    DialogAbout *dialogAbout;

    QMenu *menuCRM;
    QMenu *menuShop;        

    QAction *actContacts;
    QAction *actOrders;
    QAction *actProducts;    
    QAction *actNews;
    QAction *actPayments;
    QAction *actSettings;
    QAction *actDesign;
    QAction *actReviews;
    QAction *actComments;
    QAction *actLibImages;
    QAction *actSupport;
    QAction *actExit;
    QAction *actLogin;
    QAction *actAbout;

    QStackedWidget *stackedWidget;

    FormContacts *formContacts;
    FormOrders *formOrders;
    FormProducts *formProducts;
    FormPublications *formPublications;
    FormSettings *formSettings;
    FormPayments *formPayments;
    FormSESections *formSections;
    FormReviews *formReviews;
    FormCatalog *formComments;
    FormLibImages *formLibImages;

    QAction *productsAct;
    QAction *contactsAct;

    QToolBar *toolbarMain;

};

#endif // MAINWINDOW_H
