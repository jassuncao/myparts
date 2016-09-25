#include "basicentitytablemodel.h"

BasicEntityTableModel::BasicEntityTableModel(QObject *parent, QSqlDatabase db) :
    QSqlTableModel(parent, db)
{
}

void BasicEntityTableModel::setTextFilter(const QString& text)
{
    if(!text.isEmpty()) {
        setFilter(QString("name LIKE '\%%1\%'").arg(text));
    }
    else{
        setFilter(QString::null);
        select();
    }
}

QModelIndex BasicEntityTableModel::findIndexOf(const QVariant& value, int columnIndex, int role)
{
    int row = 0;
    bool dataAvailable = true;
    while(dataAvailable){
        for(; row < rowCount(); ++row){
            const QModelIndex & idx = index(row, columnIndex);
            QVariant columnValue = data(idx, role);
            if(columnValue == value)
                return idx;
        }
        dataAvailable = canFetchMore();
        if(dataAvailable){
            fetchMore();
        }
    }
    return QModelIndex();
}

CompanyTableModel::CompanyTableModel(QObject *parent, QSqlDatabase db) :
    BasicEntityTableModel(parent, db)
{
    setSort(ColumnName, Qt::AscendingOrder);
    setEditStrategy(QSqlTableModel::OnManualSubmit);
}

int CompanyTableModel::getNameColumn() const
{
    return ColumnName;
}

QModelIndex CompanyTableModel::findIndexOfKeyValue(const QVariant& keyValue)
{
    return findIndexOf(keyValue, ColumnId, Qt::EditRole);
}

QVariant CompanyTableModel::keyValue(int row) {
    return index(row, ColumnId).data(Qt::EditRole);
}

//QModelIndex CompanyTableModel::rootIndex(int row) const
//{
//    return index(row, ColumnId);
//}

PackageTableModel::PackageTableModel(QObject *parent, QSqlDatabase db) :
    BasicEntityTableModel(parent, db)
{
    setSort(ColumnName, Qt::AscendingOrder);
    setEditStrategy(QSqlTableModel::OnManualSubmit);
}

int PackageTableModel::getNameColumn() const
{
    return ColumnName;
}

QModelIndex PackageTableModel::findIndexOfKeyValue(const QVariant& keyValue)
{
    return findIndexOf(keyValue, ColumnId, Qt::EditRole);
}

QVariant PackageTableModel::keyValue(int row) {
    return index(row, ColumnId).data(Qt::EditRole);
}

//QModelIndex PackageTableModel::rootIndex(int row) const
//{
//    return index(row, ColumnId);
//}
