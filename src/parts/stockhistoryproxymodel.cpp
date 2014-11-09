#include "stockhistoryproxymodel.h"

StockHistoryProxyModel::StockHistoryProxyModel(QObject *parent) :
    QAbstractProxyModel(parent)
{
}

int StockHistoryProxyModel::columnCount(const QModelIndex &parent) const
{
    Q_ASSERT(parent.isValid() ? parent.model() == this : true);
    return sourceModel()->columnCount(parent);
}

int StockHistoryProxyModel::rowCount(const QModelIndex &parent) const
{
    Q_ASSERT(parent.isValid() ? parent.model() == this : true);
    return sourceModel()->rowCount();
}


QModelIndex StockHistoryProxyModel::mapFromSource ( const QModelIndex & sourceIndex ) const
{
    if(!sourceModel() || !sourceIndex.isValid())
        return QModelIndex();
    Q_ASSERT(sourceIndex.model() == sourceModel());
    return createIndex(sourceIndex.row(), sourceIndex.column(), sourceIndex.internalPointer());
}

QModelIndex StockHistoryProxyModel::mapToSource ( const QModelIndex & proxyIndex ) const
{
    if(!sourceModel() || !proxyIndex.isValid())
        return QModelIndex();
    Q_ASSERT(proxyIndex.model() == this);
    return sourceModel()->index(proxyIndex.row(), proxyIndex.column());
}

QModelIndex StockHistoryProxyModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_ASSERT(parent.isValid() ? parent.model() == this : true);
    if (!hasIndex(row, column, parent))
        return QModelIndex();
    const QModelIndex sourceParent = mapToSource(parent);
    const QModelIndex sourceIndex = sourceModel()->index(row, column, sourceParent);
    qDebug("Index for %d, %d", row, column);
    Q_ASSERT(sourceIndex.isValid());
    return mapFromSource(sourceIndex);
}

QModelIndex StockHistoryProxyModel::parent(const QModelIndex &child) const
{
    Q_ASSERT(child.isValid() ? child.model() == this : true);
    const QModelIndex sourceIndex = mapToSource(child);
    const QModelIndex sourceParent = sourceIndex.parent();
    return mapFromSource(sourceParent);
}
