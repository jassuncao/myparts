#ifndef PARTSTOCKTABLEMODEL_H
#define PARTSTOCKTABLEMODEL_H

#include "customtablemodel.h"

class PartStockTableModel : public SimpleSqlTableModel
{
    Q_OBJECT
public:
    enum ColumnsIndex {
        ColumnDateTime,
        ColumnChange,
        ColumnPrice,
        ColumnComment
    };

    inline void setCurrentPartId(const QVariant & partId) { setCurrentForeignKey(partId); }
    static PartStockTableModel * createNew(QObject *parent);
    bool appendRow(const int quantity, const QVariant & price, const QString & comment);
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
protected:
    explicit PartStockTableModel(const QStringList & fieldNames, const QStringList & columnNames, QObject *parent = 0);
};

#endif // PARTSTOCKTABLEMODEL_H
