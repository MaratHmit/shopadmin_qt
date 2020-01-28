#include "preloader.h"

#include <QPainter>

Preloader::Preloader(QWidget* parent)
        : QWidget(parent),
        angle(0), timerId(-1), delay(30), sizePreloader(30),
        isDisplayedWhenStopped(false)
{
    parentWidget = parent;
    color.setRgb(73, 104, 166);
    setVisible(0);
    setAttribute(Qt::WA_TranslucentBackground);    
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setFocusPolicy(Qt::NoFocus);
}


bool Preloader::isAnimated () const
{
    return (timerId != -1);
}

void Preloader::setDisplayedWhenStopped(bool state)
{
    isDisplayedWhenStopped = state;
    update();
}

bool Preloader::getIsDisplayedWhenStopped() const
{
    return isDisplayedWhenStopped;
}

void Preloader::startAnimation()
{
    setPosition();    
    raise();    
    show();

    angle = 0;
    if (timerId == -1)
        timerId = startTimer(delay);
}

void Preloader::stopAnimation()
{    
    if (timerId != -1)
        killTimer(timerId);

    timerId = -1; 
    close();
}

void Preloader::setAnimationDelay(int delay)
{
    if (timerId != -1)
        killTimer(timerId);

    this->delay = delay;

    if (timerId != -1)
        timerId = startTimer(delay);
}


QSize Preloader::sizeHint() const
{
    return QSize(sizePreloader, sizePreloader);
}

int Preloader::heightForWidth(int w) const
{
    return w;
}

void Preloader::timerEvent(QTimerEvent *)
{
    angle = (angle + 30) % 360;
    update();
}

void Preloader::paintEvent(QPaintEvent *)
{
    if (!isDisplayedWhenStopped && !isAnimated())
        return;

    int width = qMin(this->width(), this->height());
    setPosition();

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);


    int outerRadius = (width - 1)*0.5;
    int innerRadius = (width - 1)*0.5*0.38;

    int capsuleHeight = outerRadius - innerRadius;
    int capsuleWidth  = (width > 32 ) ? capsuleHeight *.23 : capsuleHeight *.35;
    int capsuleRadius = capsuleWidth / 2;

    for (int i = 0; i < 12; i++)
    {
        color.setAlphaF(1.0f - (i/12.0f));
        p.setPen(Qt::NoPen);
        p.setBrush(color);
        p.save();
        p.translate(rect().center());
        p.rotate(angle - i*30.0f);
        p.drawRoundedRect(-capsuleWidth*0.5, -(innerRadius+capsuleHeight),
                          capsuleWidth, capsuleHeight, capsuleRadius, capsuleRadius);
        p.restore();
    }
}

void Preloader::setPosition()
{
    if (parentWidget) {
      int x, y;

      x = (parentWidget->width() - sizePreloader) / 2;
      y = (parentWidget->height() - sizePreloader) / 2;
      setGeometry(x, y, sizePreloader, sizePreloader);
    }
}
