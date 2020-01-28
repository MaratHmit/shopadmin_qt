#include "formlogin.h"
#include "formmain.h"
#include "apiadapter.h"
#include "dataitem.h"
#include "datacontact.h"

#include <QApplication>
#include <QStyle>
#include <QStyleFactory>
#include <seconfig.h>
#include <QtGui>
#include <QDebug>
#include <QSplashScreen>

bool Auth(int argc, char *argv[]) {

    FormLogin authForm;
    bool isUriAuth = false;
    if (argc > 3 ||
            (argc > 1 && (isUriAuth = QString(argv[1]).contains("SEManager5.3:", Qt::CaseInsensitive)))) {
        if (isUriAuth) {
            QString paramsStr = QString(argv[1]).remove("SEManager5.3:", Qt::CaseInsensitive);
            QStringList paramsList = paramsStr.split("&");
            if (paramsList.size() > 2) {
                SEConfig::projectAlias = paramsList.at(0);
                SEConfig::serial = paramsList.at(1);
                SEConfig::keyHash = paramsList.at(2);
                if (SEConfig::keyHash.length() < 32)
                    SEConfig::keyHash = QMD5(SEConfig::keyHash);
            }
        } else {
            SEConfig::projectAlias = QString(argv[1]);
            SEConfig::serial = QString(argv[2]);
            SEConfig::keyHash = QString(argv[3]);
        }        
        if (authForm.authAuto())
            return true;
    } else if (authForm.exec() == QDialog::Accepted)
        return true;
    return false;
}

int main(int argc, char *argv[]){ 

    QApplication app(argc, argv);

    SEConfig::regAutoStart();  

    ApiAdapter::getApi()->isCompressed = 0;

    #if defined(QT_DEBUG)
        SEConfig::setIsDebugMode(1);
    #endif    

    app.setOrganizationName("Edgestile");
    app.setApplicationName("Менеджер SE");
    app.setApplicationVersion("5.3.346");
    app.setApplicationDisplayName(app.applicationName() + " "  + app.applicationVersion());

    SEConfig::idThreadUi = QThread::currentThreadId();
    QDir::setCurrent(QCoreApplication::applicationDirPath());

    QTranslator translator;
    translator.load("qt_ru", QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    app.installTranslator(&translator);

    qRegisterMetaType<DataItem *>();
    qRegisterMetaType<DataContact *>();

    SEConfig::createDebugLogFile();

    if (Auth(argc, argv)) {
        QPixmap pixmap(":/128/icons/splach.png");
        QSplashScreen splash(pixmap);
        splash.show();        
        app.processEvents();

        FormMain window;
        window.showMaximized();
        splash.finish(&window);
        return app.exec();
    } else return 0;
}
