#include "dialogaddfromymurl.h"
#include "ui_dialogaddfromymurl.h"
#include "apiadapter.h"

#include <QWebPage>
#include <QWebFrame>
#include <QMessageBox>
#include <QNetworkCookie>
#include <QNetworkCookieJar>
#include <QSettings>
#include <QList>

DialogAddFromYMUrl::DialogAddFromYMUrl(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogAddFromYMUrl)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Window);
    isShowed = false;
    preloader = new Preloader(this);

    initSignals();
}

DialogAddFromYMUrl::~DialogAddFromYMUrl()
{
    delete ui;
}

void DialogAddFromYMUrl::goToPage()
{
    QString urlString = ui->lineEditUrl->text().trimmed();
    if (urlString.indexOf("://") == -1)
        urlString = "http://" + urlString;

    QUrl url(urlString);
    ui->webViewYandexMarket->load(url);
}

void DialogAddFromYMUrl::urlChanged(const QUrl &url)
{
    QString urlString = url.toDisplayString();
    ui->lineEditUrl->setText(urlString);
    ui->pushButtonParse->setEnabled(urlString.indexOf("https://market.yandex.ru/product/") != -1);    
}

void DialogAddFromYMUrl::initSignals()
{
    connect(ui->lineEditUrl, SIGNAL(returnPressed()), SLOT(goToPage()));
    connect(ui->pushButtonBack, SIGNAL(clicked(bool)), ui->webViewYandexMarket, SLOT(back()));
    connect(ui->pushButtonNext, SIGNAL(clicked(bool)), ui->webViewYandexMarket, SLOT(forward()));    
    connect(ui->pushButtonParse, SIGNAL(clicked(bool)), SLOT(importProduct()));
    connect(ui->webViewYandexMarket, SIGNAL(loadStarted()), SLOT(loadStarted()));
    connect(ui->webViewYandexMarket, SIGNAL(loadFinished(bool)), SLOT(loadFinished(bool)));
}

void DialogAddFromYMUrl::getCookies()
{
    QSettings settings;
    QString cookiesString = settings.value("/cookies/ym/", QString()).toString();
    if (!cookiesString.isEmpty()) {
        QList<QNetworkCookie> listCookies;
        QStringList cookiesList = cookiesString.split("\n");
        foreach (QString cookieItem, cookiesList) {
            QStringList nameValue = cookieItem.split("=");
            if (nameValue.size() == 2) {
                QNetworkCookie cookie;
                cookie.setName(nameValue.at(0).toLatin1());
                cookie.setValue(nameValue.at(1).toLatin1());
                listCookies.append(cookie);
            }
        }
        ui->webViewYandexMarket->page()->networkAccessManager()->
                    cookieJar()->setCookiesFromUrl(listCookies, QUrl("https://market.yandex.ru"));
    }

}

void DialogAddFromYMUrl::saveCookies()
{
    QList<QNetworkCookie> listCookies;
    QSettings settings;
    listCookies = ui->webViewYandexMarket->page()->networkAccessManager()->
            cookieJar()->cookiesForUrl(ui->webViewYandexMarket->url());
    QByteArray cookiesBytes;
    for (int i = 0; i < listCookies.size(); i++) {
        cookiesBytes += listCookies[i].name();
        cookiesBytes += "=";
        cookiesBytes += listCookies[i].value();
        if (i < listCookies.size())
            cookiesBytes += "\n";
    }
    settings.setValue("/cookies/ym/", QString(cookiesBytes));
    settings.sync();

}

void DialogAddFromYMUrl::importProduct()
{
    saveCookies();
    QString html = ui->webViewYandexMarket->page()->mainFrame()->toHtml();

    DataProduct *product = new DataProduct;
    product->setProperty("urlParse", ui->lineEditUrl->text().trimmed());
    product->setProperty("html", html);
    preloader->startAnimation();
    if (ApiAdapter::getApi()->importProductFromYMUrl(product)) {
        emit saved(product, true);
        QMessageBox::information(this, "Парсинг товара из Яндекс.Маркет",
                                 QString("Товар: " + product->getName() + " успешно добавлен!"));
    }
    preloader->stopAnimation();
    delete product;
}

void DialogAddFromYMUrl::loadStarted()
{
    ui->lineEditUrl->setEnabled(false);
    preloader->startAnimation();
}

void DialogAddFromYMUrl::loadFinished(bool)
{
    preloader->stopAnimation();
    ui->lineEditUrl->setEnabled(true);
}

void DialogAddFromYMUrl::showEvent(QShowEvent *)
{
    startTimer(100);
}

void DialogAddFromYMUrl::timerEvent(QTimerEvent *event)
{
    killTimer(event->timerId());
    if (!isShowed) {
        isShowed = true;
        connect(ui->webViewYandexMarket, SIGNAL(urlChanged(QUrl)), SLOT(urlChanged(QUrl)));
        getCookies();
        ui->webViewYandexMarket->load(QUrl("https://market.yandex.ru/?tab=catalog"));
    }
}



