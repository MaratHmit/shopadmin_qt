#ifndef FRAMEHTMLEDITOR_H
#define FRAMEHTMLEDITOR_H

#include <QWidget>

#include "xmlhighlighter.h"

namespace Ui {
class FrameHTMLEditor;
}

class FrameHTMLEditor : public QWidget
{
    Q_OBJECT

public:
    explicit FrameHTMLEditor(QWidget *parent = 0);
    ~FrameHTMLEditor();

    void showEvent(QShowEvent *e);
    void setHTMLText(const QString &text = QString());
    void setSection(const int &section);

    QString text();
    QString textHTML();

signals:
    void modified();
    void documentChanged();
    void codeHTMLChanged(const QString &textHTML);

private slots:
    void onTabWidgetChanged(int index);
    void onDocumentChanged();

    void setFontText(const QFont &font);
    void setFontSize(const int &size);
    void setFontBold(const bool &isBold);
    void setTextColor();
    void setTextBackgroundColor();
    void linkURL();
    void unLinkURL();
    void setAlignmentLeft();
    void setAlignmentRight();
    void setAlignmentCenter();
    void setAlignmentJustify();
    void insertImage();
    void addTable();    
    void setNumList();
    void setList();
    void addShift();
    void delShift();

private:
    Ui::FrameHTMLEditor *ui;
    int mSection;
    QString mImagePath;
    XmlHighlighter *mHighlighter;    

    QString transformHTML(const QString &text);
    QString setLocalURLImage(const QString &text);
    QString unSetLocalURLImage(const QString &text);
    // возвращает выделенный фрагмент HTML
    QString selectionHTML() const;
    void initSignals();
    void loadImages(const QString &html);

    const QString getImageFileFromSelection(const QString &html);
    const QString getSelectionFragment(const QString &html);


};

#endif // FRAMEHTMLEDITOR_H
