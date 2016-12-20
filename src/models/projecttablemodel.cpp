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

/*

ProjectPartTableModel::ProjectPartTableModel(QObject *parent) :
    CustomTableModel(6, parent)
{
}

QString ProjectPartTableModel::generateSelectStatement(int sortColumn, Qt::SortOrder order)
{
    QString selectStmt = QString("SELECT refdes, quantity, part_name, part, remark, id, p.name FROM project_part LEFT JOIN part p ON part=p.id WHERE project = ?");
    if((sortColumn >= 0) && (sortColumn < fieldNames.size())){
        QLatin1String orderStatement = order == Qt::AscendingOrder ? QLatin1String(" ASC") : QLatin1String(" DESC");
        selectStmt.append(" ORDER BY ").append(fieldNames.at(sortColumn)).append(orderStatement);
    }
    return selectStmt;
}

ProjectPartTableModel* ProjectPartTableModel::createNew(QObject *parent)
{
    QStringList fieldNames;
    QStringList columnNames;
    fieldNames<< QLatin1String("refdes") << QLatin1String("quantity") << QLatin1String("part_name") << QLatin1String("part") << QLatin1String("remark");
    columnNames<< tr("Ref. Des.") << tr("Quantity") << tr("Part") << tr("Assigned Part") << tr("Remark") << tr("Assigned Part");
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

void ProjectPartTableModel::setParentKey(const QVariant & parentKey)
{
    _parentKey = parentKey;
}

void ProjectPartTableModel::sort(int column, Qt::SortOrder order)
{
    const QString & selectStmt = generateSelectStatement(_tableName, _fieldNames, _foreignKeyField, column, order);
    qDebug()<<"Select query: "<<selectStmt;
    _selectQuery.prepare(selectStmt);
    select();
}

bool ProjectPartTableModel::deleteItem(QVariant id)
{
    _deleteQuery.bindValue(0, id);
    bool res = _deleteQuery.exec();
    if(!res){
        qWarning()<<"Failed to execute delete query. Reason: "<<_deleteQuery.lastError().driverText();
    }
    return res;
}

bool ProjectPartTableModel::saveItem(TableItem* item)
{
    if(item->id().isValid()){
        //Edited item. Update
        qDebug()<<"Updating item "<<item->id();
        int i = 0;
        for(; i<_maxColumns;++i){
            _updateQuery.bindValue(i, item->data(i));
        }
        _updateQuery.bindValue(i, item->id());
        if(_updateQuery.exec()){
            item->resetDirty();
            return true;
        }
        qWarning()<<"Failed to execute update query. Reason: "<<_updateQuery.lastError().driverText();
        return false;
    }
    else{
        //Brand new item. Insert
        qDebug()<<"Inserting new item ";
        int i = 0;
        for(; i<_maxColumns;++i){
            _insertQuery.bindValue(i, item->data(i));
        }
        if(!_foreignKeyField.isEmpty()){
            _insertQuery.bindValue(i, _foreignKey);
        }
        if(_insertQuery.exec()){
            item->setId(_insertQuery.lastInsertId());
            item->resetDirty();
            return true;
        }
        qWarning()<<"Failed to execute insert query. Reason: "<<_insertQuery.lastError().driverText();
        return false;
    }
}

bool ProjectPartTableModel::loadItems(QList<TableItem*> & dest)
{
    if(!_parentKey.isEmpty()){
        if(!_parentKey.isValid())
            return false;
        qDebug()<<"loading items for "<<_parentKey;
        _selectQuery.bindValue();
        _selectQuery.bindValue(0, _parentKey);
    }

    if(_selectQuery.exec()){
        while(_selectQuery.next())
        {
            QVector<QVariant> rowData(_maxColumns);
            for(int i=0; i<_maxColumns; ++i){
                rowData[i] = _selectQuery.value(i);
            }
            QVariant id = _selectQuery.value(_maxColumns);
            TableItem * item = new TableItem(rowData, id, false);
            dest.append(item);
        }
        return true;
    }
    qWarning()<<"Failed to execute query. Reason "<<_selectQuery.lastError().driverText();
    return false;
}
*/
