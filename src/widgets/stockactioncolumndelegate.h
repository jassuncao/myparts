#ifndef STOCKACTIONCOLUMNDELEGATE_H
#define STOCKACTIONCOLUMNDELEGATE_H

#include <QStyledItemDelegate>
#include <QPixmap>

class StockActionColumnDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit StockActionColumnDelegate(QObject *parent = 0);
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
private:
    QPixmap _addIcon;
    QPixmap _removeIcon;
    QSize _sizeHint;
};

#endif // STOCKACTIONCOLUMNDELEGATE_H
