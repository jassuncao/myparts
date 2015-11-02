#include "qdualpushbutton.h"
#include <QIcon>
#include <QToolButton>
#include <QComboBox>
#include <QStyle>

QDualPushButton::QDualPushButton(QWidget *parent)
    :QComboBox(parent)
{
    clearButton = new QToolButton(this);   
    clearButton->setFixedSize(QSize(16,16));    
    clearButton->setCursor(Qt::ArrowCursor);
    clearButton->setStyleSheet("QToolButton { "
                               "border: none; "
                               "padding: 0px; "

                               "background: url(:/icons/close) center center no-repeat;"
                               "}"
                               "QToolButton:hover {"
                               "background: url(:/icons/close_hover) center center no-repeat;"
                               "}"
                               "QToolButton:pressed {"
                               "background: url(:/icons/close_pressed) center center no-repeat;"
                               "}");
    connect(clearButton, SIGNAL(clicked()), this, SLOT(slotClearText()));
    int frameWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
    //Rounded corners:
    //border: 2px solid gray; border-radius: 10px;
    QString style = QString("QPushButton { padding-right: %2px; outline: none; } ")
            .arg(clearButton->sizeHint().width() + frameWidth + 1);
    setStyleSheet(style);
    QSize msz = minimumSizeHint();
    setMinimumSize(qMax(msz.width(), clearButton->sizeHint().height() + frameWidth * 2 + 2),
                       qMax(msz.height(), clearButton->sizeHint().height() + frameWidth * 2 + 2));

}

QDualPushButton::~QDualPushButton()
{

}

void QDualPushButton::resizeEvent(QResizeEvent *)
{
    QSize sz = clearButton->size();
    int frameWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
    clearButton->move(rect().right() - frameWidth - sz.width(),
                      (rect().bottom() + 1 - sz.height())/2);
}



void QDualPushButton::slotClearText()
{
    qDebug("Clicked");
}

