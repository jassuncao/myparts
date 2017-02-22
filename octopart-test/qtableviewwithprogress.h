#ifndef QTABLEVIEWWITHPROGRESS_H
#define QTABLEVIEWWITHPROGRESS_H

#include <QTableView>
#include <QColor>

class QTimer;

class QTableViewWithProgress : public QTableView
{
    Q_OBJECT
public:
    explicit QTableViewWithProgress(QWidget *parent = 0);

    /*! Returns the delay between animation steps.
        \return The number of milliseconds between animation steps. By default, the animation delay is set to 40 milliseconds.
        \sa setAnimationDelay
     */
    int animationDelay() const { return m_delay; }

    /*! Returns a Boolean value indicating whether the component is currently animated.
        \return Animation state.
        \sa startAnimation stopAnimation
     */
    bool isAnimated () const;

public slots:


    /*! Starts the spin animation.
        \sa stopAnimation isAnimated
     */
    void startAnimation();

    /*! Stops the spin animation.
        \sa startAnimation isAnimated
     */
    void stopAnimation();

    /*! Sets the delay between animation steps.
        Setting the \a delay to a value larger than 40 slows the animation, while setting the \a delay to a smaller value speeds it up.
        \param delay The delay, in milliseconds.
        \sa animationDelay
     */
    void setAnimationDelay(int delay);

    /*! Sets the color of the components to the given color.
        \sa color
     */
    void setColor(const QColor & color);
protected:
    virtual void timerEvent(QTimerEvent * event);
    virtual void paintEvent(QPaintEvent * event);
private slots:
    void updateProgress();
private:
    QTimer * _timer;
    int m_angle;
    int m_timerId;
    int m_delay;
    QColor m_color;
};

#endif // QTABLEVIEWWITHPROGRESS_H
