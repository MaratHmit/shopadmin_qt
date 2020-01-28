#include <QApplication>
#include <QAction>
#include <QActionGroup>
#include <QToolBar>
#include <QToolButton>
#include <QMenu>
#include <QMenuBar>
#include <QStatusBar>
#include <QSettings>
#include <QThread>
#include <QErrorMessage>

#include "formmain.h"
#include "formlogin.h"
#include "seconfig.h"
#include "userpermission.h"
#include "formcommentcard.h"
#include "dialogcomplaint.h"

FormMain::FormMain()
{    
    stackedWidget = new QStackedWidget(this);
    setCentralWidget(stackedWidget);

    isInitConfig = false;
    updateUpdater();
    initVariables();
    createActions();
    createToolBars();        
    startDefaultModule();
    updateApiExt();
}

void FormMain::reloadProject()
{
    setEnabled(false);
    SEConfig::reset();
    FormMain *formMain = new FormMain();
    formMain->showMaximized();
    close();
    setEnabled(true);
}

void FormMain::createActions()
{
    QActionGroup  *actGroups = new QActionGroup(this);

    if (UserPermission::isRead(UserPermission::CONTACTS)) {
        actContacts = new QAction(QIcon(":/48/icons/48x48/iconcrm_contacts_48.png"),
                                      tr("Контакты"), this);
        actContacts->setObjectName("actContacts");
        actContacts->setStatusTip(tr("Контакты"));
        actContacts->setCheckable(true);
        actGroups->addAction(actContacts);
        connect(actContacts, &QAction::triggered, this, &FormMain::showContacts);
    }
    if (UserPermission::isRead(UserPermission::ORDERS)) {
        actOrders = new QAction(QIcon(":/48/icons/48x48/iconcrm_orders_48.png"),
                                tr("Заказы"), this);
        actOrders->setStatusTip(tr("Заказы"));
        actOrders->setCheckable(true);
        actGroups->addAction(actOrders);
        connect(actOrders, SIGNAL(triggered()), this, SLOT(showOrders()));
    }
    if (UserPermission::isRead(UserPermission::PAYMENTS)) {
        actPayments = new QAction(QIcon(":/48/icons/48x48/payments.png"),
                                      tr("Платежи"), this);
        actPayments->setStatusTip(tr("Платежи к заказам"));
        actPayments->setCheckable(true);
        actGroups->addAction(actPayments);
        connect(actPayments, SIGNAL(triggered()), this, SLOT(showPayments()));
    }
    if (UserPermission::isRead(UserPermission::PRODUCTS)) {
        actProducts = new QAction(QIcon(":/48/icons/48x48/Products.png"),
                                      tr("Товары"), this);
        actProducts->setStatusTip(tr("Товары"));
        actProducts->setCheckable(true);
        actGroups->addAction(actProducts);
        connect(actProducts, SIGNAL(triggered()), this, SLOT(showProducts()));
    }
    if (UserPermission::isRead(UserPermission::NEWS)) {
        actNews = new QAction(QIcon(":/48/icons/48x48/news.png"),
                              tr("Новости"), this);
        actNews->setStatusTip(tr("Новости"));
        actNews->setCheckable(true);
        actGroups->addAction(actNews);
        connect(actNews, SIGNAL(triggered()), this, SLOT(showNews()));
    }
    if (UserPermission::isRead(UserPermission::SETTINGS) &&
            (UserPermission::isRead(UserPermission::DELIVERIES) ||
            UserPermission::isRead(UserPermission::PAYSYSTEMS) ||
            UserPermission::isRead(UserPermission::MAILS) ||
            UserPermission::isRead(UserPermission::CURRENCIES))) {
        actSettings = new QAction(QIcon(":/48/icons/48x48/iconcrm_tools_48.png"),
                                      tr("Настройки"), this);
        actSettings->setStatusTip(tr("Настройки и дополнительные справочники"));
        actSettings->setCheckable(true);
        actGroups->addAction(actSettings);
        connect(actSettings, SIGNAL(triggered()), this, SLOT(showSettings()));
    }
    if (UserPermission::isRead(UserPermission::REVIEWS)) {
        actReviews = new QAction(QIcon(":/32/icons/32x32/feedback.png"),
                                      tr("Отзывы"), this);
        actReviews->setStatusTip(tr("Отзывы о товарах"));
        actReviews->setCheckable(true);
        actGroups->addAction(actReviews);
        connect(actReviews, SIGNAL(triggered()), this, SLOT(showReviews()));
    }
    if (UserPermission::isRead(UserPermission::COMMENTS)) {
        actComments = new QAction(QIcon(":/32/icons/32x32/comments.png"),
                                      tr("Комменты"), this);
        actComments->setStatusTip(tr("Комментарии к товарам"));
        actComments->setCheckable(true);
        actGroups->addAction(actComments);
        connect(actComments, SIGNAL(triggered()), this, SLOT(showComments()));
    }
    if (UserPermission::isRead(UserPermission::IMAGES)) {
        actLibImages = new QAction(QIcon(":/32/icons/32x32/pictures.png"),
                                      tr("Картинки"), this);
        actLibImages->setStatusTip(tr("Галерея картинок"));
        actLibImages->setCheckable(true);
        actGroups->addAction(actLibImages);
        connect(actLibImages, SIGNAL(triggered()), this, SLOT(showLibImages()));
    }
    if (UserPermission::isAdmin) {
        actDesign = new QAction(QIcon(":/48/icons/48x48/web-design-8-48.png"),
                                      tr("Разделы"), this);
        actDesign->setStatusTip(tr("Разделы"));
        actDesign->setCheckable(true);
        actGroups->addAction(actDesign);
        connect(actDesign, SIGNAL(triggered()), this, SLOT(showSESections()));
    }    
    if (UserPermission::isAdmin) {
        actSupport = new QAction(QIcon(":/48/icons/32x32/technical-support.png"), tr("Тех. помощь"), this);
        actSupport->setStatusTip(tr("Техническая поддержка"));
        connect(actSupport, SIGNAL(triggered()), this, SLOT(showSupport()));
    }
    actLogin = new QAction(QIcon(":/48/icons/48x48/account.png"), tr("Логин"), this);
    actLogin->setStatusTip(tr("Перелогиниться"));
    connect(actLogin, SIGNAL(triggered()), this, SLOT(relogin()));
    actAbout = new QAction(QIcon(":/48/icons/48x48/info.png"), tr("О программе"), this);
    actAbout->setStatusTip(tr("О программе"));
    connect(actAbout, SIGNAL(triggered()), this, SLOT(showAboutInfo()));

    actExit = new QAction(tr("Выход"), this);
    actExit->setStatusTip(tr("Выход"));
    connect(actExit, SIGNAL(triggered()), this, SLOT(close()));
}

void FormMain::createToolBars()
{
    QSettings settings;
    int toolBarArea = settings.value("/ui/toolBarArea", Qt::TopToolBarArea).toInt();
    toolbarMain = new QToolBar(this);
    addToolBar(static_cast <Qt::ToolBarArea> (toolBarArea), toolbarMain);
    toolbarMain->setIconSize(QSize(32, 32));
    if (SEConfig::getMainInfo()->isShowSESection())
        toolbarMain->addAction(actDesign);
    if (UserPermission::isAdmin || SEConfig::getMainInfo()->isShowSESection())
        toolbarMain->addSeparator();
    toolbarMain->addAction(actOrders);
    toolbarMain->addAction(actPayments);
    toolbarMain->addAction(actContacts);
    toolbarMain->addAction(actProducts);
    toolbarMain->addAction(actNews);
    if (UserPermission::isRead(UserPermission::COMMENTS) || UserPermission::isRead(UserPermission::REVIEWS)) {
        toolbarMain->addSeparator();
        toolbarMain->addAction(actReviews);
        toolbarMain->addAction(actComments);
    }
    if (UserPermission::isRead(UserPermission::IMAGES) || UserPermission::isRead(UserPermission::SETTINGS)) {
        toolbarMain->addSeparator();
        toolbarMain->addAction(actLibImages);
        toolbarMain->addAction(actSettings);
    }  

    QWidget *widget = new QWidget(toolbarMain);
    widget->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
    toolbarMain->addWidget(widget);

    toolbarMain->addSeparator();
    toolbarMain->addAction(actLogin);
    if (UserPermission::isAdmin)
        toolbarMain->addAction(actSupport);
    toolbarMain->addAction(actAbout);

    QList<QToolButton *> allButtons = toolbarMain->findChildren<QToolButton *>();

    QListIterator<QToolButton *> i(allButtons);
    while (i.hasNext()) {
        QToolButton *btn = i.next();
        btn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        btn->setMinimumWidth(80);
    }
}

void FormMain::createMenus()
{
    menuCRM = menuBar()->addMenu("CRM");
    menuCRM->addAction(actContacts);    
    menuCRM->addAction(actSettings);
    menuCRM->addSeparator();
    menuCRM->addAction(actExit);

    menuShop = menuBar()->addMenu(tr("Магазин"));
    menuShop->addAction(actProducts);
    menuShop->addAction(actOrders);
    menuShop->addAction(actPayments);
    menuShop->addAction(actNews);
}

void FormMain::createStatusBar()
{
    statusBar()->showMessage(tr("Запуск"));
}

void FormMain::initVariables()
{
    setWindowTitle(QString(SEConfig::projectName + " - " + SEConfig::userDisplayName));

    dialogAbout = 0;

    formProducts = 0;
    formContacts = 0;
    formOrders = 0;
    formPublications = 0;
    formSettings = 0;
    formPayments = 0;
    formSections = 0;
    formComments = 0;
    formReviews = 0;
    formLibImages = 0;

    actOrders = 0;
    actContacts = 0;
    actProducts = 0;
    actPayments = 0;
    actLibImages = 0;
    actSettings = 0;
    actComments = 0;
    actNews = 0;
    actReviews = 0;
    actDesign = 0;

}

void FormMain::startDefaultModule()
{    
    if (actOrders)
        actOrders->trigger();
}

void FormMain::updateUpdater()
{
    QString fileUpdaterName = QApplication::applicationDirPath() + QDir::separator() + SEConsts::UPDATER_NAME;
    QString fileNewName= fileUpdaterName + ".new";
    QFile fileNew(fileNewName);
    QFile fileUpdater(fileUpdaterName);
    if (fileNew.exists()) {
        if (fileUpdater.exists())
            fileUpdater.remove();
        bool result = fileNew.rename(fileUpdaterName);
        if (!result)
            qDebug() << fileNew.errorString();
    }
}

void FormMain::updateApiExt()
{
    if (!SEConfig::isExtHosting || SEConfig::apiBuild > 0)
        return;    

    setCursor(Qt::WaitCursor);
    ApiAdapter::getApi()->updateApiExt();
    setCursor(Qt::ArrowCursor);
}

void FormMain::showProducts()
{    
    if (!formProducts) {
        setCursor(QCursor(Qt::WaitCursor));
        formProducts = new FormProducts(this);
        stackedWidget->addWidget(formProducts);
        setCursor(QCursor(Qt::ArrowCursor));        
    }
    stackedWidget->setCurrentWidget(formProducts);
}

void FormMain::showContacts()
{ 
    if (!formContacts) {
        setCursor(QCursor(Qt::WaitCursor));
        formContacts = new FormContacts(this);
        stackedWidget->addWidget(formContacts);
        setCursor(QCursor(Qt::ArrowCursor));
    }
    stackedWidget->setCurrentWidget(formContacts);
}

void FormMain::showOrders()
{    
    if (!formOrders) {
        setCursor(QCursor(Qt::WaitCursor));
        formOrders = new FormOrders(this);
        stackedWidget->addWidget(formOrders);
        setCursor(QCursor(Qt::ArrowCursor));
    }
    stackedWidget->setCurrentWidget(formOrders);
}

void FormMain::showNews()
{
    if (!formPublications) {
        setCursor(QCursor(Qt::WaitCursor));
        formPublications = new FormPublications(this);
        stackedWidget->addWidget(formPublications);
        setCursor(QCursor(Qt::ArrowCursor));
    }
    stackedWidget->setCurrentWidget(formPublications);
}

void FormMain::showSettings()
{
    if (!formSettings) {
        setCursor(QCursor(Qt::WaitCursor));
        formSettings = new FormSettings(this);        
        connect(formSettings, SIGNAL(reloadProject()), SLOT(reloadProject()));
        stackedWidget->addWidget(formSettings);
        setCursor(QCursor(Qt::ArrowCursor));
    }
    stackedWidget->setCurrentWidget(formSettings);
}

void FormMain::showPayments()
{
    if (!formPayments) {        
        formPayments = new FormPayments(this);
        stackedWidget->addWidget(formPayments);        
    }
    stackedWidget->setCurrentWidget(formPayments);
}

void FormMain::showReviews()
{
    if (!formReviews) {        
        formReviews = new FormReviews(this);
        stackedWidget->addWidget(formReviews);        
    }
    stackedWidget->setCurrentWidget(formReviews);
}

void FormMain::showComments()
{
    if (!formComments) {        
        formComments = new FormCatalog(this, SEConsts::API_COMMENTS);
        formComments->setWindowFlags(Qt::Widget);
        formComments->setSortingEnabled(true);
        formComments->setRootIsDecorated(false);
        formComments->setWindowTitle("Комментарии к товарам");
        formComments->setTitleButtonAdd("Добавить комментарий", "Добавить новый комментарий");
        formComments->addField(new Field("date", "Дата"));
        formComments->addField(new Field("contactTitle", "Имя пользователя"));
        formComments->addField(new Field("nameProduct", "Наименование товара"));
        formComments->addField(new Field("commentary", "Комментарий"));
        formComments->addField(new Field("response", "Ответ администратора"));
        FormCommentCard *cardComment = new FormCommentCard(formComments);
        formComments->setDialogCard(cardComment);
        stackedWidget->addWidget(formComments);        
    }
    stackedWidget->setCurrentWidget(formComments);
}

void FormMain::showLibImages()
{
    if (!formLibImages) {
        formLibImages = new FormLibImages(this);
        stackedWidget->addWidget(formLibImages);
        formLibImages->showImages();
    }
    stackedWidget->setCurrentWidget(formLibImages);
}

void FormMain::showSESections()
{
    if (!formSections) {
        setCursor(QCursor(Qt::WaitCursor));
        formSections = new FormSESections(this);
        stackedWidget->addWidget(formSections);
        setCursor(QCursor(Qt::ArrowCursor));
    }
    stackedWidget->setCurrentWidget(formSections);
}

void FormMain::showSupport()
{
    QString urlHelp;
    urlHelp = "https://helpdesk.siteedit.ru/otrs/customer.pl?Action=Login&User=" ;
    urlHelp += SEConfig::serial;
    urlHelp += "&Password=";
    urlHelp += QMD5(SEConfig::token);
    urlHelp += "&AuthType=hash";
    QDesktopServices::openUrl(QUrl(urlHelp));
}

void FormMain::relogin()
{
    QSettings settings;
    settings.setValue("/ui/toolBarArea", static_cast <int> (toolBarArea(toolbarMain)));
    setEnabled(false);

    FormLogin authForm;

    if (authForm.exec() == QDialog::Accepted) {
        SEConfig::reset();
        FormMain *formMain = new FormMain();                
        formMain->showMaximized();
        close();        
    }

    setEnabled(true);
}

void FormMain::showAboutInfo()
{
    if (!dialogAbout)
        dialogAbout = new DialogAbout(this);
    dialogAbout->show();
}

void FormMain::mousePressEvent(QMouseEvent *event)
{
    if (dialogAbout && dialogAbout->isVisible())
        dialogAbout->close();

    QMainWindow::mousePressEvent(event);
}

void FormMain::closeEvent(QCloseEvent *event)
{
    QMainWindow::closeEvent(event);

    QSettings settings;
    settings.setValue("/ui/toolBarArea", static_cast <int> (toolBarArea(toolbarMain)));
}
