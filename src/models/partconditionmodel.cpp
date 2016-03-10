#include "partconditionmodel.h"

PartConditionModel::PartConditionModel(const QStringList & fieldNames, const QStringList & columnNames, QObject *parent) :
    SimpleSqlTableModel("condition", fieldNames, columnNames, QString(), parent)
{
}

int PartConditionModel::findDefaultValueRow() const
{
    QModelIndex start = index(0, PartConditionModel::ColumnDefault);
    QModelIndexList res = match(start, Qt::EditRole, QVariant(true));
    if(!res.isEmpty())
        return res.first().row();
    qWarning("Default value not found");
    return -1;
}

void PartConditionModel::makeSelectedItemDefault(const int selectedRow)
{
    int rCount = rowCount();
    for(int row=0; row<rCount; ++row){
        QModelIndex idx = index(row, PartConditionModel::ColumnDefault);
        QVariant var = data(idx, Qt::EditRole);
        if(var.toBool() && row!=selectedRow){
            setData(idx, QVariant(false));
        }
        if(row==selectedRow){
            setData(idx, QVariant(true));
        }
    }
}

PartConditionModel* PartConditionModel::createNew(QObject *parent)
{
    QStringList fieldNames;
    QStringList columnNames;
    fieldNames<<QLatin1String("defaultCondition")<<QLatin1String("value")<<QLatin1String("description");
    columnNames<<QString()<<tr("Value")<<tr("Description");
    return new PartConditionModel(fieldNames, columnNames, parent);
}

