#include "stocktableformatproxymodel.h"
#include "partstocklogtablemodel.h"


StockTableFormatProxyModel::StockTableFormatProxyModel(QObject *parent) :
    QIdentityProxyModel(parent)
{}

QVariant StockTableFormatProxyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    return sourceModel()->headerData(section, orientation, role);
}

QVariant StockTableFormatProxyModel::data(const QModelIndex &proxyIndex, int role) const
{
    if(!proxyIndex.isValid()) return QVariant();
    if(role==Qt::ForegroundRole){
        if(proxyIndex.column() == PartStockLogTableModel::ColumnChange){
            QVariant var = sourceModel()->data(proxyIndex, Qt::DisplayRole);
            int change = var.toInt();
            if(change<0)
                return QColor(Qt::red);
            else if(change>0)
                return QColor(Qt::darkGreen);
        }
    }
    else if(role == Qt::TextAlignmentRole) {
        if(proxyIndex.column() == PartStockLogTableModel::ColumnChange){
            return Qt::AlignCenter;
        }
    }
    else if(role == Qt::ToolTipRole){
        if(proxyIndex.column() == PartStockLogTableModel::ColumnComment){
            return QIdentityProxyModel::data(proxyIndex, Qt::DisplayRole);
        }
    }
    return QIdentityProxyModel::data(proxyIndex, role);
}
