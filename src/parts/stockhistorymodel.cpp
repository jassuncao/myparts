#include "stockhistorymodel.h"
#include <QColor>
#include <QSqlQuery>
#include <QDebug>
#include <QSqlError>


static QString ASC("ASC");
static QString DESC("DESC");

StockHistoryModel::StockHistoryModel(QObject *parent) :
    QSqlQueryModel(parent),
    _sortKeyColumn(ColumnDateTime)
{  
}

void StockHistoryModel::setSelectedPart(const QVariant & partId)
{
    _partId = partId;
    select();
}

void StockHistoryModel::select()
{
    QString dir = _sortOrder==Qt::AscendingOrder ? ASC : DESC ;
    QString stmt = QString("SELECT change>0 AS action, dateTime, change, price, comment FROM "
            "stock_change WHERE part=:partId ORDER BY %1 %2").arg(QString::number(_sortKeyColumn+1),dir);
    qDebug("Used query '%s'", qPrintable(stmt));
    QSqlQuery query;
    query.prepare(stmt);
    query.bindValue(0,_partId);    
    if(!query.exec()){
        qWarning()<<"Failed to execute parts query: "<<query.lastError();
    }    
    setQuery(query);
}

void StockHistoryModel::setSort(int column, Qt::SortOrder order)
{
    _sortKeyColumn = column;
    _sortOrder = order;
}


void StockHistoryModel::sort(int column, Qt::SortOrder order)
{
    if ((_sortKeyColumn != column) || (_sortOrder != order)) {
        setSort(column, order);
        select();
    }
}
