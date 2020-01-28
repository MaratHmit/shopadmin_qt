#ifndef ACTIVELABEL_H
#define ACTIVELABEL_H

#include <QLabel>

class ActiveLabel : public QLabel
{
    Q_OBJECT
public:
    explicit ActiveLabel(QWidget *parent = 0);

signals:
    void doubleClicked();

protected:
    void mouseDoubleClickEvent(QMouseEvent * e);

};

#endif // ACTIVELABEL_H
