#include "booleanitemdelegate.h"
#include <QPainter>


BooleanItemDelegate::BooleanItemDelegate(const QPixmap & truePix, const QPixmap & falsePix, QObject *parent) :
    QStyledItemDelegate(parent),
    _trueIcon(truePix),
    _falseIcon(falsePix)
{
    _sizeHint = _trueIcon.size().expandedTo(_falseIcon.size());
}

void BooleanItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QPixmap pix = index.data().toBool() ? _trueIcon :_falseIcon;
    if(pix.isNull())
        return;
    QSize size = pix.size();
    QRect rect = option.rect;
    int yOffset = (rect.height() - size.height())/2;
    int xOffset = (rect.width() - size.width())/2;
    painter->drawPixmap(rect.x()+xOffset, rect.y()+yOffset, pix);
}

QSize BooleanItemDelegate::sizeHint(const QStyleOptionViewItem &, const QModelIndex &) const
{
    return _sizeHint;
}

QWidget * BooleanItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &, const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;
   return 0;
}

int BooleanItemDelegate::widthHint()
{
    return _sizeHint.width()+4;
}

