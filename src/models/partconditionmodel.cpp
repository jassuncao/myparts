#include "partconditionmodel.h"

PartConditionModel::PartConditionModel(const QStringList & fieldNames, const QStringList & columnNames, QObject *parent) :
    SimpleSqlTableModel("condition", fieldNames, columnNames, QString(), parent)
{
}

int PartConditionModel::findDefaultValueRow() const
{
    QModelIndex start = index(0, PartConditionModel::ColumnDefault);
    QModelIndexList res = match(start, Qt::EditRole, QVariant(1));
    if(!res.isEmpty())
        return res.first().row();
    qWarning("Default value not found");
    return -1;
}

void PartConditionModel::makeSelectedItemDefault(const int selectedRow)
{
    int rowCount = rowCount();
    for(int row=0; row<rowCount; ++row){
        QModelIndex index = index(row, PartConditionModel::ColumnDefault);
        QVariant var = data(index, Qt::EditRole);
        if(var.toBool() && row!=selectedRow){
            setData(index, QVariant(false));
        }
        if(row==selectedRow){
            setData(index, QVariant(true));
        }
    }
}
