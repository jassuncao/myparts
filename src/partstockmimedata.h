#ifndef PARTSTOCKMIMEDATA_H
#define PARTSTOCKMIMEDATA_H

#include <QMimeData>
#include <QList>

struct PartStockItem {
    PartStockItem(QVariant aPartId, QVariant aStorageId, QVariant aConditionId, QVariant aQuantity);
    PartStockItem();
    QVariant partId;
    QVariant storageId;
    QVariant conditionId;
    QVariant quantity;
};

class PartStockMimeData : public QMimeData
{
    Q_OBJECT
public:
    static const char * PART_STOCK_MIMETYPE;
    explicit PartStockMimeData();
    explicit PartStockMimeData(QList<PartStockItem> items);
    QStringList formats() const;
    inline QList<PartStockItem> items() const { return _items; }
private:
    QList<PartStockItem> _items;
};

#endif // PARTSTOCKMIMEDATA_H
