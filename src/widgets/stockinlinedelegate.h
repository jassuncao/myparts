#ifndef STOCKINLINEDELEGATE_H
#define STOCKINLINEDELEGATE_H

#include <QStyledItemDelegate>
#include <QSqlDatabase>

class PartsSqlTableModel;

class StockInlineDelegate : public QStyledItemDelegate {
    Q_OBJECT
public:
     StockInlineDelegate(QSqlDatabase &database, QObject * parent = 0);
     ~StockInlineDelegate();
     virtual QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &, const QModelIndex &index) const;
     virtual void setModelData(QWidget *editor,QAbstractItemModel *model, const QModelIndex &index) const;
     void setEditorData(QWidget *editor, const QModelIndex &index) const;
private:
    bool insertStockChange(const QVariant& partId, const QVariant & stockChange);
    QSqlDatabase _database;
};

#endif // STOCKINLINEDELEGATE_H
