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

QModelIndex CompanyTableModel::rootIndex(int row) const
{
    return index(row, ColumnId);
}

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

QModelIndex PackageTableModel::rootIndex(int row) const
{
    return index(row, ColumnId);
}
