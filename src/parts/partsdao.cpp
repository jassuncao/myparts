#include <dquest.h>
#include "entities/entities.h"
#include <QDebug>
#include <QSqlError>
#include "partsdao.h"
#include <QDebug>


PartsDAO::PartsDAO()
{
}

bool PartsDAO::addStock(QVariant partId, int quantity, double price, const QString & comment)
{
    Entities::PartEntity part;
    if(!part.load(DQWhere("id")==partId)){
        qWarning("PartEntity with id %d not found",partId.toInt());
        return false;
    }
    QDateTime now = QDateTime::currentDateTimeUtc();
    Entities::StockChangeEntity stockChange;    
    stockChange.change = quantity;
    stockChange.price = price;
    stockChange.comment = comment;
    stockChange.dateTime = now.toTime_t();
    stockChange.part = part.id;
    return stockChange.save();
}

bool PartsDAO::removeStock(QVariant partId, int quantity, const QString & comment)
{
    Entities::PartEntity part;
    if(!part.load(DQWhere("id")==partId)){
        qWarning("PartEntity with id %d not found",partId.toInt());
        return false;
    }
    QDateTime now = QDateTime::currentDateTimeUtc();
    Entities::StockChangeEntity stockChange;
    stockChange.change = quantity;
    stockChange.comment = comment;
    stockChange.dateTime = now.toTime_t();
    stockChange.part = part.id;
    return stockChange.save();
}
