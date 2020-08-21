#include "unittablemodel.h"
#include <QDebug>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QSqlField>
#include <QSqlError>
#include <QStringList>


//UnitTableModel::UnitTableModel(QObject *parent, QSqlDatabase db) :
//    QSqlTableModel(parent, db)
//{
//    setTable("unit");
//    _deletePrefixAssociationQuery.prepare("DELETE FROM unit_prefix WHERE unit_prefix.unit=?");
//    _insertPrefixAssociationQuery.prepare("INSERT OR IGNORE INTO unit_prefix (unit, siPrefix) VALUES(?,?)");
//}

UnitTableModel::~UnitTableModel()
{
}

QString UnitTableModel::selectStatement() const
{
    QString query("SELECT unit.id, unit.name, unit.symbol, group_concat(unit_prefix.siPrefix) AS prefixes "
                  "FROM unit LEFT JOIN unit_prefix ON unit.id = unit_prefix.unit");

    if(!filter().isEmpty()){
        query.append(QLatin1String(" WHERE ")).append(filter());
    }
    query.append(" GROUP BY unit.id");
    QString orderBy = orderByClause();
    if (!orderBy.isEmpty())
        query = query.append(QLatin1Char(' ')).append(orderBy);
#ifdef QT_DEBUG
    qDebug()<<"Query is "<<query;
#endif
    return query;
}

QVariant UnitTableModel::data(const QModelIndex &idx, int role) const
{

    if(idx.column()==ColumnPrefixes && role==Qt::EditRole){
        //Convert a comma separated list of prefixes to a QVariantList
        QVariant value = QSqlTableModel::data(idx, role);
        if(value.isValid() && value.canConvert(QVariant::String)){
            QVariantList res;
            QStringList list = value.toString().split(",",QString::SkipEmptyParts);
            QString var;
            foreach (var, list) {
                res.append(var.toLongLong());
            }
            return res;
        }
    }
    return QSqlTableModel::data(idx, role);
}

bool UnitTableModel::insertRowIntoTable(const QSqlRecord &values)
{
    for(int i=0; i<values.count(); ++i){
        qDebug()<<"Value to insert "<<values.fieldName(i)<<" to "<< values.value(i);
    }
    QSqlRecord rec(values);
    for(int i=FAKE_COLUMNS_INDEX; i<rec.count();++i){
        rec.setGenerated(i,false);
    }
    bool res = QSqlTableModel::insertRowIntoTable(rec);
    if(!res){
        qWarning()<<"Error saving Unit:"<<lastError().text();
    }
    QVariant lastInsertedId =  query().lastInsertId();
    saveUnitPrefixes(lastInsertedId, values.value(ColumnPrefixes));
    return res;
}

bool UnitTableModel::updateRowInTable(int row, const QSqlRecord &values)
{
#ifdef QT_DEBUG
    for(int i=0; i<values.count(); ++i){
        qDebug()<<"Value to update "<<values.fieldName(i)<<" to "<< values.value(i);
    }   
#endif
    QSqlRecord rec(values);
    for(int i=FAKE_COLUMNS_INDEX; i<rec.count();++i){
        rec.setGenerated(i,false);
    }
    bool res = QSqlTableModel::updateRowInTable(row, rec);
    if(!res){
        qWarning()<<"Update Error in row "<<row<<" "<<lastError().text();
    }
    else{
        qDebug()<<"Update SUCCESS ";
    }    
    QVariant unitId = query().value(ColumnId);\
    qDebug()<<"Unit id is "<<unitId;

    _deletePrefixAssociationQuery.bindValue(0, unitId);
    if(!_deletePrefixAssociationQuery.exec()){
        qWarning()<<"Unable to delete previous associations "<<_deletePrefixAssociationQuery.lastError();
        setLastError(_deletePrefixAssociationQuery.lastError());
    }        
    saveUnitPrefixes(unitId, values.value(ColumnPrefixes));
    return res;
}

void UnitTableModel::saveUnitPrefixes(const QVariant & unitId, const QVariant & prefixesVar)
{
    if(prefixesVar.isValid() && prefixesVar.canConvert(QVariant::List)){
        _insertPrefixAssociationQuery.bindValue(0, unitId);
        QVariantList prefixes = prefixesVar.toList();
        QVariant prefix;
        foreach (prefix, prefixes) {
            qDebug()<<"Creating unit_prefix "<<unitId<<" "<<prefix;
            _insertPrefixAssociationQuery.bindValue(1, prefix);
            if(!_insertPrefixAssociationQuery.exec()){
                qWarning()<<"Unable to create association "<<_insertPrefixAssociationQuery.lastError();
                setLastError(_deletePrefixAssociationQuery.lastError());
            }
        }
    }
}

