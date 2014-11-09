#include "stockhistorymodel.h"
#include <QColor>
#include <QSqlQuery>
#include <QDebug>
#include <QSqlError>
#include <QDateTime>
#include <QSettings>


static QString ASC("ASC");
static QString DESC("DESC");

StockHistoryModel::StockHistoryModel(QObject *parent) :
    QSqlQueryModel(parent),
    _sortKeyColumn(ColumnDateTime)
{
    QString defaultCurrency(QChar(8364));
    QSettings settings;
    QString currency = settings.value("currency/symbol", defaultCurrency).toString();
    bool currencyAfter = settings.value("currency/after",true).toBool();
    if(currencyAfter){
        _currencyFormat = QString("%L2%1").arg(currency);
    }
    else{
        _currencyFormat = QString("%1%L2").arg(currency);
    }
    _numberAlignment = Qt::AlignVCenter | Qt::AlignTrailing;
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

QVariant StockHistoryModel::data(const QModelIndex &index, int role) const
{
    int col = index.column();
    switch(col){
    case ColumnPrice:
        if(role==Qt::DisplayRole){
            double price = QSqlQueryModel::data(index, Qt::DisplayRole).toDouble();
            return _currencyFormat.arg(price ,0,'f',2,' ');
        }
        else if(role == Qt::TextAlignmentRole){
            return _numberAlignment;
        }
        break;
    case ColumnChange:
        if(role == Qt::TextAlignmentRole){
            return _numberAlignment;
        }
        break;
    /*
    case ColumnAction:
        if(role==Qt::DisplayRole){
            return QSqlQueryModel::data(index, Qt::DisplayRole).toBool();
        }
        return QVariant();
        break;
        */
    case ColumnDateTime:
        if(role==Qt::DisplayRole){
            uint timeT = QSqlQueryModel::data(index, Qt::DisplayRole).toUInt();
            return QDateTime::fromTime_t(timeT).toString(Qt::SystemLocaleShortDate);
        }
        return QVariant();
    }
    return QSqlQueryModel::data(index,role);
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
