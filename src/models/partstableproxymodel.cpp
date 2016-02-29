#include "partstableproxymodel.h"
#include "partssqltablemodel.h"
#include "partstocktablemodel.h"
#include <QColor>

PartsTableProxyModel::PartsTableProxyModel(QObject *parent) :
    QIdentityProxyModel(parent)
{}

void PartsTableProxyModel::setPartStockTableModel(PartStockTableModel* stockTableModel)
{
    _stockTableModel = stockTableModel;
}

QVariant PartsTableProxyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    return sourceModel()->headerData(section, orientation, role);
}

Qt::ItemFlags PartsTableProxyModel::flags(const QModelIndex& index) const
{
    Qt::ItemFlags res = QIdentityProxyModel::flags(index);
    if(index.column()!=PartsSqlTableModel::ColumnActualStock){
        res &= ~Qt::ItemIsEditable;
    }
    return res;
}

bool PartsTableProxyModel::submit()
{
    PartsSqlTableModel* partsTableModel = static_cast<PartsSqlTableModel*>(sourceModel());
    bool res = QIdentityProxyModel::submit();
    if(partsTableModel){
        res = partsTableModel->submitAll();
    }
    return res ;
}

/*
bool PartsTableProxyModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(index.column()==PartsSqlTableModel::ColumnActualStock){
        PartsSqlTableModel* partsTableModel = static_cast<PartsSqlTableModel*>(sourceModel());
        partsTableModel->database().transaction();
        int oldValue = data(index, Qt::EditRole).toInt();
        int newValue = value.toInt();
        int diff = oldValue - newValue;
        _stockTableModel->appendRow(diff, QVariant(), QString());
        bool res = setData(index, value, Qt::EditRole);
        if(res){
            partsTableModel->database().commit();
        }
        else{
            //TODO: Display some warning
            qWarning("Failed to insert stock change (addition) in database");
            partsTableModel->database().rollback();
        }

    }
    return QIdentityProxyModel::setData(index, value, role);
}
*/

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

