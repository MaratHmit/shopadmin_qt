#include "framewysiwyg.h"
#include "ui_framewysiwyg.h"
#include "seconfig.h"
#include "apiadapter.h"
#include "seutils.h"
#include "filewebloader.h"

#include <QThread>
#include <QDir>
#include <QFile>
#include <QWebElement>
#include <QPushButton>
#include <QColor>
#include <QDebug>

FrameWYSIWYG::FrameWYSIWYG(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FrameWYSIWYG)
{
    ui->setupUi(this);
    mFrameWYSIWYG = 0;    
    mSetContentWYSIWYG = 0;
    mIsLoaded = 0;        
    mIsReplaceUrl = true;
    mJSOperation = new JavaScriptOperations;
    mHighlight = new XmlHighlighter(ui->plainTextEditHTML->document());
    if (SEConfig::useWYSIWYG) {
        ui->tabWidget->setCurrentIndex(0);
        connect(ui->tabWidget, SIGNAL(currentChanged(int)), SLOT(onChangeEditor(int)));
        connect(ui->toolButtonClearStyles, SIGNAL(clicked()), SLOT(onClearStylesInHtml()));
        initWYSIWYGEditor();
    } else
        ui->tabWidget->removeTab(0);
}

FrameWYSIWYG::~FrameWYSIWYG()
{
    delete ui;
}

void FrameWYSIWYG::setHTMLText(const QString &text)
{
    if (text == textHTML())
        return;

    if (!text.isEmpty())
        loadImages(text);

    ui->plainTextEditHTML->document()->setPlainText(text);    
    connect(ui->plainTextEditHTML, SIGNAL(textChanged()), SIGNAL(modified()));
    if (isVisible() && mIsLoaded)
        setWYSIWYGEditorText();
}

void FrameWYSIWYG::setSection(const int &section, const bool isReplaceImage)
{
    mSection = section;
    mImagePath = SEConfig::getFolderImages(section);
    mImagePath.replace("\\", "/");
    if (mJSOperation)
        mJSOperation->setSection(mSection);
    mIsReplaceUrl = isReplaceImage;
}

void FrameWYSIWYG::refreshSourceText()
{
    if (!ui->tabWidget->currentIndex())
        setPlainText();
}

const QString FrameWYSIWYG::textHTML() const
{
    return ui->plainTextEditHTML->document()->toPlainText();
}

const QString FrameWYSIWYG::text() const
{
    return ui->plainTextEditHTML->document()->toPlainText();
}

void FrameWYSIWYG::onChangeEditor(const int &index)
{
    if (index)
        setPlainText();
    else setWYSIWYGEditorText();
}

void FrameWYSIWYG::onPageLoadFinish(const bool &loaded)
{
    mIsLoaded = loaded;
    if (loaded && !ui->tabWidget->currentIndex())
        setWYSIWYGEditorText();
}

void FrameWYSIWYG::onClearStylesInHtml()
{
    QString source = ui->plainTextEditHTML->document()->toPlainText();

    QString tags = "font|b|span|div";

    QRegExp exp;
    exp.setMinimal(true);

    exp.setPattern("\\<\\/?(" + tags + ")[\\s][^\\>]*\\>");
    source.replace(exp, "");
    exp.setPattern("\\<\\/?(" + tags + ")\\>");
    source.replace(exp, "");
    exp.setPattern("<p></p>");
    source.replace(exp, "");
    exp.setPattern("(style|class|id)[\\s]*=[\\s]*\"(.*)\"");
    source.replace(exp, "");
    exp.setPattern("\\s\\>");
    source.replace(exp, ">");
    exp.setPattern("\\b[A-Z]:(.*)");
    source.replace(exp, "");
    exp.setPattern("\\s\\s");
    source.replace(exp, "");

    ui->plainTextEditHTML->document()->setPlainText(source);
}

void FrameWYSIWYG::refreshPage()
{
    if (!ui->tabWidget->currentIndex())
        ui->webViewEditor->triggerPageAction(QWebPage::ReloadAndBypassCache);
}

QString FrameWYSIWYG::setLocalURLImage(const QString &text)
{
    QString s = text;
    QRegExp exp;
    exp.setMinimal(true);
    exp.setPattern("(<img[^>]*src=\"(?!http:)).*([^/]*\")");    
    s.replace(exp, "\\1" + SEConfig::getUrlImages(mSection) + "/" + "\\2");
    s.replace("<shoplist>", "<!--<shoplist>-->");
    s.replace("</shoplist>", "<!--</shoplist>-->");
    s.replace("<delivery>", "<!--<delivery>-->");
    s.replace("</delivery>", "<!--</delivery>-->");
    s.replace("<SHOPLIST>", "<!--<SHOPLIST>-->");
    s.replace("</SHOPLIST>", "<!--</SHOPLIST>-->");
    s.replace("<DELIVERY>", "<!--<DELIVERY>-->");
    s.replace("</DELIVERY>", "<!--</DELIVERY>-->");

    return s;
}

QString FrameWYSIWYG::setServerURLImage(const QString &text)
{
    QString address = SEConfig::getUrlImages(mSection, true);

    QString s = text;
    s.replace("http://" + SEConfig::projectHostName + "/images", "/images");

    QRegExp exp;
    exp.setMinimal(true);
    exp.setPattern("(<img[^>]*src=\"(?!http:)).*([^/]*\")");
    if (mIsReplaceUrl)
        s.replace(exp, "\\1" + address + "/" + "\\2");
    s.replace("<!--<shoplist>-->", "<shoplist>");
    s.replace("<!--</shoplist>-->", "</shoplist>");
    s.replace("<!--<delivery>-->", "<delivery>");
    s.replace("<!--</delivery>-->", "</delivery>");
    s.replace("<!--<SHOPLIST>-->", "<SHOPLIST>");
    s.replace("<!--</SHOPLIST>-->", "</SHOPLIST>");
    s.replace("<!--<DELIVERY>-->", "<DELIVERY>");
    s.replace("<!--</DELIVERY>-->", "</DELIVERY>");

    return s;
}

void FrameWYSIWYG::initWYSIWYGEditor()
{
    if (mFrameWYSIWYG)
        return;

    mWebView = ui->webViewEditor;
    QWebPage *page = new QWebPage(mWebView);
    page->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
    mFrameWYSIWYG = page->mainFrame();
    QString url = "file:///" + QDir::currentPath() + "/wysiwyg/editor.html";
    mFrameWYSIWYG->setUrl(QUrl(url));
    mWebView->setPage(page);
    mFrameWYSIWYG->addToJavaScriptWindowObject("qtOperations", mJSOperation);
    connect(page, SIGNAL(loadFinished(bool)), SLOT(onPageLoadFinish(bool)));
}

void FrameWYSIWYG::setWYSIWYGEditorText()
{
    if (mFrameWYSIWYG) {                        
        disconnect(mFrameWYSIWYG->page(), SIGNAL(contentsChanged()));
        QString s = "setContent('" +
                base64Encode(setLocalURLImage(ui->plainTextEditHTML->document()->toPlainText())) + "');";
        mFrameWYSIWYG->evaluateJavaScript(s);                        
        connect(mFrameWYSIWYG->page(), SIGNAL(contentsChanged()), SIGNAL(modified()));        
    }
}

void FrameWYSIWYG::setPlainText()
{
    if (!mFrameWYSIWYG)
        return;

    ui->plainTextEditHTML->disconnect();
    ui->plainTextEditHTML->document()->setPlainText(
                setServerURLImage(mFrameWYSIWYG->evaluateJavaScript("getSource();").toString()));    
    connect(ui->plainTextEditHTML, SIGNAL(textChanged()), SIGNAL(modified()));
}

void FrameWYSIWYG::loadImages(const QString &text)
{
    QStringList listImages;
    QString folder = SEConfig::getFolderImages(mSection);
    QString s = text;
    QRegExp exp;
    exp.setMinimal(true);
    exp.setPattern("(<img[^>]*src=\")(.*)([^/]*\")");
    int pos = 0;
    while (pos >= 0) {
        pos = exp.indexIn(s, pos);
        if (pos >= 0) {
            if (exp.captureCount() > 1) {
                QString file = exp.cap(2).replace("\\", "/");
                int i = file.lastIndexOf("/");
                file = file.right(file.length() - i - 1);
                QFileInfo fileInfo(folder, file);
                if (!fileInfo.exists())
                    listImages.append(file);
            }
            pos += exp.matchedLength();
        }
    }
}
