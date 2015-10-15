#ifndef BOOLEANITEMDELEGATE_H
#define BOOLEANITEMDELEGATE_H

#include <QStyledItemDelegate>
#include <QPixmap>

class BooleanItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit BooleanItemDelegate(const QPixmap & truePix, const QPixmap & falsePix, QObject *parent = 0);
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QWidget * createEditor(QWidget *parent, const QStyleOptionViewItem &, const QModelIndex &index) const;
    int widthHint();
private:
    QPixmap _trueIcon;
    QPixmap _falseIcon;
    QSize _sizeHint;
};

#endif // BOOLEANITEMDELEGATE_H
