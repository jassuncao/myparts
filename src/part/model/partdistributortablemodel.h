#ifndef PARTDISTRIBUTORTABLEMODEL_H
#define PARTDISTRIBUTORTABLEMODEL_H

#include "core/sql/customtablemodel.h"

class PartDistributorTableModel :  public SimpleSqlTableModel
{
    Q_OBJECT
public:
    enum ColumnsIndex {
        ColumnDistributor,
        ColumnPartNumber,
        ColumnMinimumOrder,
        ColumnUnitPrice,
        ColumnCurrency,
        ColumnPackaging,
        ColumnDateTime
    };

    inline void setCurrentPartId(const QVariant & partId) { setCurrentForeignKey(partId); }
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role=Qt::EditRole);
    static PartDistributorTableModel * createNew(QObject *parent);
protected:
    explicit PartDistributorTableModel(const QStringList & fieldNames, const QStringList & columnNames, QObject *parent = 0);
    TableItem * createBlankItem() const;
};

#endif // PARTDISTRIBUTORTABLEMODEL_H
