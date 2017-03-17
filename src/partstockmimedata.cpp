#include "partstockmimedata.h"

#include <QStringList>

const char * PartStockMimeData::PART_STOCK_MIMETYPE = "myparts/partStock";

PartStockItem::PartStockItem(QVariant aPartId, QVariant aStorageId, QVariant aConditionId, QVariant aQuantity) :
    partId(aPartId), storageId(aStorageId), conditionId(aConditionId), quantity(aQuantity)
{}

PartStockItem::PartStockItem()
{}

PartStockMimeData::PartStockMimeData()
{}

PartStockMimeData::PartStockMimeData(QList<PartStockItem> items) : QMimeData(),
    _items(items)
{}

QStringList PartStockMimeData::formats() const
{
    QStringList list;
    list.append(PART_STOCK_MIMETYPE);
    return list;
}
