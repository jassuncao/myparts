#include "qsortfiltersqlquerymodel.h"
#include <QtSql>
#include <QString>

QSortFilterSqlQueryModel::QSortFilterSqlQueryModel(QObject *parent) :
    QSqlQueryModel(parent)
{

}

void QSortFilterSqlQueryModel::setQuery(const QString &query, const QSqlDatabase &db)
{
    queryClause = query;
    queryDB = db;

    filterString.clear();
    //filterColumn.clear();
    filterFlags = Qt::MatchStartsWith;
    sortKeyColumn = -1;
    sortOrder = Qt::AscendingOrder;
}

Qt::ItemFlags QSortFilterSqlQueryModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flags = QSqlQueryModel::flags(index);
    flags = flags | Qt::ItemIsDragEnabled;
    return flags;
}

Qt::DropActions QSortFilterSqlQueryModel::supportedDropActions() const
{
    return Qt::MoveAction | Qt::CopyAction;
}

QStringList QSortFilterSqlQueryModel::mimeTypes() const
{
    QStringList types;
    types << "myparts/part" << "text/plain" << "text/html";
    return types;
}

QMimeData *QSortFilterSqlQueryModel::mimeData(const QModelIndexList &indexes) const
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
            //stream << partId;
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

/*
bool QSortFilterSqlQueryModel::removeRows(int row, int count, const QModelIndex &parent)
{
    beginRemoveRows(parent, row, row+count);
    endRemoveRows();
    return true;
}
*/

void QSortFilterSqlQueryModel::select()
{

    if (queryClause.isEmpty() || (!queryDB.isValid()))
        return;

    QString query = queryClause;

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

    if (sortKeyColumn >= 0) {
        QString orderClause;
        orderClause = "ORDER BY " + QString::number(sortKeyColumn+1) + " " + ((sortOrder == Qt::AscendingOrder) ? "ASC" : "DESC");
        //qDebug("orderClause=%s",orderClause.toLatin1().data());
        query.append(" " + orderClause);
    }

    QSqlQueryModel::setQuery(query, queryDB);
}


void QSortFilterSqlQueryModel::setSort(int column, Qt::SortOrder order)
{
    sortKeyColumn = column;
    sortOrder = order;
}


void QSortFilterSqlQueryModel::sort(int column, Qt::SortOrder order)
{
    if ((sortKeyColumn != column) || (sortOrder != order)) {
        setSort(column, order);
        select();
    }
}

void QSortFilterSqlQueryModel::setFilterColumn(const QString &column)
{
    filterColumn = column;
}

void QSortFilterSqlQueryModel::setFilter(const QString &filter) {
    filterString = filter;
}

void QSortFilterSqlQueryModel::setFilterFlags(const Qt::MatchFlag flags)
{
    filterFlags = flags;
}

void QSortFilterSqlQueryModel::filter(const QString &filter)
{
    if (filterString != filter) {
        setFilter(filter);
        select();
    }
}
