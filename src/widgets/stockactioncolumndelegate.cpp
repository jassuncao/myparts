#include "stockactioncolumndelegate.h"
#include <QPainter>

StockActionColumnDelegate::StockActionColumnDelegate(QObject *parent) :
    QStyledItemDelegate(parent),
    _addIcon(":icons/addStock"),
    _removeIcon(":icons/removeStock")
{    
    _sizeHint = _addIcon.size().boundedTo(_removeIcon.size());
}

void StockActionColumnDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
 {
    QPixmap icon = index.data().toBool() ? _addIcon :_removeIcon;
    QSize size = icon.size();
    QRect rect = option.rect;
    int yOffset = (rect.height() - size.height())/2;
    int xOffset = (rect.width() - size.width())/2;
    painter->drawPixmap(rect.x()+xOffset, rect.y()+yOffset, icon);
 }

 QSize StockActionColumnDelegate::sizeHint(const QStyleOptionViewItem &, const QModelIndex &) const
 {
     return _sizeHint;
 }

