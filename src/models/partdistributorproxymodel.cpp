#include "partdistributorproxymodel.h"

#include "customtablemodel.h"
#include "price.h"

PartDistributorProxyModel::PartDistributorProxyModel(QObject *parent) :
    QIdentityProxyModel(parent)
{}

PartDistributorProxyModel::~PartDistributorProxyModel()
{
}


QVariant PartDistributorProxyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    return sourceModel()->headerData(section, orientation, role);
}



Qt::ItemFlags PartDistributorProxyModel::flags(const QModelIndex& index) const
{
    Qt::ItemFlags res = QIdentityProxyModel::flags(index);
    if(index.column() == PartDistributorTableModel2::ColumnDateTime){
        res &= ~Qt::ItemIsEditable;
    }
    return res;
}

QVariant PartDistributorProxyModel::data(const QModelIndex &proxyIndex, int role) const
{
    if(!proxyIndex.isValid()) return QVariant();
    int column = proxyIndex.column();
    if(column == PartDistributorTableModel2::ColumnUnitPrice){
        if(role == Qt::DisplayRole || role == Qt::EditRole){
            QVariant value = sourceModel()->index(proxyIndex.row(), PartDistributorTableModel2::ColumnUnitPrice).data();
            QVariant currency = sourceModel()->index(proxyIndex.row(), PartDistributorTableModel2::ColumnCurrency).data();
            if(value.isValid()){
                return QVariant::fromValue(Price(value.toDouble(), currency.toString()));
            }
        }
    }
    return QIdentityProxyModel::data(proxyIndex, role);
}

