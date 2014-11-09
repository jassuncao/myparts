#include "partunitssqltablemodel.h"

PartUnitsSqlTableModel::PartUnitsSqlTableModel(QObject *parent)
    : QSqlTableModel(parent)
{
    setTable("part_unit");
}

Qt::ItemFlags PartUnitsSqlTableModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flags = QSqlTableModel::flags(index);
    if(index.column()==ColumnDefault){
        flags = flags & (~Qt::ItemIsEditable);
    }
     return flags;
}

