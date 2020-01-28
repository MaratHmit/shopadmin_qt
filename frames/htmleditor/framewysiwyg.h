#ifndef FRAMEWYSIWYG_H
#define FRAMEWYSIWYG_H

#include "xmlhighlighter.h"
#include "dialoginsertimage.h"

#include <QWebFrame>
#include <QUrl>
#include <QWidget>
#include <QWebView>
#include <QMessageBox>
#include <QDebug>

class JavaScriptOperations : public QObject {
    Q_OBJECT
public:
    Q_INVOKABLE QString insertImage() {
        DialogInsertImage dlg;
        dlg.setSection(mSection);

        if (dlg.exec() == QDialog::Accepted)
            return dlg.getURL();

        return QString();
    }

    Q_INVOKABLE QString editImage(const QString &imageSrc, const QString &imageHeight,
                                  const QString &imageWidth, const QString &imageStyle) {
        DialogInsertImage dlg;
        dlg.setSection(mSection);
        dlg.setImageCode(imageSrc, imageHeight, imageWidth, imageStyle);

        if (dlg.exec() == QDialog::Accepted)
            return dlg.getURL();

        return QString();
    }

    void setSection(const int &section) {
        mSection = section;
    }

private:
    int mSection;
};

namespace Ui {
class FrameWYSIWYG;
}

class FrameWYSIWYG : public QWidget
{
    Q_OBJECT

public:
    explicit FrameWYSIWYG(QWidget *parent = 0);
    ~FrameWYSIWYG();

    void setHTMLText(const QString &text = QString());
    void setSection(const int &section, const bool isReplaceImage = true);
    void refreshSourceText();    

    const QString textHTML() const;
    const QString text() const;

signals:
    void modified();    

private slots:    
    void onChangeEditor(const int &index);        
    void onPageLoadFinish(const bool &loaded);
    void onClearStylesInHtml();
    void refreshPage();

private:
    Ui::FrameWYSIWYG *ui;
    QWebFrame *mFrameWYSIWYG;        
    QWebView *mWebView;
    int mSection;
    QString mImagePath;
    XmlHighlighter *mHighlight;  
    JavaScriptOperations *mJSOperation;
    bool mIsLoaded;
    bool mIsReplaceUrl;

    bool mSetContentWYSIWYG;

    QString setLocalURLImage(const QString &text);
    QString setServerURLImage(const QString &text);

    void initWYSIWYGEditor();    
    void setWYSIWYGEditorText();       
    void setPlainText();
    void loadImages(const QString &text);

};

#endif // FRAMEWYSIWYG_H
