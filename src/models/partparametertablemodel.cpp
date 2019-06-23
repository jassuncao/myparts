#include "partparametertablemodel.h"
#include "parameter.h"
#include "parametervalue.h"
#include "../widgets/unitformatter.h"
#include <QSqlError>
#include <QSqlQuery>

const static int COLUMN_COUNT = 4;
const static int PARAM_NAME_COLUMN = 1;

class PartParameterItem {

public:
    PartParameterItem() : _id(), _param(), _value(), _minValue(), _maxValue() {
    }

    PartParameterItem(const QVariant & id,
                      const Parameter & param,
                      const QVariant & value,
                      const QVariant & minValue,
                      const QVariant & maxValue) :
        _id(id),
        _param(param),
        _value(value),
        _minValue(minValue),
        _maxValue(maxValue)
    {}

    QVariant id() const {
        return _id;
    }

    void setId(QVariant id) {
        _id = id;
    }

    inline bool dirty() const {
        return _dirty;
    }

    void resetDirty() {
        _dirty = false;
    }

    Parameter param() const {
        return _param;
    }

    void setParam(const Parameter & param){
        _param = param;
    }    

    QVariant value() const {
        return _value;
    }

    QVariant validateValue(QVariant value, bool *ok = 0){
        bool valid;
        QVariant res;
        if(value.isNull()){
           valid = true;
        }
        else{
            switch(_param.datatype()){
            case Parameter::TEXT:
                valid = value.canConvert(QVariant::String);
                if(valid){
                     res = QVariant(value.toString());
                }
                break;
            case Parameter::DECIMAL:
                valid = value.canConvert(QVariant::Double);
                if(valid){
                    res = QVariant(value.toReal());
                }
                break;
            case Parameter::INTEGER:
                valid = value.canConvert(QVariant::Double);
                if(valid){
                    res = QVariant(qRound(value.toReal()));
                }
                break;
            default:
                valid = false;
            }
        }
        if(ok){
            *ok = valid;
        }
        return res;
    }

    bool setValue(QVariant value) {
        bool ok;        
        const QVariant res = validateValue(value, &ok);
        if(ok){
            _value = res;
            _dirty = true;
        }
        return ok;
    }

    QVariant minValue() const {
        return _minValue;
    }

    bool setMinValue(QVariant value) {
        bool ok;
        const QVariant res = validateValue(value, &ok);
        if(ok){
            _minValue = res;
            _dirty = true;
        }
        return ok;
    }

    QVariant maxValue() const {
        return _maxValue;
    }

    bool setMaxValue(QVariant value) {
        bool ok;
        const QVariant res = validateValue(value, &ok);
        if(ok){
            _maxValue = res;
            _dirty = true;
        }
        return ok;
    }

private:
    QVariant _id;
    Parameter _param;
    QVariant _value;
    QVariant _minValue;
    QVariant _maxValue;
    bool _dirty;
};

class ParametersListModel : public QAbstractListModel{
public:
    explicit ParametersListModel(const QList<Parameter>& parameters, QObject *parent = 0) : QAbstractListModel(parent),
        _items(parameters)
    {}
    int rowCount(const QModelIndex &) const
    {
        return _items.size();
    }

    QVariant data(const QModelIndex &index, int role) const
    {
        if (index.row() < 0 || index.row() >= _items.size())
            return QVariant();
        if (role == Qt::DisplayRole || role == Qt::EditRole)
            return _items.at(index.row()).name();
        if (role == IModelWithForeignKey::ForeignKeyRole)
            return _items.at(index.row()).id();
        return QVariant();
    }
private:
    const QList<Parameter> _items;
};

PartParameterTableModel::PartParameterTableModel(QObject *parent) :
    QAbstractTableModel(parent)
{
    QString selectStmt("SELECT id, parameter, value, minValue, maxValue FROM part_parameter WHERE part=? ORDER BY id ASC");
    QString insertStmt("INSERT INTO part_parameter (part, parameter, value, minValue, maxValue) VALUES (?,?,?,?,?)");
    QString deleteStmt("DELETE FROM part_parameter WHERE id = ?");
    QString updateStmt("UPDATE part_parameter SET part=?, parameter=?, value=?, minValue=?, maxValue=? WHERE id = ?");

    _selectQuery.prepare(selectStmt);
    _insertQuery.prepare(insertStmt);
    _updateQuery.prepare(updateStmt);
    _deleteQuery.prepare(deleteStmt);
    loadParameters();
}

PartParameterTableModel::~PartParameterTableModel()
{
    qDeleteAll(_items);
    _items.clear();
}

void PartParameterTableModel::setCurrentPartId(const QVariant& partId)
{
    _partId = partId;
}

void PartParameterTableModel::cloneData()
{
    QVariant invalidId;
    for(int i=0; i<_items.count(); ++i){
        PartParameterItem * item = _items[i];
        item->setId(invalidId);
    }
}
QAbstractListModel * PartParameterTableModel::relationModel(const int column) const
{
    if(column==ColumnParameter){        
        return new ParametersListModel(_parameters.values());
    }
    return 0;
}

int PartParameterTableModel::rowCount(const QModelIndex& /* parent */) const
{
    return _items.count();
}

int PartParameterTableModel::columnCount(const QModelIndex&/* parent*/) const
{
    return COLUMN_COUNT;
}


QVariant PartParameterTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Horizontal)
        {
            switch (section)
            {
            case ColumnParameter:
                return tr("Parameter");
            case ColumnParameterValue:
                return tr("Value");
            case ColumnParameterMinValue:
                return tr("Min. Value");
            case ColumnParameterMaxValue:
                return tr("Max. Value");
            default:
                break;
            }
        }
        else if (orientation == Qt::Vertical)
        {
            return section + 1;
        }
    }

    return QVariant();
}


Qt::ItemFlags PartParameterTableModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags theFlags = QAbstractTableModel::flags(index);
    if (index.isValid()){
        theFlags |= Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
        //Prevent edit of the value when the parameter is not set
        if(index.row()<_items.count() && index.column()==ColumnParameterValue){
            const PartParameterItem * item = _items.at(index.row());
            if(!item->param().valid()){
                theFlags &= ~(Qt::ItemIsEditable);
            }
        }
    }
    return theFlags;
}

/**
 * @brief Wraps numeric values in a ParameterValue so that the unit symbol is available for use by the editor
 * @param value
 * @param parameter
 * @return
 */
QVariant asEditableValue(const QVariant& value, const Parameter& parameter) {
    switch(parameter.datatype()){
    case Parameter::DECIMAL:
    case Parameter::INTEGER:
        return QVariant::fromValue(ParameterValue(value, parameter.unitSymbol()));
    case Parameter::TEXT:
    default:
        return value;
    }
}

QVariant asDisplayValue(const QVariant& value, const Parameter& parameter)  {
    switch(parameter.datatype()){

    case Parameter::DECIMAL:        
        if(!value.isNull() && value.canConvert(QVariant::Double)){
            return UnitFormatter::format(value.toDouble(), parameter.unitSymbol());
        }
        return QVariant();
    case Parameter::INTEGER:
        if(!value.isNull() && value.canConvert(QVariant::Double)){
            return UnitFormatter::format(qRound(value.toDouble()), parameter.unitSymbol());
        }
        return QVariant();
    default:
        return value;
    }
}

QVariant PartParameterTableModel::data(const QModelIndex &index, int role) const
{
    int column = index.column();
    if (!index.isValid() ||
        index.row() < 0 || index.row() >= _items.count() ||
        index.column() < 0 || column >= COLUMN_COUNT) {
        return QVariant();
    }
    if(role == Qt::DisplayRole) {
        const PartParameterItem * item = _items.at(index.row());
        switch(index.column()){
            case ColumnParameter:                
                return item->param().name();
            case ColumnParameterValue:
                return asDisplayValue(item->value(), item->param());
            case ColumnParameterMinValue:
                return asDisplayValue(item->minValue(), item->param());
            case ColumnParameterMaxValue:
                return asDisplayValue(item->maxValue(), item->param());
        }
    }
    else if (role == Qt::EditRole) {
        const PartParameterItem * item = _items.at(index.row());
        switch(index.column()){
            case ColumnParameter:
                return item->param().id();
            case ColumnParameterValue:
                return asEditableValue(item->value(), item->param());
            case ColumnParameterMinValue:
                return asEditableValue(item->minValue(), item->param());
            case ColumnParameterMaxValue:
                return asEditableValue(item->maxValue(), item->param());
        }
    }
    else if(role == Qt::ToolTipRole) {
        if(index.column()==ColumnParameter){
            const PartParameterItem * item = _items.at(index.row());
            return item->param().description();
        }
    }
    return QVariant();
}



bool PartParameterTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || role != Qt::EditRole ||
        index.row() < 0 || index.row() >= _items.count() ||
        index.column() < 0 || index.column() >= COLUMN_COUNT){
        return false;
    }

    bool ok;
    PartParameterItem * item = _items.at(index.row());
    if(index.column()==ColumnParameter){
        int paramId = value.toInt(&ok);
        if(ok){
            if(paramId!=item->param().id()){
                const Parameter& param = _parameters[paramId];
                item->setParam(param);
                emit dataChanged(index, index);
            }
        }
    }
    else if(index.column() == ColumnParameterValue){
        ok = item->setValue(value);
        emit dataChanged(index, index);
    }
    else if(index.column() == ColumnParameterMinValue){
        ok = item->setMinValue(value);
        emit dataChanged(index, index);
    }
    else if(index.column() == ColumnParameterMaxValue){
        ok = item->setMaxValue(value);
        emit dataChanged(index, index);
    }
    return ok;
}

bool PartParameterTableModel::insertRows(int row, int count, const QModelIndex&)
{
    beginInsertRows(QModelIndex(), row, row + count - 1);
    for (int i = 0; i < count; ++i) {
        PartParameterItem * item = new PartParameterItem();
        _items.insert(row, item);
    }
    endInsertRows();
    return true;
}

bool PartParameterTableModel::removeRows(int row, int count, const QModelIndex&)
{
    beginRemoveRows(QModelIndex(), row, row + count - 1);
    for (int i = 0; i < count; ++i) {
        const PartParameterItem * item = _items.at(row);
        if(item->id().isValid()){
            _deletedItems.append(item->id());
        }
        delete _items.takeAt(row);
    }
    endRemoveRows();
    return true;
}

void PartParameterTableModel::select()
{
    beginResetModel();
    qDeleteAll(_items);
    _items.clear();
    loadItems();
    endResetModel();
}

bool PartParameterTableModel::submitAll()
{
    while (!_deletedItems.isEmpty()){
        QVariant id = _deletedItems.takeFirst();
        qDebug()<<"Deleting item "<<id;
        deleteItem(id);
    }
    for(int i=0; i<_items.count(); ++i){
        PartParameterItem * item = _items[i];
        if(item->dirty()){
           if(saveItem(item)==false)
               return false;
        }
    }
    return true;
}

bool PartParameterTableModel::deleteItem(QVariant id)
{
    _deleteQuery.bindValue(0, id);
    bool res = _deleteQuery.exec();
    if(!res){
        qWarning()<<"Failed to execute delete query. Reason: "<<_deleteQuery.lastError().driverText();
    }
    return res;
}

bool PartParameterTableModel::saveItem(PartParameterItem* item)
{        
    if(item->id().isValid()){        
        //Edited item. Update
        qDebug()<<"Updating item "<<item->id() << "with:"
               << "partId=" << _partId
               << "parameter=" << item->param().id()
               << "value=" << item->value()
               << "minValue=" << item->minValue()
               << "maxValue=" << item->maxValue()
                  ;
        _updateQuery.bindValue(0,_partId);
        _updateQuery.bindValue(1,item->param().id());
        _updateQuery.bindValue(2,item->value());
        _updateQuery.bindValue(3,item->minValue());
        _updateQuery.bindValue(4,item->maxValue());
        _updateQuery.bindValue(5,item->id());
        if(_updateQuery.exec()){
            item->resetDirty();
            return true;
        }
        qWarning()<<"Failed to execute update query. Reason: "<<_updateQuery.lastError().driverText();
        return false;
    }
    else{        
        //Brand new item. Insert
        qDebug("Inserting new part parameter (partId=%d, paramId=%d, value=%s)",
               _partId.toInt(), item->param().id(), qPrintable(item->value().toString()));

        _insertQuery.bindValue(0,_partId);
        _insertQuery.bindValue(1,item->param().id());
        _insertQuery.bindValue(2,item->value());
        _insertQuery.bindValue(3,item->minValue());
        _insertQuery.bindValue(4,item->maxValue());

        if(_insertQuery.exec()){
            item->setId(_insertQuery.lastInsertId());
            item->resetDirty();
            return true;
        }
        qWarning()<<"Failed to execute insert query. Reason: "<<_insertQuery.lastError().driverText();
        return false;
    }
}

bool PartParameterTableModel::appendParameter(int paramId, const QVariant& value, const QVariant& minValue, const QVariant& maxValue)
{
    int row = rowCount();
    bool success = insertRow(row);
    if(success){
        QModelIndex paramIdx = index(row, ColumnParameter);
        success= success && setData(paramIdx, paramId, Qt::EditRole);

        QModelIndex paramValueIdx = index(row, ColumnParameterValue);
        success= success && setData(paramValueIdx, value, Qt::EditRole);

        QModelIndex paramMinValueIdx = index(row, ColumnParameterMinValue);
        success= success && setData(paramMinValueIdx, minValue, Qt::EditRole);

        QModelIndex paramMaxValueIdx = index(row, ColumnParameterMaxValue);
        success= success && setData(paramMaxValueIdx, maxValue, Qt::EditRole);
    }
    return success;
}

bool PartParameterTableModel::loadItems()
{
    if(!_partId.isValid())
        return false;
    qDebug()<<"loading items for "<<_partId;
    _selectQuery.bindValue(0, _partId);


    if(_selectQuery.exec()){
        while(_selectQuery.next())
        {
            //"SELECT id, parameter, value,  minValue, maxValue FROM part_parameter WHERE part=? ORDER BY id ASC");

            QVariant id = _selectQuery.value(0);
            QVariant parameterId = _selectQuery.value(1);
            QVariant value = _selectQuery.value(2);
            QVariant minValue = _selectQuery.value(3);
            QVariant maxValue = _selectQuery.value(4);
            const Parameter& param = _parameters[parameterId.toInt()];
            PartParameterItem * item = new PartParameterItem(id, param, value, minValue, maxValue);
            _items.append(item);
        }
        return true;
    }
    qWarning()<<"Failed to execute query. Reason "<<_selectQuery.lastError().driverText();
    return false;
}

static Parameter loadParameter(const QSqlQuery& query)
{
    QVariant id = query.value(0);
    QVariant name = query.value(1);
    QVariant datatype = query.value(2);
    QVariant description = query.value(3);
    QVariant symbol = query.value(4);
    Parameter param;
    param.setId(id.toInt());
    param.setName(name.toString());
    param.setDatatype(static_cast<Parameter::ParameterDatatypes>(datatype.toInt()));
    param.setDescription(description.toString());
    param.setUnitSymbol(symbol.toString());
    return param;
}

bool PartParameterTableModel::loadParameters()
{

    qDebug()<<"loading parameters ";
    QSqlQuery query("SELECT p.id, p.name, p.datatype, p.description, u.symbol "
                    "FROM parameter p "
                    "LEFT JOIN unit u "
                    "ON p.unit = u.id");
    if(query.exec()){
        while(query.next())
        {
            Parameter param = loadParameter(query);
            _parameters.insert(param.id(), param);
        }
        qDebug()<<"Loaded "<<_parameters.size()<<" parameters";
        return true;
    }
    else{
       qWarning()<<"Failed to execute query. Reason "<<query.lastError().driverText();
       return false;
    }
}

Parameter PartParameterTableModel::findParameter(const QString& key)
{
    QSqlQuery query("SELECT p.id, p.name, p.datatype, p.description, u.symbol "
                    "FROM parameter p "
                    "LEFT JOIN unit u "
                    "ON p.unit = u.id "
                    "WHERE p.key=?");
    query.bindValue(0, key);
    if(query.exec() && query.next()){
        return loadParameter(query);
    }
    return Parameter();
}


