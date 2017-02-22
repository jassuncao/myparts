#include "qtableviewwithprogress.h"
#include <QPainter>
#include <QTimerEvent>
#include <QTimer>

QTableViewWithProgress::QTableViewWithProgress(QWidget *parent)
    : QTableView(parent),
      m_angle(0),
      m_timerId(-1),
      m_delay(40),
      m_color(Qt::black)
{
    _timer = new QTimer(this);
    connect(_timer, SIGNAL(timeout()), this, SLOT(updateProgress()));
}

bool QTableViewWithProgress::isAnimated() const
{
    return _timer->isActive();
    //return (m_timerId != -1);
}

void QTableViewWithProgress::startAnimation()
{
    m_angle = 0;

    if(!_timer->isActive()){
        _timer->start(m_delay);
    }
    /*
    if (m_timerId == -1)
        m_timerId = startTimer(m_delay);
        */
}

void QTableViewWithProgress::stopAnimation()
{
    if(_timer->isActive()){
        _timer->stop();
    }
    /*
    if (m_timerId != -1)
        killTimer(m_timerId);

    m_timerId = -1;
*/
    update();
}

void QTableViewWithProgress::setAnimationDelay(int delay)
{
    if(_timer->isActive()){
        _timer->stop();
    }
    /*
    if (m_timerId != -1)
        killTimer(m_timerId);
*/
    m_delay = delay;
    _timer->start(m_delay);
/*
    if (m_timerId != -1)
        m_timerId = startTimer(m_delay);
        */
}

void QTableViewWithProgress::setColor(const QColor & color)
{
    m_color = color;
    update();
}

void QTableViewWithProgress::updateProgress()
{
    qDebug("time");
    m_angle = (m_angle+30)%360;
    update();
}

void QTableViewWithProgress::timerEvent(QTimerEvent * event)
{
    QTableView::timerEvent(event);
    /*
    if(event->timerId() == m_timerId){
        qDebug("time");
        m_angle = (m_angle+30)%360;
        update();
    }
    */
}

void QTableViewWithProgress::paintEvent(QPaintEvent *e)
{
    QTableView::paintEvent(e); // draw original content


    if(isAnimated()){
        qDebug("animate");
        QWidget * canvas = this->viewport();
        int width = qMin(canvas->width(), canvas->height());

        QPainter p(canvas);
        p.setRenderHint(QPainter::Antialiasing);

        int outerRadius = (width-1)*0.5;
        int innerRadius = (width-1)*0.5*0.38;

        int capsuleHeight = outerRadius - innerRadius;
        int capsuleWidth  = (width > 32 ) ? capsuleHeight *.23 : capsuleHeight *.35;
        int capsuleRadius = capsuleWidth/2;

        for (int i=0; i<12; i++)
        {
            QColor color = m_color;
            color.setAlphaF(1.0f - (i/12.0f));
            p.setPen(Qt::NoPen);
            p.setBrush(color);
            p.save();
            p.translate(rect().center());
            p.rotate(m_angle - i*30.0f);
            p.drawRoundedRect(-capsuleWidth*0.5, -(innerRadius+capsuleHeight), capsuleWidth, capsuleHeight, capsuleRadius, capsuleRadius);
            p.restore();
        }
    }
}


