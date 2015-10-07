#include "comboboxlookupmodel.h"

ComboBoxLookupModel::ComboBoxLookupModel(const QList<QPair<QString, int> > &items, QObject *parent) :
    QAbstractListModel(parent), _items(items)
{
}

ComboBoxLookupModel::ComboBoxLookupModel(QObject *parent) : QAbstractListModel(parent)
{
}

ComboBoxLookupModel::~ComboBoxLookupModel()
{
    qDebug("ComboBoxLookupModel destroy");
}

void ComboBoxLookupModel::setItems(QList<QPair<QString, int> > &items)
{
    beginResetModel();
    _items = items;
    endResetModel();
}

int ComboBoxLookupModel::rowCount(const QModelIndex &parent ) const
{
    if (parent.isValid())
        return 0;
    return _items.size();
}

QVariant ComboBoxLookupModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= _items.size())
        return QVariant();
    if (role == Qt::DisplayRole)
        return _items.at(index.row()).first;
    if (role == Qt::EditRole)
        return _items.at(index.row()).second;
    return QVariant();
}

int ComboBoxLookupModel::findIdIndex(const int id) const
{
    for(int i=0; i<_items.size();++i){
        const QPair<QString, int> pair = _items.at(i);
        if(pair.second==id)
            return i;
    }
    return -1;
}

