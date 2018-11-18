#include "qsearchlineedit.h"

#include <QLabel>
#include <QIcon>
#include <QToolButton>
#include <QStyle>
#include <QKeyEvent>
#include <QPainter>
#include <QDebug>
#include "qprogressindicator.h"

static const int FIXED_HMARGIN = 2;
static const int FIXED_VMARGIN = 1;

QSearchLineEdit::QSearchLineEdit(QWidget *parent) :
    QLineEdit(parent)
{
    const int focusFrameHMargin = style()->pixelMetric(QStyle::PM_FocusFrameHMargin);
    const int focusFrameVMargin = style()->pixelMetric(QStyle::PM_FocusFrameVMargin);

    const int hMargin = FIXED_HMARGIN + focusFrameHMargin;
    const int vMargin = FIXED_VMARGIN + focusFrameVMargin;

    _styleMargins = QSize(hMargin, vMargin);

    _clearButton = new QToolButton(this);
    _clearButton->setCursor(Qt::ArrowCursor);
    _clearButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
    _clearButton->setStyleSheet("QToolButton {border: none; padding: 0px; margin: 0px; }");
    _clearButton->hide();
    connect(_clearButton, SIGNAL(clicked()), this, SLOT(slotClearText()));
    connect(this, SIGNAL(textChanged(const QString&)), this, SLOT(updateClearButton(const QString&)));
#ifdef WITH_PROGRESS
    _progressIndicator = new QProgressIndicator(this);
    _progressIndicator->setDisplayedWhenStopped(false);
    _progressIndicator->setFixedSize(20,20);
#endif
}

QIcon QSearchLineEdit::searchIcon() const
{
   return _searchIcon;
}

void QSearchLineEdit::setSearchIcon(const QIcon &icon)
{
    _searchIcon = icon;
    updateSearchIconGeometry();
    updateTextMargins();
}

QIcon QSearchLineEdit::clearIcon() const
{
   return _clearButton->icon();
}

void QSearchLineEdit::setClearIcon(const QIcon &icon)
{
    _clearButton->setIcon(icon);
    if(!icon.isNull() && icon.availableSizes().size() > 0){
        QSize iconSize = icon.availableSizes().first();
        _clearButton->setMaximumSize(iconSize);
    }
    updateTextMargins();
}

#ifdef WITH_PROGRESS
QProgressIndicator *QSearchLineEdit::progressIndicator() const
{
    return _progressIndicator;
}
#endif

void QSearchLineEdit::updateTextMargins()
{
    int leftMargin = FIXED_HMARGIN;
    int rightMargin = FIXED_HMARGIN;

    const int topMargin = 0;
    const int bottomMargin = 0;
    int progressIndicatorWidth = 0;
    int searchIconWidth = _searchIcon.isNull() ? 0 : _searchIcon.availableSizes().first().width();
#ifdef WITH_PROGRESS
    progressIndicatorWidth = _progressIndicator->size().width();
#endif

    leftMargin += qMax(progressIndicatorWidth, searchIconWidth);
    rightMargin += _clearButton->width();

    setTextMargins(leftMargin, topMargin, rightMargin, bottomMargin);
}

void QSearchLineEdit::updateSearchIconGeometry()
{

    if(!_searchIcon.isNull() && !_searchIcon.availableSizes().isEmpty()){
        _searchIconSize = _searchIcon.availableSizes().first();
    }
    else{
        _searchIconSize = QSize();
    }
}

QSize QSearchLineEdit::minimumSizeHint() const
{
    const QSize thisSize = QLineEdit::minimumSizeHint();
    const QSize buttonSize = _clearButton->size();

    const int vMargin =  + _styleMargins.height() * 2 ;
    int h = qMax(thisSize.height(), buttonSize.height() +vMargin );

    h = qMax(h, _searchIconSize.height() + vMargin);

#ifdef WITH_PROGRESS
    const QSize indicatorSize = _progressIndicator->size();
    h = qMax(h, indicatorSize.height() + vMargin);
#endif


    return QSize(thisSize.width(), h);
}



void QSearchLineEdit::paintEvent(QPaintEvent * event)
{
    QLineEdit::paintEvent(event);    
    if (!_searchIcon.isNull() && !isBusy()) {

        const int x = _styleMargins.width();
        const int y = 0;
        const int w = _searchIconSize.width();
        const int h = contentsRect().height();
        QPainter painter(this);
        _searchIcon.paint(&painter, x, y, w, h, Qt::AlignCenter);
    }
}


void QSearchLineEdit::resizeEvent(QResizeEvent *)
{
    updateSearchIconGeometry();

    const int hMargin = _styleMargins.width();

    const QRect contentRect = rect();
    //Position clear button ;

    QSize sz = _clearButton->size();
    const int btnX  = contentRect.right() - sz.width() - hMargin;
    const int btnY = (contentRect.height() - sz.height())/2;
    _clearButton->move(btnX, btnY);

    //Position progress indicator widget
#ifdef WITH_PROGRESS
    const int x = hMargin;
    const int y = (contentRect.height() - _progressIndicator->height())/2;
    _progressIndicator->move(x,y+1);
#endif

}

void QSearchLineEdit::updateClearButton(const QString& text)
{
    _clearButton->setVisible(!text.isEmpty());
}

void QSearchLineEdit::slotClearText()
{
    clear();
    emit textClear();
}

void QSearchLineEdit::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape)
    {
        slotClearText();
    }   
    QLineEdit::keyPressEvent(event);
}

bool QSearchLineEdit::isBusy() const
{
#ifdef WITH_PROGRESS
    return _progressIndicator->isAnimated();
#else
    return false;
#endif
}

void QSearchLineEdit::setBusy(bool busy)
{
#ifdef WITH_PROGRESS
    if(busy){
        _progressIndicator->show();
        _progressIndicator->startAnimation();
    }
    else{
        _progressIndicator->hide();
        _progressIndicator->stopAnimation();
    }
#endif
}
