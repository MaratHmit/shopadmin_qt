#include "dialoghtmltable.h"
#include "ui_dialoghtmltable.h"

#include <QColorDialog>

DialogHTMLTable::DialogHTMLTable(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogHTMLTable)
{
    ui->setupUi(this);
    pwi = 0;
    mRow = 3;
    mColumn = 3;
    ui->tableWidgetHTMLtable->setRowCount(mRow);
    ui->tableWidgetHTMLtable->setColumnCount(mColumn);
    ui->lineEditWidthTable->setText("100");
    ui->lineEditCellSpassing->setText("0");

     createTable();
     background();
     textAlign();
     setbackground();
}

DialogHTMLTable::~DialogHTMLTable()
{
    delete ui;
}

int DialogHTMLTable::getRow()
{
    return mRow;
}

int DialogHTMLTable::getColum()
{
    return mColumn;
}


QVector<int> DialogHTMLTable::getmRow()
{
    return m_Row;
}

QVector<int> DialogHTMLTable::getmColum()
{
    return m_Colum;
}

QVector<int> DialogHTMLTable::getm_rowCount()
{
    return m_rowCount;
}

QVector<int> DialogHTMLTable::getm_columCount()
{
    return m_columCount;
}

QVector<int> DialogHTMLTable::getm_align()
{
    return m_align;
}

QVector<QColor> DialogHTMLTable::getColor()
{
    return mBackground;
}

QTextTableFormat DialogHTMLTable::getFormat()
{
    return format;
}

void DialogHTMLTable::createTable()
{
    for (int i = 0; i < mRow; i++){
        for(int j = 0; j < mColumn; j++){
            pwi = new QTableWidgetItem(QString("").arg(i).arg(j));
            ui->tableWidgetHTMLtable->setItem(i, j ,pwi);
        }
    }
}

void DialogHTMLTable::textAlign()
{
    m_align.clear();
    for(int i = 0; i < mRow; i++){
        for(int j = 0; j < mColumn; j++)
            m_align << ui->tableWidgetHTMLtable->item(i,j)->textAlignment();
    }
}

void DialogHTMLTable::background()
{
    for(int i = 0; i < mRow; i++){
        for (int j = 0; j < mColumn; j++)
            ui->tableWidgetHTMLtable->item(i,j)->setBackground(QBrush(QColor("#ffffff")));
    }
}

void DialogHTMLTable::setbackground()
{
    mBackground.clear();
    for(int i = 0; i < mRow; i++){
        for(int j = 0; j<mColumn; j++)
            mBackground<<ui->tableWidgetHTMLtable->item(i,j)->backgroundColor();
    }
}

void DialogHTMLTable::on_spinBoxRowCount_valueChanged(const QString &arg1)
{
    mRow = arg1.toInt();
    ui->tableWidgetHTMLtable->setRowCount(mRow);
    createTable();
    background();
    textAlign();
    setbackground();
}

void DialogHTMLTable::on_spinBoxColumCount_valueChanged(const QString &arg1)
{
    mColumn = arg1.toInt();
    ui->tableWidgetHTMLtable->setColumnCount(mColumn);
    createTable();
    background();
    textAlign();
    setbackground();
}

void DialogHTMLTable::on_pushButtonBackgroundColor_clicked()
{
    QColorDialog dialog(this);
    dialog.setWindowTitle("Выбор Цвет Заливки");
    QColor col = dialog.getColor(Qt::white);
    QVariant c = col;
    ui->lineEditBackgroundColor->setText(c.toString());
}

void DialogHTMLTable::on_lineEditBackgroundColor_textChanged(const QString &arg1)
{
    for(int i = 0; i < mRow; i++){
        for (int j = 0; j < mColumn; j++)
            ui->tableWidgetHTMLtable->item(i,j)->setBackground(QBrush(QColor(arg1)));
    }
   //format.setBackground(QBrush(QColor(arg1)));
    setbackground();
}

void DialogHTMLTable::on_lineEditSetWidth_textChanged(const QString &arg1)
{
     format.setWidth(QTextLength(QTextLength::PercentageLength,arg1.toInt()));
}

void DialogHTMLTable::on_pushButtonBorderColor_clicked()
{
    QColorDialog dialog(this);
    dialog.setWindowTitle("Выбор Цвета Границы");
    QColor col = dialog.getColor(Qt::white);
    QVariant c = col;
    ui->lineEditBorderColor->setText(c.toString());
}

void DialogHTMLTable::on_lineEditBorderColor_textChanged(const QString &arg1)
{
    format.setBorderBrush(QBrush(QColor(arg1)));
}

void DialogHTMLTable::on_lineEditCellpadding_textChanged(const QString &arg1)
{
    format.setCellPadding(arg1.toInt());
}

void DialogHTMLTable::on_lineEditCellSpassing_textChanged(const QString &arg1)
{
    format.setCellSpacing(arg1.toInt());
}

void DialogHTMLTable::on_comboBoxAligen_currentIndexChanged(int index)
{
    switch (index) {
    case 0:
            format.setAlignment(Qt::AlignLeft);
        break;
    case 1: format.setAlignment(Qt::AlignCenter);
        break;
    case 2: format.setAlignment(Qt::AlignRight);
        break;
    default:
        break;
    }
}

void DialogHTMLTable::on_pushButtonSetSpan_clicked()
{
    int x1 = mColumn,y1 = mRow,x2 = 0, y2 = 0,w = 0,h = 0;
    QList<QTableWidgetItem*> list = ui->tableWidgetHTMLtable->selectedItems();
    for(int i = 0; i < list.size(); i++){
        QTableWidgetItem* item = list.at(i);
        if(item->column() < x1){

            x1 = item->column();
        }
        if(item->row() < y1) y1 = item->row();
        if(item->column() > x2) x2 = item->column();
        if(item->row() > y2) y2 = item->row();
    }

    w = x2 - x1 + 1;
    h = y2 - y1 + 1;

    ui->tableWidgetHTMLtable->setSpan(y1,x1,h,w);

    m_Row << y1;
    m_Colum << x1;
    m_rowCount << h;
    m_columCount <<w;
}

void DialogHTMLTable::on_lineEditBorder_textChanged(const QString &arg1)
{
    format.setBorder(arg1.toInt());
}

void DialogHTMLTable::on_comboBoxTextValign_currentIndexChanged(int index)
{
    switch (index) {
    case 0:
            mVertikal = 32;
        break;
    case 1: mVertikal = 128;
        break;
    case 2: mVertikal = 64;
        break;
    case 3: mVertikal = 256;
        break;
    default:
        break;
    }
    int x1 = 5,y1 = 5,x2 = 0, y2 = 0;
    QList<QTableWidgetItem*> list = ui->tableWidgetHTMLtable->selectedItems();
    for(int i = 0; i < list.size(); i++){
        QTableWidgetItem* item = list.at(i);
        if(item->column() < x1){

            x1 = item->column();
        }
        if(item->row() < y1) y1 = item->row();
        if(item->column() > x2) x2 = item->column();
        if(item->row() > y2) y2 = item->row();
    }



    for(int i = y1; i <= y2; i++){
    for(int j = x1; j <= x2; j++){
        ui->tableWidgetHTMLtable->item(i,j)->setTextAlignment(mVertikal);
    }
    }
    textAlign();
}

void DialogHTMLTable::on_pushButtonCellbackground_clicked()
{
    QColorDialog dialog(this);    
    QColor col = dialog.getColor(Qt::white);
    QVariant c = col;
    ui->lineEditCellBackground->setText(c.toString());
}

void DialogHTMLTable::on_lineEditCellBackground_textChanged(const QString &arg1)
{
    int x1 = 5,y1 = 5,x2 = 0, y2 = 0;
    QList<QTableWidgetItem*> list = ui->tableWidgetHTMLtable->selectedItems();
    for(int i = 0; i < list.size(); i++){
        QTableWidgetItem* item = list.at(i);
        if(item->column() < x1){

            x1 = item->column();
        }
        if(item->row() < y1) y1 = item->row();
        if(item->column() > x2) x2 = item->column();
        if(item->row() > y2) y2 = item->row();
    }



    for(int i = y1; i <= y2; i++){
    for(int j = x1; j <= x2; j++){
        ui->tableWidgetHTMLtable->item(i,j)->setBackground(QBrush(QColor(arg1)));
    }
    }
    setbackground();
}
