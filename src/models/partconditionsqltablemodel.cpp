#include "partconditionsqltablemodel.h"

PartConditionSqlTableModel::PartConditionSqlTableModel(QObject *parent) :
    QSqlTableModel(parent)
{
    setTable("part_condition");
}

Qt::ItemFlags PartConditionSqlTableModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flags = QSqlTableModel::flags(index);
    if(index.column()==ColumnDefault){
        flags = flags & (~Qt::ItemIsEditable);
    }
     return flags;
}
