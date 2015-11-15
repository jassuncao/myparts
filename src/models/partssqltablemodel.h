#ifndef PARTSSQLTABLEMODEL_H
#define PARTSSQLTABLEMODEL_H

#include <QSqlTableModel>
#include <QVector>
#include <QDate>

class PartsSqlTableModel : public QSqlTableModel
{
    Q_OBJECT
public:    
    enum ColumnsIndex {
        ColumnId = 0,
        ColumnName,
        ColumnDescription,
        ColumnActualStock,
        ColumnMinStock,
        ColumnAvgPrice,
        ColumnComment,
        ColumnCustomPartNumber,
        ColumnCreateDate,
        ColumnPartUnitId,
        ColumnCategoryId,
        ColumnStorageId,
        ColumnConditionId,
        ColumnFootprintId,
        ColumnPartUnit,
        ColumnCategoryName,
        ColumnStorage,
        ColumnCondition,
        ColumnFootprintName,
        LAST_COLUMN
    };

    enum SuportedFilters {
        FilterByStock,
        FilterByCondition,
        FilterByDistributor,
        FilterByManufacturer,
        FilterByFootprint,
        FilterByText,
    };

    enum CategoryFilterMode {
        AllCategories=1,
        SubCategories=2,
        SelectedCategory=3
    };

    enum StorageLocationFilterMode {
        StorageLocationFilterAll=1,
        StorageLocationFilterSub=2,
        StorageLocationFilterSelected=3
    };

    enum StockFilterMode {
        AnyStockLevel,
        StockLevelZero,
        StockLevelGreaterZero,
        StockLevelBelowMin
    };

    enum DateFilterMode {
        DateFilterNone,
        DateFilterBefore,
        DateFilterOn,
        DateFilterAfter
    };

    static const int VISIBILITY_COLUMN_ROLE = Qt::UserRole+1;
    static const int FAKE_COLUMNS_INDEX = ColumnPartUnit;

    explicit PartsSqlTableModel(QObject *parent = 0);
    int columnCount(const QModelIndex &index = QModelIndex()) const;
    QString selectStatement() const;
    bool insertRowIntoTable(const QSqlRecord &values);
    bool updateRowInTable(int row, const QSqlRecord &values);
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QStringList mimeTypes() const;
    QMimeData *mimeData(const QModelIndexList &indexes) const;
    Qt::DropActions supportedDropActions() const;
    inline QVariant lastInsertedId() const {return _lastInsertedId;}
    void setFilter(SuportedFilters filter, const QVariant & value);
signals:

private:
    void setColumnName(int section, const QString & columnName);
    const QLatin1String _baseSelectClause;
    const QLatin1String _distributorJoinClause;
    const QLatin1String _manufacturerJoinClause;

    QVariant _lastInsertedId;
    QVariant _textFilter;
    CategoryFilterMode _categoryFilterMode;
    QVector<QVariant> _selectedCategories;
    StorageLocationFilterMode _storageLocationFilterMode;
    QVector<QVariant> _selectedStorageLocations;
    StockFilterMode _stockFilterMode;
    DateFilterMode _dateFilterMode;
    QDateTime _selectedDateUtc;
    QVariant _selectedConditionId;
    QVariant _selectedFootprintId;
    QVariant _selectedDistributorId;
    QVariant _selectedManufacturerId;    
    QString _cachedSelectStatement;

};

#endif // PARTSSQLTABLEMODEL_H
