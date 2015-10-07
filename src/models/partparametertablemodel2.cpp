#include <QSqlError>
#include <QDebug>

#include "partparametertablemodel2.h"
#include "valuewithunit.h"
#include "parametervalue.h"

namespace {
    const int MaxColumns = 4;
}

PartParameterTableModel2::PartParameterTableModel2(QObject *parent) : QAbstractTableModel(parent),
    _unitsCacheReady(false)
{
    _selectQuery.prepare("SELECT id, name, value, unit, description "
                         "FROM part_parameter "
                         "WHERE part = ?");
    _insertQuery.prepare("INSERT INTO part_parameter (name, value, unit, description, part) "
                         "VALUES (?,?,?,?,?)");
    _updateQuery.prepare("UPDATE part_parameter SET name=?, value=?, unit=?, description=? "
                         "WHERE id = ?");
    _deleteQuery.prepare("DELETE FROM part_parameter "
                         "WHERE id = ?");

    _selectUnitsQuery.prepare("SELECT id, name, symbol FROM unit ORDER BY name");
}

PartParameterTableModel2::~PartParameterTableModel2()
{

}

QVariant PartParameterTableModel2::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();
    if (orientation == Qt::Horizontal) {
        switch (section) {
            case ColumnName: return tr("Name");
            case ColumnValue: return tr("Value");
            case ColumnUnit: return tr("Unit");
            case ColumnDescription: return tr("Description");
            default: Q_ASSERT(false);
        }
    }
    return section + 1;
}

int PartParameterTableModel2::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : _items.count();
}


int PartParameterTableModel2::columnCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : MaxColumns;
}

Qt::ItemFlags PartParameterTableModel2::flags(const QModelIndex &index) const
{
    Qt::ItemFlags theFlags = QAbstractTableModel::flags(index);
    if (index.isValid()){
        theFlags |= Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
    }
    return theFlags;
}

QVariant PartParameterTableModel2::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() ||
        index.row() < 0 || index.row() >= _items.count() ||
        index.column() < 0 || index.column() >= MaxColumns) {
        return QVariant();
    }

    const PartParameterItem &item = _items.at(index.row());
    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        switch (index.column()) {
            case ColumnName: return item.name;
            case ColumnValue: {
                if(role==Qt::EditRole)
                    return item.value;
                return getValue(item.value, item.unit);
            }
            case ColumnUnit: {
                if(role==Qt::EditRole)
                    return item.unit;
                return getUnitName(item.unit);
            }
            case ColumnDescription: return item.description;
            default: Q_ASSERT(false);
        }
    }
    return QVariant();
}

bool PartParameterTableModel2::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || role != Qt::EditRole ||
        index.row() < 0 || index.row() >= _items.count() ||
        index.column() < 0 || index.column() >= MaxColumns){
        return false;
    }
    PartParameterItem &item = _items[index.row()];
    switch (index.column()) {
        case ColumnName: {
            item.name = value.toString();
            break;
        }
        case ColumnValue: {
            bool ok;
            double paramValue = value.toDouble(&ok);
            if(!ok){
                return false;
            }
            item.value = paramValue;
            break;
        }
        case ColumnUnit: {
            bool ok;
            int unitId = value.toInt(&ok);
            if(!ok){
                return false;
            }
            if(!_unitsCache.contains(unitId)){
                return false;
            }
            item.unit = unitId;
            break;
        }
        case ColumnDescription: {
            item.description = value.toString();
            break;
        }
        default: Q_ASSERT(false);
    }
    item.dirty = true;
    emit dataChanged(index, index);
    return true;
}

bool PartParameterTableModel2::insertRows(int row, int count, const QModelIndex&)
{
    beginInsertRows(QModelIndex(), row, row + count - 1);
    for (int i = 0; i < count; ++i) {
        _items.insert(row, PartParameterItem());
    }
    endInsertRows();
    return true;
}


bool PartParameterTableModel2::removeRows(int row, int count, const QModelIndex&)
{
    beginRemoveRows(QModelIndex(), row, row + count - 1);
    for (int i = 0; i < count; ++i) {
        const PartParameterItem & item = _items.at(row);
        if(item.id>0){
            _deletedItems.append(item.id);
            _items.remove(row);
        }
    }
    endRemoveRows();
    return true;
}


void PartParameterTableModel2::load(const QVariant & partId)
{
    beginResetModel();
    _items.clear();
    if(partId.isValid()){
        _selectQuery.bindValue(0, partId);
        if(_selectQuery.exec()){
            while(_selectQuery.next())
            {
                QVariant id = _selectQuery.value(0);
                QVariant name = _selectQuery.value(1);
                QVariant value = _selectQuery.value(2);
                QVariant unit = _selectQuery.value(3);
                QVariant description = _selectQuery.value(4);

                PartParameterItem item(id.toInt(), name.toString(), value.toDouble(),
                                       unit.toInt(), description.toString());
                _items.append(item);
            }
        }
        else{
            qWarning()<<"Failed to execute query. Reason "<<_selectQuery.lastError().driverText();
        }

    }
    else{
        qDebug()<<"Part entity id is invalid";
    }
    if(!_unitsCacheReady)
        loadUnitsCache();
    endResetModel();
}

void PartParameterTableModel2::save(const QVariant & partId)
{
    while (!_deletedItems.isEmpty()){
        int paramId = _deletedItems.takeFirst();
        qDebug()<<"Deleting part parameter "<<paramId;
        _deleteQuery.bindValue(0, paramId);
        if(!_deleteQuery.exec()){
            qWarning()<<"Failed to execute delete query. Reason: "<<_deleteQuery.lastError().driverText();
        }
    }

    for(int i=0; i<_items.count(); ++i){
        PartParameterItem &item = _items[i];
        if(item.dirty){
            if(item.id<0){
                //Brand new item. Insert
                qDebug()<<"Inserting new part parameter ";
                _insertQuery.bindValue(0, item.name);
                _insertQuery.bindValue(1, item.value);
                _insertQuery.bindValue(2, item.unit);
                _insertQuery.bindValue(3, item.description);
                _insertQuery.bindValue(4, partId);
                if(_insertQuery.exec()){
                    item.dirty = false;
                }
                else{
                    qWarning()<<"Failed to execute insert query. Reason: "<<_insertQuery.lastError().driverText();
                }
            }
            else{
                //Edited item. Update
                qDebug()<<"Updating part parameter "<<item.id;
                _updateQuery.bindValue(0, item.name);
                _updateQuery.bindValue(1, item.value);
                _updateQuery.bindValue(2, item.unit);
                _updateQuery.bindValue(3, item.description);
                _updateQuery.bindValue(4, item.id);
                if(_updateQuery.exec()){
                    item.dirty = false;
                }
                else{
                    qWarning()<<"Failed to execute update query. Reason: "<<_updateQuery.lastError().driverText();
                }
            }
        }
    }
}

void PartParameterTableModel2::loadUnitsCache()
{
    _unitsCache.clear();
    QList<QPair<QString,int> > list;
    if(_selectUnitsQuery.exec()){
        while(_selectUnitsQuery.next())
        {
            QVariant id = _selectUnitsQuery.value(0);
            QVariant name = _selectUnitsQuery.value(1);
            QVariant symbol = _selectUnitsQuery.value(2);
            ParamUnitItem item(id.toInt(), name.toString(), symbol.toString());
            _unitsCache.insert(item.id, item);
            list.append(qMakePair(item.name, item.id));
        }
        qSort(list);
        _lookupModel = new ComboBoxLookupModel(list, this);
    }
    else{
        qWarning()<<"Failed to execute query. Reason "<<_selectUnitsQuery.lastError().driverText();
    }
    _unitsCacheReady = true;
}

ComboBoxLookupModel *PartParameterTableModel2::unitsModel() const
{
    return _lookupModel;   
}

QString PartParameterTableModel2::getUnitName(int unitId) const
{
    return _unitsCache.value(unitId).name;
}

QVariant PartParameterTableModel2::getValue(double value, int unitId) const
{
    const QString & symbol =_unitsCache.value(unitId).symbol;
    ParameterValue paramValue(value, symbol);
    QVariant var;
    var.setValue(paramValue);
    return var;
}

