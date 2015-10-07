#include "unitproxymodel.h"
#include <QDebug>

UnitProxyModel::UnitProxyModel(QObject* parent)
    : QIdentityProxyModel(parent)
{
}

UnitProxyModel::~UnitProxyModel()
{

}

/*
int UnitProxyModel::columnCount(const QModelIndex& parent) const
{
    return QIdentityProxyModel::columnCount(parent) + 1;
}
*/

/*
QModelIndex UnitProxyModel::index(int row, int column, const QModelIndex& parent) const
{
    if(column==QIdentityProxyModel::columnCount(parent)){
        return createIndex()
    }
}
*/

QVariant UnitProxyModel::data(const QModelIndex &proxyIndex, int role) const
{
    return QIdentityProxyModel::data(proxyIndex, role);

    /*
    qDebug()<<"data for column "<<proxyIndex.column();
    if(proxyIndex.column()==QIdentityProxyModel::columnCount(proxyIndex.parent())){
        qDebug()<<"Returning list";
        QList<QVariant> items;
        items<<1<<2;
        return QVariant(items);
    }
    else{
        return QIdentityProxyModel::data(proxyIndex, role);
    }
    */
}

/*
QModelIndex UnitProxyModel::mapToSource(const QModelIndex &proxyIndex) const
{
   return proxyIndex;
}

QModelIndex UnitProxyModel::mapFromSource(const QModelIndex &sourceIndex) const
{

}
*/



