#include "frametabshtmleditor.h"
#include "ui_frametabshtmleditor.h"

#include "seconfig.h"
#include "seconsts.h"

#include <QLabel>
#include <QLineEdit>
#include <QToolButton>


FrameTabsHTMLEditor::FrameTabsHTMLEditor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FrameTabsHTMLEditor)
{
    ui->setupUi(this);    
    mSection = SEConsts::Section_Products;
    addTabHTMLEditor(tr("Описание товара"), QString(), false); 
}

FrameTabsHTMLEditor::~FrameTabsHTMLEditor()
{
    delete ui;
}

void FrameTabsHTMLEditor::setHTMLText(const QString &text)
{
    mHTMLText = text;
    createTabs();
}

void FrameTabsHTMLEditor::setSection(const int &section)
{
    mSection = section;
}

QString FrameTabsHTMLEditor::text()
{
    if (ui->tabWidget->count() == 1 && mEditors.size())
        return mEditors.at(0)->text();
    return QString();
}

QString FrameTabsHTMLEditor::textHTML()
{
    QString result;
    QString startTag("<tab title=");
    QString endTag("</tab>");
    for (int i = 0; i < ui->tabWidget->count(); i++) {
        result += startTag;
        result += "\"" + ui->tabWidget->tabText(i) + "\">";
        if (mEditors.size() > i)
            result += mEditors.at(i)->textHTML();
        result += endTag;
    }
    return result;
}

void FrameTabsHTMLEditor::refreshSourceText()
{
    for (int i = 0; i < mEditors.size(); i++)
        mEditors.at(i)->refreshSourceText();
}

void FrameTabsHTMLEditor::addTabHTMLEditor(const QString &name, const QString &html, const bool &isDeleted)
{
    QVBoxLayout *layout = new QVBoxLayout();
    QHBoxLayout *layoutTop = new QHBoxLayout();
    QLabel *label = new QLabel(tr("Наименование вкладки: "));
    layoutTop->addWidget(label);
    QLineEdit *editName = new QLineEdit();
    editName->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    editName->setText(name);
    layoutTop->addWidget(editName);
    layoutTop->addStretch();
    QToolButton *buttonAdd = new QToolButton();
    buttonAdd->setIcon(QIcon(":/16/icons/16x16/add.png"));
    buttonAdd->setToolTip(tr("Добавить новую вкладку"));
    layoutTop->addWidget(buttonAdd);
    if (isDeleted) {
        QToolButton *buttonDel = new QToolButton();
        layoutTop->addWidget(buttonDel);
        buttonDel->setIcon(QIcon(":/16/icons/16x16/Delete.png"));
        buttonDel->setToolTip(tr("Удалить вкладку"));
        connect(buttonDel, &QAbstractButton::clicked, this, &FrameTabsHTMLEditor::onDelTab);
    }
    layout->addLayout(layoutTop);
    FrameWYSIWYG *editor = new FrameWYSIWYG(ui->tabWidget);
    editor->setSection(mSection);
    connect(editor, &FrameWYSIWYG::modified, this, &FrameTabsHTMLEditor::modified);
    connect(editor, &FrameWYSIWYG::modified, this, &FrameTabsHTMLEditor::documentChanged);    
    mEditors.append(editor);
    editor->setHTMLText(html);
    layout->addWidget(editor);
    layout->setMargin(4);
    QWidget *w = new QWidget(ui->tabWidget);
    editName->setParent(w);

    w->setLayout(layout);
    ui->tabWidget->addTab(w, name);
    ui->tabWidget->setCurrentIndex(ui->tabWidget->count() - 1);
    editName->setFocus();    
    connect(editName, &QLineEdit::textEdited, this, &FrameTabsHTMLEditor::onChangeTabName);
    connect(buttonAdd, &QAbstractButton::clicked, this, &FrameTabsHTMLEditor::onAddTab);    
}

void FrameTabsHTMLEditor::createTabs()
{    
    deleteTabs();
    QString text(mHTMLText);
    QString startTag("<tab title=");
    QString endTag("</tab>");    
    QString title;
    if (!text.isEmpty()) {        
        int i = 0;
        int k = 0;
        int n = text.indexOf(endTag);
        if (n >= 0) {
            mEditors.clear();
            ui->tabWidget->clear();
            QString subStr;
            do {
                title = "-";
                subStr = text.mid(startTag.length(), n - startTag.length());
                k = subStr.indexOf(">");
                if (k > -1) {
                    title = subStr.mid(1, k - 2);
                    subStr.remove(0, k + 1);
                }
                addTabHTMLEditor(title, subStr, i);
                text.remove(0, n + endTag.size());                
                n = text.indexOf(endTag);
                i++;
            } while (n > -1);
        } else
            if (mEditors.count())
                mEditors.at(0)->setHTMLText(mHTMLText);
    }
    ui->tabWidget->setCurrentIndex(0);
}

void FrameTabsHTMLEditor::deleteTabs()
{
    for (int i = ui->tabWidget->count() - 1; i > 0; --i) {
        mEditors.removeAt(i);
        ui->tabWidget->removeTab(i);
    }
    if (mEditors.count())
        mEditors.at(0)->setHTMLText();
}

void FrameTabsHTMLEditor::onChangeTabName(const QString &name)
{
    int index = ui->tabWidget->indexOf(dynamic_cast <QWidget *> (sender()->parent()));
    if (index >= 0 && index < ui->tabWidget->count())
        ui->tabWidget->setTabText(index, name);
    emit modified();
    emit documentChanged();
}

void FrameTabsHTMLEditor::onAddTab()
{
    addTabHTMLEditor("Новая вкладка");
    emit modified();
}

void FrameTabsHTMLEditor::onDelTab()
{
    mEditors.removeAt(ui->tabWidget->currentIndex());
    ui->tabWidget->removeTab(ui->tabWidget->currentIndex());
    emit modified();
    emit documentChanged();
}


