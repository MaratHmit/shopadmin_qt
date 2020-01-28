#ifndef DIALOGCOMPLAINT_H
#define DIALOGCOMPLAINT_H

#include <QDialog>

namespace Ui {
class DialogComplaint;
}

class DialogComplaint : public QDialog
{
    Q_OBJECT

public:
    explicit DialogComplaint(QWidget *parent = 0);
    ~DialogComplaint();
    void setInfo(const QString &info);

private slots:
    void on_pushButton_clicked();

private:
    Ui::DialogComplaint *ui;
};

#endif // DIALOGCOMPLAINT_H
