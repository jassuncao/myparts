#ifndef PARTSTOCKTABLEMODEL2_H
#define PARTSTOCKTABLEMODEL2_H

#include "core/sql/customtablemodel.h"
#include "stock/partstockmimedata.h"

class PartStockTableModel2 : public SimpleSqlTableModel
{
    Q_OBJECT
public:
    enum ColumnsIndex {
        ColumnStorage,
        ColumnCondition,
        ColumnQuantity,
        ColumnLastUpdate
    };

    inline void setCurrentPartId(const QVariant & partId) { setCurrentForeignKey(partId); }    
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    Qt::DropActions supportedDropActions() const;
    QStringList mimeTypes() const;
    QMimeData * mimeData(const QModelIndexList &indexes) const;
    bool insertOrUpdateRow(const QVariant & condition, const QVariant & storage, QVariant quantiy);
    QVariant computeCurrentStock() const;
    bool rawInsert(const QVariant & partId, const QVariant & condition, const QVariant & storage, QVariant quantiy);
    bool rawMoveStockToStorage(const PartStockItem & stockItem, const QVariant & newStorage);
    bool rawMovePartToStorage(int partId, const QVariant & newStorage);
    static PartStockTableModel2 * createNew(QObject *parent);    
protected:
    explicit PartStockTableModel2(const QStringList & fieldNames, const QStringList & columnNames, QObject *parent = 0);
    TableItem * createBlankItem() const;
    bool saveItem(TableItem* item);
};

#endif // PARTSTOCKTABLEMODEL2_H
