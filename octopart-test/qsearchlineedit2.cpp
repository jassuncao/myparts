#include "qsearchlineedit2.h"
#include <QLabel>
#include <QIcon>
#include <QToolButton>
#include <QStyle>
#include <QKeyEvent>
#include <QProgressIndicator>

QSearchLineEdit2::QSearchLineEdit2(QWidget *parent) :
    QLineEdit(parent)
{
    _searchPixmap.load(QString(":/icons/find"));
    imageLabel = new QLabel(this);
    imageLabel->setPixmap(_searchPixmap);

    clearButton = new QToolButton(this);
    QIcon icon(":/icons/edit-clear-locationbar-rtl");
    clearButton->setIcon(icon);
    clearButton->setCursor(Qt::ArrowCursor);
    clearButton->setStyleSheet("QToolButton { border: none; padding: 0px; }");
    clearButton->hide();

    _progressIndicator = new QProgressIndicator(this);
    _progressIndicator->hide();

    connect(clearButton, SIGNAL(clicked()), this, SLOT(slotClearText()));
    connect(this, SIGNAL(textChanged(const QString&)), this, SLOT(updateClearButton(const QString&)));
    int frameWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth);

    //Rounded corners:
    //border: 2px solid gray; border-radius: 10px;
    /*
    QString style = QString("QLineEdit { padding-left: %1px; padding-right: %2px; } ")
            .arg(imageLabel->sizeHint().width() + frameWidth + 1)
            .arg(clearButton->sizeHint().width() + frameWidth + 1);
    setStyleSheet(style);
    */

    int leftMargin, topMargin, rightMargin, bottomMargin;
    getTextMargins(&leftMargin, &topMargin, &rightMargin, &bottomMargin);
    leftMargin+=imageLabel->sizeHint().width();
    rightMargin+=clearButton->sizeHint().width();
    setTextMargins(leftMargin, topMargin, rightMargin, bottomMargin);

    QSize msz = minimumSizeHint();
    setMinimumSize(qMax(msz.width(), clearButton->sizeHint().height() + frameWidth * 2 + 2),
                       qMax(msz.height(), clearButton->sizeHint().height() + frameWidth * 2 + 2));

    _progressIndicator->setMinimumSize(imageLabel->minimumSize());
}

void QSearchLineEdit2::resizeEvent(QResizeEvent *)
{
    const QRect contentRect = rect();
    QSize sz = clearButton->sizeHint();
    int frameWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth);

    clearButton->move(contentRect.right() - frameWidth - sz.width(),
                      (contentRect.bottom() + 1 - sz.height())/2);

    int x = contentRect.x()+frameWidth + 2;
    int y = (contentRect.height()-16)/2;
    imageLabel->move(x,y);
    _progressIndicator->move(x,y);
}

void QSearchLineEdit2::updateClearButton(const QString& text)
{
    clearButton->setVisible(!text.isEmpty());
}

void QSearchLineEdit2::slotClearText()
{
    clear();
    emit textClear();
}

void QSearchLineEdit2::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape)
    {
        slotClearText();
    }
    QLineEdit::keyPressEvent(event);
}

void QSearchLineEdit2::setBusy(bool busy)
{
    if(busy){
        _progressIndicator->show();
        imageLabel->hide();
        _progressIndicator->startAnimation();
    }
    else{
        _progressIndicator->hide();
        imageLabel->show();
        _progressIndicator->stopAnimation();
    }
}


