#ifndef FRAMESEARCHTABLE_H
#define FRAMESEARCHTABLE_H

#include <QWidget>

namespace Ui {
class FrameSearchTable;
}

class FrameSearchTable : public QWidget
{
    Q_OBJECT

signals:
  void runSearch();

public:
    explicit FrameSearchTable(QWidget *parent = 0);
    ~FrameSearchTable();

    QString searchText() const;
    void setSearchText(const QString &searchText);
    void setIsHotSearchMode();
    void setFocus();

private slots:
  void on_toolButtonFind_clicked();
  void on_toolButtonClear_clicked();
  void onChangeText(const QString &);

private:
    Ui::FrameSearchTable *ui;
    bool mIsHotSearchMode;
    QString mLastText;

};

#endif // FRAMESEARCHTABLE_H
