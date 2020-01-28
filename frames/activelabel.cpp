#include "activelabel.h"

ActiveLabel::ActiveLabel(QWidget *parent) :
    QLabel(parent)
{
}

void ActiveLabel::mouseDoubleClickEvent(QMouseEvent *)
{
    emit doubleClicked();
}
