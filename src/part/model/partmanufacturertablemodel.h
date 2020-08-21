#ifndef PARTMANUFACTURERTABLEMODEL_H
#define PARTMANUFACTURERTABLEMODEL_H

#include "core/sql/customtablemodel.h"

class PartManufacturerTableModel : public SimpleSqlTableModel
{
    Q_OBJECT
public:

    enum ColumnsIndex {
        ColumnManufacturer,
        ColumnPartNumber
    };

    inline void setCurrentPartId(const QVariant & partId) { setCurrentForeignKey(partId); }

    static PartManufacturerTableModel * createNew(QObject *parent);
protected:
    explicit PartManufacturerTableModel(const QStringList & fieldNames, const QStringList & columnNames, QObject *parent = 0);
};

#endif // PARTMANUFACTURERTABLEMODEL_H
