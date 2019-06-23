#ifndef PARTSTOCKTABLEMODEL_H
#define PARTSTOCKTABLEMODEL_H

#include "customtablemodel.h"

class PartStockLogTableModel : public SimpleSqlTableModel
{
    Q_OBJECT
public:
    enum ColumnsIndex {
        ColumnLastUpdate,
        ColumnChange,
        ColumnPrice,
        ColumnComment
    };

    inline void setCurrentPartId(const QVariant & partId) { setCurrentForeignKey(partId); }
    bool appendRow(const QVariant &quantity, const QVariant & price, const QString & comment);
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant computeAveragePrice() const;
    static PartStockLogTableModel * createNew(QObject *parent);
    bool rawInsert(const QVariant & partId, const QVariant & quantity, const QVariant & price, const QString & comment);
protected:
    explicit PartStockLogTableModel(const QStringList & fieldNames, const QStringList & columnNames, QObject *parent = 0);
};

#endif // PARTSTOCKTABLEMODEL_H
