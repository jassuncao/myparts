#ifndef PARTSTOCKTABLEMODEL2_H
#define PARTSTOCKTABLEMODEL2_H

#include "customtablemodel.h"

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
    bool insertOrUpdateRow(const QVariant & condition, const QVariant & storage, double quantiy);
    QVariant computeCurrentStock() const;
    static PartStockTableModel2 * createNew(QObject *parent);
protected:
    explicit PartStockTableModel2(const QStringList & fieldNames, const QStringList & columnNames, QObject *parent = 0);
    TableItem * createBlankItem() const;
    bool saveItem(TableItem* item);
};

#endif // PARTSTOCKTABLEMODEL2_H
