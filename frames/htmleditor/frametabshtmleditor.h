#ifndef FRAMETABSHTMLEDITOR_H
#define FRAMETABSHTMLEDITOR_H

#include "framewysiwyg.h"

#include <QWidget>

namespace Ui {
class FrameTabsHTMLEditor;
}

class FrameTabsHTMLEditor : public QWidget
{
    Q_OBJECT

public:
    explicit FrameTabsHTMLEditor(QWidget *parent = 0);
    ~FrameTabsHTMLEditor();

    void setHTMLText(const QString &text);
    void setSection(const int &section);

    QString text();
    QString textHTML();

    void refreshSourceText();

signals:
    void modified();
    void documentChanged();
    void codeHTMLChanged(const QString &textHTML);

private:
    Ui::FrameTabsHTMLEditor *ui;
    int mSection;
    QString mHTMLText;
    QVector<FrameWYSIWYG *> mEditors;   

    void addTabHTMLEditor(const QString &name, const QString &html = QString(), const bool &isDeleted = true);
    void createTabs();
    void deleteTabs();

    void onChangeTabName(const QString &name);
    void onAddTab();
    void onDelTab();      
};

#endif // FRAMETABSHTMLEDITOR_H
