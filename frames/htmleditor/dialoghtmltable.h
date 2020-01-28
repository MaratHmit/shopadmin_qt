#ifndef DIALOGHTMLTABLE_H
#define DIALOGHTMLTABLE_H

#include <QDialog>
#include <QTableWidgetItem>
#include <QTextTableFormat>
#include <QTextTable>
namespace Ui {
class DialogHTMLTable;
}

class DialogHTMLTable : public QDialog
{
    Q_OBJECT

public:
    explicit DialogHTMLTable(QWidget *parent = 0);
    ~DialogHTMLTable();

    int getRow();
    int getColum();

    QVector<int> getmRow();
    QVector<int> getmColum();
    QVector<int> getm_rowCount();
    QVector<int> getm_columCount();
    QVector<int> getm_align();
    QVector<QColor> getColor();
    QTextTableFormat getFormat();

private slots:
    void on_spinBoxRowCount_valueChanged(const QString &arg1);
    void on_spinBoxColumCount_valueChanged(const QString &arg1);
    void on_pushButtonBackgroundColor_clicked();
    void on_lineEditBackgroundColor_textChanged(const QString &arg1);
    void on_lineEditSetWidth_textChanged(const QString &arg1);
    void on_pushButtonBorderColor_clicked();
    void on_lineEditBorderColor_textChanged(const QString &arg1);
    void on_lineEditCellpadding_textChanged(const QString &arg1);
    void on_lineEditCellSpassing_textChanged(const QString &arg1);
    void on_comboBoxAligen_currentIndexChanged(int index);
    void on_pushButtonSetSpan_clicked();
    void on_lineEditBorder_textChanged(const QString &arg1);
    void on_comboBoxTextValign_currentIndexChanged(int index);
    void on_pushButtonCellbackground_clicked();
    void on_lineEditCellBackground_textChanged(const QString &arg1);

private:
    Ui::DialogHTMLTable *ui;
    int mRow;
    int mColumn;
    int mVertikal;

    QVector<int> m_Row;
    QVector<int> m_Colum;
    QVector<int> m_rowCount;
    QVector<int> m_columCount;
    QVector<int> m_align;

    QVector<QColor> mBackground;
    QTableWidgetItem *pwi;
    QTextTableFormat format;

    void createTable();
    void textAlign();
    void background();
    void setbackground();

};

#endif // DIALOGHTMLTABLE_H
