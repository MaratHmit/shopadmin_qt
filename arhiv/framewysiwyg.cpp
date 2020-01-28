#include "framewysiwyg.h"
#include "ui_framewysiwyg.h"
#include "seconfig.h"
#include "apiadapter.h"

#include <QWebElement>
#include <QDebug>

FrameWYSIWYG::FrameWYSIWYG(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FrameWYSIWYG)
{
    ui->setupUi(this);
    mFrameWYSIWYG = 0;
    mFrameHTML = 0;
    mSetContentWYSIWYG = false;
    initSignals();
    initHTMLEditor();
    initWYSIWYGEditor();
}

FrameWYSIWYG::~FrameWYSIWYG()
{
    delete ui;    
}

void FrameWYSIWYG::showEvent(QShowEvent *)
{
    ui->tabWidget->setCurrentIndex(0);
    setHTMLEditorText();
}

void FrameWYSIWYG::setHTMLText(const QString &text)
{
    mHTMLText = text;
}

void FrameWYSIWYG::setSection(const int &/*section*/)
{

}

QString FrameWYSIWYG::text()
{
    return QString();
}

QString FrameWYSIWYG::textHTML()
{
    return QString();
}

void FrameWYSIWYG::onChaneEditor(const int &index)
{    
    if (index == 0 && mFrameWYSIWYG)
        setHTMLEditorText();
    if (index == 1 && mFrameWYSIWYG)
        setWYSIWYGEditorText();
}

void FrameWYSIWYG::initSignals()
{
    connect(ui->tabWidget, SIGNAL(currentChanged(int)),
            SLOT(onChaneEditor(int)));
}

void FrameWYSIWYG::initWYSIWYGEditor()
{
    if (mFrameWYSIWYG)
        return;

    QWebPage *page = new QWebPage(ui->webViewWYSIWYG);    
    page->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
    mFrameWYSIWYG = page->mainFrame();
    ui->webViewWYSIWYG->setPage(page);
}

void FrameWYSIWYG::initHTMLEditor()
{
    if (mFrameHTML)
        return;

    QWebPage *page = new QWebPage(ui->webViewHTMLCode);
    page->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
    mFrameHTML = page->mainFrame();        
    ui->webViewHTMLCode->setPage(page);    
}

void FrameWYSIWYG::setWYSIWYGEditorText()
{
    if (mFrameWYSIWYG) {
        mFrameWYSIWYG->setHtml(ApiAdapter::api()->editorWYSIWYG(), QUrl(SEConfig::urlWYSIWYGEditor()));
        QWebElement el = mFrameWYSIWYG->findFirstElement("textarea");
        el.setPlainText(mHTMLText);
    }
}

void FrameWYSIWYG::setHTMLEditorText()
{
    if (mFrameHTML) {
        mFrameHTML->setHtml(ApiAdapter::api()->editorHTMLCode(), QUrl(SEConfig::urlHTMLEditor()));
        QWebElement el = mFrameHTML->findFirstElement("textarea");
        el.setPlainText(mHTMLText);
    }
}
