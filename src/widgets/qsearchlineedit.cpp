#include "qsearchlineedit.h"
#include <QtGui>

QSearchLineEdit::QSearchLineEdit(QWidget *parent) :
    QLineEdit(parent)
{
    QIcon searchIcon(":/icons/find");
    imageLabel = new QLabel(this);
    imageLabel->setPixmap(searchIcon.pixmap(16,16));
    clearButton = new QToolButton(this);
    QIcon icon(":/icons/edit-clear-location-rtl");
    clearButton->setIcon(icon);
    //clearButton->setIconSize(icon.);
    clearButton->setCursor(Qt::ArrowCursor);
    clearButton->setStyleSheet("QToolButton { border: none; padding: 0px; }");
    clearButton->hide();
    connect(clearButton, SIGNAL(clicked()), this, SLOT(clear()));
    connect(this, SIGNAL(textChanged(const QString&)), this, SLOT(updateClearButton(const QString&)));
    int frameWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
    //Rounded corners:
    //border: 2px solid gray; border-radius: 10px;
    QString style = QString("QLineEdit { padding-left: %1px; padding-right: %2px; } ")
            .arg(imageLabel->sizeHint().width()+frameWidth +1)
            .arg(clearButton->sizeHint().width() + frameWidth + 1);
    setStyleSheet(style);
    QSize msz = minimumSizeHint();
    setMinimumSize(qMax(msz.width(), clearButton->sizeHint().height() + frameWidth * 2 + 2),
                       qMax(msz.height(), clearButton->sizeHint().height() + frameWidth * 2 + 2));
}

void QSearchLineEdit::resizeEvent(QResizeEvent *)
{
    QSize sz = clearButton->sizeHint();
    int frameWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
    clearButton->move(rect().right() - frameWidth - sz.width(),
                      (rect().bottom() + 1 - sz.height())/2);

    int x = rect().x()+frameWidth + 2;
    int y = (rect().height()-16)/2;
    imageLabel->move(x,y);
}

void QSearchLineEdit::updateClearButton(const QString& text)
{
    clearButton->setVisible(!text.isEmpty());
}

