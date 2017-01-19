#include "hintlineedit.h"
#include <QIcon>
#include <QToolButton>
#include <QStyle>

HintLineEdit::HintLineEdit(QWidget *parent) :
    QLineEdit(parent)
{
    _hintButton = new QToolButton(this);
    QIcon icon(":/icons/information");
    _hintButton->setIcon(icon);
    _hintButton->setCursor(Qt::ArrowCursor);
    _hintButton->setStyleSheet("QToolButton { border: none; padding: 0px; }");
    _hintButton->setVisible(true);

    connect(_hintButton, SIGNAL(clicked()), this, SLOT(slotShowHint()));
    int frameWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
    //Rounded corners:
    //border: 2px solid gray; border-radius: 10px;
    QString style = QString("QLineEdit { padding-right: %2px; } ")
            .arg(_hintButton->sizeHint().width() + frameWidth + 1);
    setStyleSheet(style);
    QSize msz = minimumSizeHint();
    setMinimumSize(qMax(msz.width(), _hintButton->sizeHint().height() + frameWidth * 2 + 2),
                       qMax(msz.height(), _hintButton->sizeHint().height() + frameWidth * 2 + 2));
}

void HintLineEdit::resizeEvent(QResizeEvent *)
{
    QSize sz = _hintButton->sizeHint();
    int frameWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
    _hintButton->move(rect().right() - frameWidth - sz.width(),
                      (rect().bottom() + 1 - sz.height())/2);
}


void HintLineEdit::slotShowHint()
{
}
