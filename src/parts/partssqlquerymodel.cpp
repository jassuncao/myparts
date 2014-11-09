#include "partssqlquerymodel.h"
#include <QVector>
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>

PartsSqlQueryModel::PartsSqlQueryModel(QObject *parent) :
    QSqlQueryModel(parent),
    _queryDB(QSqlDatabase::database()),
    _sortKeyColumn(-1),
    _sortOrder(Qt::AscendingOrder),
    _categoryFilteringMode(Parts::AllCategories),
    _filterByStorage(false),
    _stockFilteringMode(Parts::AnyStockLevel)
{
    /*
    setHeaderData(Parts::ColumnName, Qt::Horizontal, tr("Name"));
    setHeaderData(Parts::ColumnDescription, Qt::Horizontal, tr("Description"));
    setHeaderData(Parts::ColumnStock, Qt::Horizontal, tr("Stock"));
    setHeaderData(Parts::ColumnMinStock, Qt::Horizontal, tr("Min. Stock"));
    setHeaderData(Parts::ColumnCategory, Qt::Horizontal, tr("Category"));
    setHeaderData(Parts::ColumnStorage, Qt::Horizontal, tr("Storage Location"));
    */
}


Qt::ItemFlags PartsSqlQueryModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flags = QSqlQueryModel::flags(index);
    flags = flags | Qt::ItemIsDragEnabled;
    return flags;
}

Qt::DropActions PartsSqlQueryModel::supportedDropActions() const
{
    return Qt::MoveAction | Qt::CopyAction;
}

QStringList PartsSqlQueryModel::mimeTypes() const
{
    QStringList types;
    types << "myparts/part" << "text/plain" << "text/html";
    return types;
}

QMimeData * PartsSqlQueryModel::mimeData(const QModelIndexList &indexes) const
{
    QMimeData * mimeData = new QMimeData;
    QByteArray encodedData;
    QString textData;
    textData.reserve(indexes.size()*64);
    int columns = columnCount();
    for(int i=0; i< columns; ++i){
        textData.append(headerData(i, Qt::Horizontal).toString());
        if(i<columns-1)
            textData.append('\t');
    }
    textData.append('\n');

    QDataStream stream(&encodedData, QIODevice::WriteOnly);
    QVector<int> partIds;
    foreach (const QModelIndex &index, indexes) {
        if (index.isValid()) {
            QSqlRecord recordData = record(index.row());
            QVariant partId = recordData.value(0);
            partIds.append(partId.toInt());
            qDebug("Adding row");
            for(int i=0; i< columns; ++i){
                textData.append(recordData.value(i).toString());
                if(i<columns-1)
                    textData.append('\t');
            }
            textData.append('\n');
        }
    }
    stream << partIds;
    mimeData->setData("myparts/part", encodedData);
    mimeData->setText(textData);
    return mimeData;
}

void PartsSqlQueryModel::select()
{

    if (!_queryDB.isValid())
        return;

    QString query("SELECT p.id, p.name, p.description, p.actualStock, p.minimumStock, p.customPartNumber, p.comment, datetime(p.createDate, 'unixepoch'), c.name, s.name, cond.value "
                    "FROM part p INNER JOIN part_category c ON p.category=c.id "
                    "LEFT JOIN part_storage s ON p.storage=s.id "
                    "LEFT JOIN part_condition cond ON p.condition=cond.id ");

    QStringList criterium;
    QVector<QVariant> arguments;

    if(!_textFilter.isEmpty()){
        criterium.append("(p.name LIKE ? OR p.description LIKE ?)");
        //Workaround to solve an issue with the use of % in the query.
        QString aux = QString("\%%1\%").arg(_textFilter);
        arguments.append(aux);
        arguments.append(aux);
    }

    qDebug("Mode is %d",_categoryFilteringMode);
    switch(_categoryFilteringMode){
    case Parts::AllCategories:
        break;
    case Parts::SubCategories:
        if(_selectedCategories.size()>0){
            QString inStatement("c.id IN (?");
            arguments.append(_selectedCategories.at(0));
            for(int i=1; i<_selectedCategories.size();++i){
                inStatement.append(",?");
                arguments.append(_selectedCategories.at(i));
            }            
            inStatement.append(")");
            criterium.append(inStatement);
        }
        break;
    case Parts::SelectedCategory:
        if(_selectedCategories.size()>0){
            criterium.append("c.id = ?");
            arguments.append(_selectedCategories.at(0));
        }
        break;
    }

    if(_filterByStorage && _selectedStorageId.isValid()){
        criterium.append("p.storage = ?");
        arguments.append(_selectedStorageId);
    }

    switch(_stockFilteringMode){
    case Parts::AnyStockLevel:
        break;
    case Parts::StockLevelZero:
        criterium.append("p.actualStock=0");
        break;
    case Parts::StockLevelGreaterZero:
        criterium.append("p.actualStock>0");
        break;
    case Parts::StockLevelBelowMin:
        criterium.append("p.actualStock<p.minimumStock");
        break;
    }

    QString whereClause;
    if(criterium.size()>0){
        whereClause.append("WHERE ").append(criterium.at(0));
        for(int i=1; i<criterium.size();++i){
            whereClause.append(" AND ").append(criterium.at(i));
        }
        whereClause.append(" ");
        query.append(" " + whereClause);
    }

/*
    if (!filterString.isEmpty() && !filterColumn.isEmpty()) {
        QString whereClause;
        QString esFilterString = filterString;
        QString esFilterColumn = filterColumn;

        if (filterFlags & Qt::MatchExactly) {
            whereClause = "WHERE %1 = %2";
        } else if (filterFlags & Qt::MatchStartsWith) {
            whereClause = "WHERE %1 LIKE %2";
            esFilterString.append("*");
        } else if (filterFlags & Qt::MatchEndsWith) {
            whereClause = "WHERE %1 LIKE %2";
            esFilterString.prepend("*");
        } else if (filterFlags & Qt::MatchRegExp) {
            whereClause = "WHERE %1 REGEXP %2";
        } else if (filterFlags & Qt::MatchWildcard) {
            whereClause = "WHERE %1 LIKE %2";
        } else if (filterFlags & Qt::MatchContains) {
            whereClause = "WHERE %1 LIKE %2";
            esFilterString.append("*");
            esFilterString.prepend("*");
        } else { return; } // unhandled filterflag

        QSqlDriver *driver = queryDB.driver();
        esFilterColumn = driver->escapeIdentifier(filterColumn, QSqlDriver::FieldName);
        QSqlField field; field.setType(QVariant::String); field.setValue(esFilterString);
        esFilterString = driver->formatValue(field);
        if (filterFlags & Qt::MatchStartsWith || Qt::MatchEndsWith || Qt::MatchWildcard) {
            esFilterString = esFilterString.replace("%", "\\%");
        //if (filterFlags & Qt::MatchWildcard)
            esFilterString = esFilterString.replace("*", "%");
        }

        //whereClause.arg(esFilterColumn).arg(esFilterString); // don't know why it doesn't work
        whereClause = whereClause.replace("%1", esFilterColumn).replace("%2", esFilterString);
        query.append(" " + whereClause);
    }
    */

    if (_sortKeyColumn >= 0) {
        QString orderClause;
        orderClause = "ORDER BY " + QString::number(_sortKeyColumn+1) + " " + ((_sortOrder == Qt::AscendingOrder) ? "ASC" : "DESC");
        query.append(" " + orderClause);
    }
    qDebug("Used query '%s'", qPrintable(query));
    //qDebug()<<"Used query "<< query;
    QSqlQuery sqlQuery(query, _queryDB);
    for(int i=0; i< arguments.size();++i)
    {
        sqlQuery.bindValue(i,arguments.at(i));
        qDebug()<<"Bind value "<<i << " with "<<arguments.at(i);
    }
    if(!sqlQuery.exec()){
        qWarning()<<"Failed to execute parts query: "<<sqlQuery.lastError();
    }
    QSqlQueryModel::setQuery(sqlQuery);
}


void PartsSqlQueryModel::setSort(int column, Qt::SortOrder order)
{
    _sortKeyColumn = column;
    _sortOrder = order;
}


void PartsSqlQueryModel::sort(int column, Qt::SortOrder order)
{
    if ((_sortKeyColumn != column) || (_sortOrder != order)) {
        setSort(column, order);
        select();
    }
}
