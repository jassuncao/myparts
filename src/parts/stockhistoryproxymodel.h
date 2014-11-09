#ifndef STOCKHISTORYPROXYMODEL_H
#define STOCKHISTORYPROXYMODEL_H

#include <QAbstractProxyModel>
#include <QModelIndex>

class StockHistoryProxyModel : public QAbstractProxyModel
{
    Q_OBJECT
public:
    explicit StockHistoryProxyModel(QObject *parent = 0);
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QModelIndex mapFromSource ( const QModelIndex & sourceIndex ) const;
    QModelIndex mapToSource ( const QModelIndex & proxyIndex ) const;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &child) const;
signals:
    
public slots:
    
};

#endif // STOCKHISTORYPROXYMODEL_H
