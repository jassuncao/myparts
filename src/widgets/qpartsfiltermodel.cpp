#include "qpartsfiltermodel.h"


QPartsFilterModel::QPartsFilterModel(QObject *parent) :
    QAbstractTableModel(parent)
{
    _items.append(QVariant(false));
    _items.append(QVariant());
}


int QPartsFilterModel::rowCount(const QModelIndex &parent) const
{
    return 1;
}

int QPartsFilterModel::columnCount(const QModelIndex &parent) const
{
    return _items.size();
}

QVariant QPartsFilterModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid()){
        return QVariant();
    }

    switch(role){
    case Qt::DisplayRole:
    case Qt::EditRole:
        //qDebug("returning for %d", index.column());
            return _items.at(index.column());
        break;
    case Qt::ToolTipRole:
            return QVariant();
        break;
    default:
        return QVariant();
        break;
    }
}

bool QPartsFilterModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(!index.isValid()){
        return false;
    }
    if(role!=Qt::EditRole){
        return false;
    }
    _items[index.column()] = value;
    emit dataChanged(index,index);
    return true;
}
