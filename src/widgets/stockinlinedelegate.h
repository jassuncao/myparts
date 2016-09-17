#ifndef STOCKINLINEDELEGATE_H
#define STOCKINLINEDELEGATE_H

#include <QStyledItemDelegate>

class StockInlineDelegate : public QStyledItemDelegate {
    Q_OBJECT
public:
     StockInlineDelegate(QObject * parent = 0);
     ~StockInlineDelegate();
     virtual QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &, const QModelIndex &index) const;
     virtual void setModelData(QWidget *editor,QAbstractItemModel *model, const QModelIndex &index) const;
     void setEditorData(QWidget *editor, const QModelIndex &index) const;
private:    
};

#endif // STOCKINLINEDELEGATE_H
