#include "projecttablemodel.h"
#include <QDebug>

ProjectTableModel::ProjectTableModel(QObject *parent, QSqlDatabase db) :
    BasicEntityTableModel(parent, db)
{    
    setEditStrategy(QSqlTableModel::OnManualSubmit);
    setTable("project");
}

int ProjectTableModel::getNameColumn() const
{
    return ColumnName;
}

QModelIndex ProjectTableModel::findIndexOfKeyValue(const QVariant& keyValue)
{
    return findIndexOf(keyValue, ColumnId, Qt::EditRole);
}

QVariant ProjectTableModel::keyValue(int row) {
    return index(row, ColumnId).data(Qt::EditRole);
}


ProjectPartTableModel::ProjectPartTableModel(const QStringList & fieldNames, const QStringList & columnNames, QObject *parent) :
    SimpleSqlTableModel("project_part", fieldNames, columnNames, "project", parent)
{
    registerRelation(ColumnPart, new TableRelation("part", "id", "name"));
}

ProjectPartTableModel* ProjectPartTableModel::createNew(QObject *parent)
{
    QStringList fieldNames;
    QStringList columnNames;
    fieldNames<< QLatin1String("refdes") << QLatin1String("quantity") << QLatin1String("part_name") << QLatin1String("part") << QLatin1String("remark");
    columnNames<< tr("Ref. Des.") << tr("Quantity") << tr("Part") << tr("Assigned Part") << tr("Remark");
    return new ProjectPartTableModel(fieldNames, columnNames, parent);
}

TableItem * ProjectPartTableModel::createBlankItem() const
{
    TableItem * item = SimpleSqlTableModel::createBlankItem();
    int value = 0;
    item->setData(ColumnQuantity, QVariant(value));
    return item;
}

QVariant ProjectPartTableModel::data(const QModelIndex &index, int role) const
{
    QVariant value = SimpleSqlTableModel::data(index, role);
    if(index.column() == ColumnQuantity && role == Qt::EditRole ) {
        value.convert(QVariant::Int);
    }
    return value;
}

