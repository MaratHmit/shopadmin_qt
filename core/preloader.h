#ifndef PRELOADER_H
#define PRELOADER_H

#include <QWidget>
#include <QColor>

class Preloader : public QWidget
{
    Q_OBJECT

    Q_PROPERTY(int delay READ animationDelay WRITE setAnimationDelay)
    Q_PROPERTY(bool displayedWhenStopped READ getIsDisplayedWhenStopped WRITE setDisplayedWhenStopped)  

public:
    Preloader(QWidget* parent = 0);

    int animationDelay() const { return delay; }
    bool isAnimated () const;
    bool getIsDisplayedWhenStopped() const;    
    virtual QSize sizeHint() const;
    int heightForWidth(int w) const;

public slots:
    void startAnimation();
    void stopAnimation();
    void setAnimationDelay(int delay);
    void setDisplayedWhenStopped(bool state);    

protected:
    virtual void timerEvent(QTimerEvent * event);
    virtual void paintEvent(QPaintEvent * event);

private:
    int angle;
    int timerId;
    int delay;
    int sizePreloader;
    QWidget *parentWidget;
    bool isDisplayedWhenStopped;    

    QColor color;

    void setPosition();
};

#endif // PRELOADER_H
