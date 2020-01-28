#ifndef PAGENAVIGATOR_H
#define PAGENAVIGATOR_H

#include <QWidget>

namespace Ui {
class FramePageNavigator;
}

class FramePageNavigator : public QWidget
{
    Q_OBJECT

public:
    explicit FramePageNavigator(QWidget *parent = 0);
    ~FramePageNavigator();

    void setNumRecord(int num, int count);
    int getLimit();
    void setLimit(const int &getLimit);
    void setEnabledLimit(const bool &enabled);
    void setVisibleButtons(const bool &first, const bool &prev, const bool &next, const bool &last);
    void setVisibleAllSelectButton(const bool &isVisible);
    bool isCheckedAllSelected();
    void disconnectSelected();
    void connectSelected();

public slots:
    void setUnchekedSelectButton();
    void setSelectedCount(int count);

signals:
    void goNextPage();
    void goPreviosPage();
    void goFirstPage();
    void goLastPage();    
    void refreshDataStart();
    void selectAll(bool checked);

private slots:
    void on_btnNext_clicked();
    void on_btnPrevios_clicked();
    void on_btnLast_clicked();
    void on_btnFirst_clicked();
    void on_spinBox_editingFinished();    

private:
    Ui::FramePageNavigator *ui;
};

#endif // PAGENAVIGATOR_H
