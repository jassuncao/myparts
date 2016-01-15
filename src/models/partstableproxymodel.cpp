#include "partstableproxymodel.h"
#include "partssqltablemodel.h"
#include <QColor>

PartsTableProxyModel::PartsTableProxyModel(QObject *parent) :
    QIdentityProxyModel(parent)
{}

QVariant PartsTableProxyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    return sourceModel()->headerData(section, orientation, role);
}

QVariant PartsTableProxyModel::data(const QModelIndex &proxyIndex, int role) const
{
    if(!proxyIndex.isValid()) return QVariant();
    if(role==Qt::ForegroundRole){
        if(proxyIndex.column()==PartsSqlTableModel::ColumnActualStock){
            QVariant minStockVar = sourceModel()->index(proxyIndex.row(), PartsSqlTableModel::ColumnMinStock).data();
            int minStock = minStockVar.toInt();
            if(minStock>0){
                QVariant stockVar = sourceModel()->data(proxyIndex, Qt::DisplayRole);
                int stock = stockVar.toInt();
                if(stock<minStock){
                    return QColor(Qt::red);
                }
                else {
                    return QColor(Qt::darkGreen);
                }
            }
        }
    }
    else if(role == Qt::TextAlignmentRole) {
        if(proxyIndex.column()==PartsSqlTableModel::ColumnActualStock
                || proxyIndex.column()==PartsSqlTableModel::ColumnMinStock){
            return Qt::AlignCenter;
        }
    }
    return QIdentityProxyModel::data(proxyIndex, role);
}

