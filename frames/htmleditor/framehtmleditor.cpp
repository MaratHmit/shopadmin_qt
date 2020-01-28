#include "framehtmleditor.h"
#include "ui_framehtmleditor.h"
#include "dialogseturl.h"
#include "dialoginsertimage.h"
#include "dialoghtmltable.h"
#include "seconfig.h"

#include <QTextDocumentFragment>
#include <QMessageBox>
#include <QStyle>
#include <QDir>
#include <QColorDialog>
#include <QTextTable>
#include <QTextCharFormat>
#include <QDebug>

FrameHTMLEditor::FrameHTMLEditor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FrameHTMLEditor)
{
    ui->setupUi(this);
    mHighlighter = new XmlHighlighter(ui->plainTextEdit->document());    
    ui->tabWidget->setCurrentIndex(0);
    mSection = 0;        
    initSignals();
}

FrameHTMLEditor::~FrameHTMLEditor()
{
    delete ui;
}

void FrameHTMLEditor::showEvent(QShowEvent *)
{
    ui->tabWidget->setCurrentIndex(0);
}

void FrameHTMLEditor::initSignals(){

    connect(ui->tabWidget, SIGNAL(currentChanged(int)),
            SLOT(onTabWidgetChanged(int)));
    connect(ui->fontComboBox, SIGNAL(currentFontChanged(QFont)),
            SLOT(setFontText(QFont)));
    connect(ui->spinBoxFontSize, SIGNAL(valueChanged(int)),
            SLOT(setFontSize(int)));
    connect(ui->pushButtonItalic, SIGNAL(clicked(bool)), ui->textEdit,
            SLOT(setFontItalic(bool)));    
    connect(ui->pushButtonUnderline, SIGNAL(clicked(bool)), ui->textEdit,
            SLOT(setFontUnderline(bool)));    
    connect(ui->pushButtonBold, SIGNAL(clicked(bool)), SLOT(setFontBold(bool)));
    connect(ui->pushButtonNumList, SIGNAL(clicked()), SLOT(setNumList()));
    connect(ui->pushButtonList, SIGNAL(clicked()), SLOT(setList()));
    connect(ui->pushButtonCut, SIGNAL(clicked()), ui->textEdit,
            SLOT(cut()));    
    connect(ui->pushButtonCopy, SIGNAL(clicked()), ui->textEdit,
            SLOT(copy()));    
    connect(ui->pushButtonPaste, SIGNAL(clicked()), ui->textEdit,
            SLOT(paste()));    
    connect(ui->pushButtonLink, SIGNAL(clicked()), SLOT(linkURL()));    
    connect(ui->pushButtonUnlink, SIGNAL(clicked()), SLOT(unLinkURL()));    
    connect(ui->pushButtonTextColor, SIGNAL(clicked()), SLOT(setTextColor()));
    connect(ui->pushButtonBackgroundColor, SIGNAL(clicked()),
            SLOT(setTextBackgroundColor()));
    connect(ui->pushButtonAlignmentLeft, SIGNAL(clicked()),
            SLOT(setAlignmentLeft()));
    connect(ui->pushButtonAlignmentRight, SIGNAL(clicked()),
            SLOT(setAlignmentRight()));
    connect(ui->pushButtonAlignmentCenter, SIGNAL(clicked()),
            SLOT(setAlignmentCenter()));
    connect(ui->pushButtonAlignmentJustify, SIGNAL(clicked()),
            SLOT(setAlignmentJustify()));
    connect(ui->pushButtonImage, SIGNAL(clicked()),
            SLOT(insertImage()));
    connect(ui->pushButtonAddTable,SIGNAL(clicked()),
            SLOT(addTable()));
    connect(ui->plainTextEdit, SIGNAL(textChanged()), SLOT(onDocumentChanged()));
    connect(ui->textEdit, SIGNAL(textChanged()), SLOT(onDocumentChanged()));
    connect(ui->pushButtonAddShift, SIGNAL(clicked()), SLOT(addShift()));
    connect(ui->pushButtonRemoveShift, SIGNAL(clicked()), SLOT(delShift()));
}

void FrameHTMLEditor::loadImages(const QString &)
{    
    /*
    QString s = unSetLocalURLImage(html);
    QRegExp exp;
    exp.setMinimal(true);
    exp.setPattern("<img[^>]*src=\"([^/]*)\"");
    int pos = exp.indexIn(s);
    QStringList list = exp.capturedTexts();
    */
}

const QString FrameHTMLEditor::getImageFileFromSelection(const QString &html)
{
    QString s = getSelectionFragment(html);
    if (s.isEmpty())
        return QString();

    QRegExp exp;
    exp.setPattern("^<img[^>]+src=\"(.*)\"[\\s]*/>$");
    exp.indexIn(s);
    QStringList list = exp.capturedTexts();
    if (!list.isEmpty())
        return list.at(list.size() - 1);
    return QString();
}

const QString FrameHTMLEditor::getSelectionFragment(const QString &html)
{
    QString startFragment("<!--StartFragment-->");
    QString endFragment("<!--EndFragment-->");
    int startI = html.indexOf(startFragment);
    if (startI <= 0)
        return QString();

    int endI = html.indexOf(endFragment);
    if (endI <= 0)
        return QString();

    QString s = html.mid(startI + startFragment.size(),
                         endI - startI - startFragment.size());
    return s;
}

void FrameHTMLEditor::setHTMLText(const QString &text)
{    
    disconnect(ui->plainTextEdit, SIGNAL(textChanged()));
    disconnect(ui->textEdit, SIGNAL(textChanged()));
    QString s = setLocalURLImage(text);
    ui->textEdit->document()->setHtml(s);
    loadImages(s);
    connect(ui->plainTextEdit, SIGNAL(textChanged()), SLOT(onDocumentChanged()));
    connect(ui->textEdit, SIGNAL(textChanged()), SLOT(onDocumentChanged()));
}

void FrameHTMLEditor::setSection(const int &section)
{
    mSection = section;
    mImagePath = SEConfig::getFolderImages(section);
    mImagePath.replace("\\", "/");
}

QString FrameHTMLEditor::text()
{
    return ui->textEdit->document()->toPlainText();
}

QString FrameHTMLEditor::textHTML()
{
    return transformHTML(ui->textEdit->document()->toHtml());
}

void FrameHTMLEditor::onTabWidgetChanged(int index)
{
    if (index == 0)
        ui->textEdit->setHtml(setLocalURLImage(ui->plainTextEdit->document()->toPlainText()));
    else ui->plainTextEdit->setPlainText(transformHTML(ui->textEdit->document()->toHtml()));
}

void FrameHTMLEditor::onDocumentChanged()
{
    emit documentChanged();
    emit modified();
    if (sender() == ui->textEdit)
        emit codeHTMLChanged(textHTML());
}

void FrameHTMLEditor::setFontText(const QFont &font)
{
    ui->textEdit->setFontFamily(font.family());
}

void FrameHTMLEditor::setFontSize(const int &size)
{
    ui->textEdit->setFontPointSize(size);
}

void FrameHTMLEditor::setFontBold(const bool &isBold)
{
    if (isBold)
        ui->textEdit->setFontWeight(75);
    else ui->textEdit->setFontWeight(0);
}

void FrameHTMLEditor::setTextColor()
{
    QColorDialog dialog(this);
    QColor color = dialog.getColor(Qt::white);
    ui->textEdit->setTextColor(color);
}

void FrameHTMLEditor::setTextBackgroundColor()
{
    QColorDialog dialog(this);
    QColor color = dialog.getColor(Qt::white);
    ui->textEdit->setTextBackgroundColor(color);
}

void FrameHTMLEditor::linkURL()
{
    DialogSetURL dlg;

    if (!ui->textEdit->textCursor().selectedText().isEmpty())
        dlg.setURLName(ui->textEdit->textCursor().selectedText());
    else ui->textEdit->cut();

    if (dlg.exec() == QDialog::Accepted) {
        ui->textEdit->insertHtml(dlg.URL());
        ui->textEdit->insertHtml("<span>&nbsp;</span>");
    }
}

void FrameHTMLEditor::unLinkURL()
{
    ui->textEdit->textCursor().clearSelection();

    QString s = selectionHTML();
    s.remove(QRegExp("<a href=[^>]*>"));
    s.remove("<\a>");
    s.remove("text-decoration: underline; color:#0000ff;");
    ui->textEdit->insertHtml(s);
}

void FrameHTMLEditor::setAlignmentLeft()
{
    ui->textEdit->setAlignment(Qt::AlignLeft);
}

void FrameHTMLEditor::setAlignmentRight()
{
    ui->textEdit->setAlignment(Qt::AlignRight);
}

void FrameHTMLEditor::setAlignmentCenter()
{
    ui->textEdit->setAlignment(Qt::AlignCenter);
}

void FrameHTMLEditor::setAlignmentJustify()
{
    ui->textEdit->setAlignment(Qt::AlignJustify);
}

void FrameHTMLEditor::insertImage()
{
    DialogInsertImage dlg;
    dlg.setSection(mSection);

    QTextCursor cursor = ui->textEdit->textCursor();
    QString imageFile = getImageFileFromSelection(cursor.selection().toHtml());
    bool isEditMode = !imageFile.isEmpty();    

    if (dlg.exec() == QDialog::Accepted) {
        if (!isEditMode)
            ui->textEdit->insertHtml(dlg.getURL());
        else {
            cursor.removeSelectedText();
            ui->textEdit->insertHtml(dlg.getURL());
        }
    }
}

void FrameHTMLEditor::addTable()
{
    DialogHTMLTable htmlTable;
    if (htmlTable.exec() == QDialog::Accepted) {  
        QTextCursor cursor = ui->textEdit->textCursor();
        QTextTable *table = cursor.insertTable(htmlTable.getRow(),
                                               htmlTable.getColum(),
                                               htmlTable.getFormat());

        for(int i = 0; i < htmlTable.getmRow().size(); i++){
            table->mergeCells(htmlTable.getmRow().at(i), htmlTable.getmColum().at(i),
                              htmlTable.getm_rowCount().at(i), htmlTable.getm_columCount().at(i));
        }

        QTextCharFormat form;
        int h = 0;
        for(int i = 0; i < htmlTable.getRow(); i++){
            for(int j = 0; j < htmlTable.getColum(); j++){
                form.setBackground(QBrush(htmlTable.getColor().at(h)));
                if(htmlTable.getm_align().at(h) == 32)
                    form.setVerticalAlignment(QTextCharFormat::AlignTop);
                if(htmlTable.getm_align().at(h) == 128)
                    form.setVerticalAlignment(QTextCharFormat::AlignMiddle);
                if(htmlTable.getm_align().at(h) == 64)
                    form.setVerticalAlignment(QTextCharFormat::AlignBottom);
                if(htmlTable.getm_align().at(h) == 256)
                    form.setVerticalAlignment(QTextCharFormat::AlignBaseline);
                table->cellAt(i,j).setFormat(form);
                h = h + 1;
            }
        }
    }
}

void FrameHTMLEditor::setNumList()
{
    if (ui->textEdit->textCursor().hasSelection())
        ui->textEdit->textCursor().createList(QTextListFormat::ListDecimal);
    else ui->textEdit->textCursor().insertList(QTextListFormat::ListDecimal);
}

void FrameHTMLEditor::setList()
{
    if (ui->textEdit->textCursor().hasSelection())
        ui->textEdit->textCursor().createList(QTextListFormat::ListDisc);
    else ui->textEdit->textCursor().insertList(QTextListFormat::ListDisc);
}

void FrameHTMLEditor::addShift()
{
    int currentIndent = (int) ui->textEdit->textCursor().blockFormat().leftMargin();
    QTextBlockFormat indentFormatting;
    indentFormatting.setLeftMargin(currentIndent + 40);
    ui->textEdit->textCursor().setBlockFormat(indentFormatting);
}

void FrameHTMLEditor::delShift()
{
    int currentIndent = (int) ui->textEdit->textCursor().blockFormat().leftMargin();
    QTextBlockFormat indentFormatting;
    if (currentIndent < 40)
        currentIndent = 40;
    indentFormatting.setLeftMargin(currentIndent - 40);
    ui->textEdit->textCursor().setBlockFormat(indentFormatting);
}

QString FrameHTMLEditor::transformHTML(const QString &text)
{
    QString s = text;

    QRegExp exp;
    // включаем жадный режим
    exp.setMinimal(true);
    exp.setPattern("^<!DOCTYPE.*>");
    s.remove(exp);
    exp.setPattern("<head.*</head>");
    s.remove(exp);
    exp.setPattern("<html.*>");
    s.remove(exp);
    exp.setPattern("<body.*>");
    s.remove(exp);
    exp.setPattern("</body></html>");
    s.remove(exp);
    s = s.trimmed();
    s = unSetLocalURLImage(s);
    s.replace(": ", ":");
    s.remove("-qt-paragraph-type:empty;");
    s.replace("  ", "");
    s.remove("-qt-block-indent:0;");
    s.replace("  ", "");
    s.remove("margin-top:0px;");
    s.replace("  ", "");
    s.remove("margin-bottom:0px;");
    s.replace("  ", "");
    s.remove("margin-top:12px;");
    s.replace("  ", "");
    s.remove("margin-bottom:12px;");
    s.replace("  ", "");
    s.remove("margin-left:0px;");
    s.replace("  ", "");
    s.remove("margin-right:0px;");
    s.replace("  ", "");
    s.remove("text-indent:0px;");
    s.replace("  ", "");
    s.remove("font-family:'MS Shell Dlg 2';");
    s.replace("  ", "");
    s.remove("font-size:8pt;");
    s.replace("  ", "");
    s.remove("color:#000000;");
    s.replace("  ", "");
    s.remove("-qt-list-indent:1;");
    s.replace("  ", "");
    s.remove("style=\"\"");
    s.replace("  ", "");
    s.remove("<p ><br /></p>");
    exp.setPattern("<span style=\"\\s*font-weight:600;\">(.*)</span>");
    if (exp.indexIn(s) != -1)
        if (exp.captureCount())
            s.replace(exp, "<strong>" + exp.cap(1) + "</strong>");
    exp.setPattern("<span style=\"\\s*font-style:italic;\">(.*)</span>");
    if (exp.indexIn(s) != -1)
        if (exp.captureCount())
            s.replace(exp, "<em>" + exp.cap(1) + "</em>");
    exp.setPattern("<span style=\"\\s*text-decoration: underline;\">(.*)</span>");
    if (exp.indexIn(s) != -1)
        if (exp.captureCount())
            s.replace(exp, "<u>" + exp.cap(1) + "</u>");
    exp.setPattern("<span style=\"\\s* font-weight:600; font-style:italic;\">(.*)</span>");
    if (exp.indexIn(s) != -1)
        if (exp.captureCount())
            s.replace(exp, "<strong><em>" + exp.cap(1) + "</em><strong>");

    return s.trimmed();
}

QString FrameHTMLEditor::setLocalURLImage(const QString &text)
{
    QString s = text;
    QRegExp exp;
    exp.setMinimal(true);
    exp.setPattern("(<img[^>]*src=\").*([^/]*\")");
    s.replace(exp, "\\1" + mImagePath + "/" + "\\2");

    return s;
}

QString FrameHTMLEditor::unSetLocalURLImage(const QString &text)
{
    QString s = text;
    QRegExp exp;
    exp.setMinimal(true);
    exp.setPattern("(<img[^>]*src=\").*([^/]*\")");
    s.replace(exp, "\\1\\2");
    return s;
}

QString FrameHTMLEditor::selectionHTML() const
{
    QString p;
    p = ui->textEdit->textCursor().selection().toHtml();

    QString startF = "<!--StartFragment-->";
    QString endF = "<!--EndFragment-->";

    int startFPos = p.indexOf(startF, 0);
    int endFPos = p.indexOf(endF, 0);    

    return p.mid(startFPos + startF.length(), endFPos - startFPos - endF.length() - 2);
}




