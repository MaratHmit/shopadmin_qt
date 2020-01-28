#ifndef FRAMEWYSIWYG_H
#define FRAMEWYSIWYG_H

#include <QWebFrame>
#include <QUrl>
#include <QWidget>

namespace Ui {
class FrameWYSIWYG;
}

class FrameWYSIWYG : public QWidget
{
    Q_OBJECT

public:
    explicit FrameWYSIWYG(QWidget *parent = 0);
    ~FrameWYSIWYG();

    void showEvent(QShowEvent *e);
    void setHTMLText(const QString &text);
    void setSection(const int &section);

    QString text();
    QString textHTML();

signals:
    void modified();
    void documentChanged();

private slots:
    void onChaneEditor(const int &index);    

private:
    Ui::FrameWYSIWYG *ui;
    QWebFrame *mFrameWYSIWYG;
    QWebFrame *mFrameHTML;
    QString mHTMLText;

    bool mSetContentWYSIWYG;

    void initSignals();
    void initWYSIWYGEditor();
    void initHTMLEditor();
    void setWYSIWYGEditorText();
    void setHTMLEditorText();
};

#endif // FRAMEWYSIWYG_H
