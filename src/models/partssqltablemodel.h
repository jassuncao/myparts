#ifndef PARTSSQLTABLEMODEL_H
#define PARTSSQLTABLEMODEL_H

#include <QSqlTableModel>
#include <QVector>
#include <QDate>
#include "filterbuilder.h"

/*
class Criterion
{
public:    
    virtual QString clause() const = 0;
    virtual QString joinClause() const;    
};

class StockCriterion : public Criterion
{
public:
    enum Mode {
        AnyStockLevel,
        StockLevelZero,
        StockLevelGreaterZero,
        StockLevelBelowMin
    };
    explicit StockCriterion(StockCriterion::Mode mode);
    QString clause() const;

private:
    const StockCriterion::Mode _mode;
};

class BasicForeignKeyCriterion : public Criterion
{
public:
    explicit BasicForeignKeyCriterion(const QString & name, const QVariant & value);
    QString clause() const;
private:
    const QString _foreignKeyName;
    const QVariant _foreignKeyValue;
};

class DistributorCriterion : public Criterion
{
public:
    explicit DistributorCriterion(QVariant & distributorId);
    QString clause() const;
    QString joinClause() const;
private:
    const QVariant _distributorId;
    const QLatin1String _joinClause;
};

class ManufacturerCriterion : public Criterion
{
public:
    explicit ManufacturerCriterion(QVariant & manufacturerId);
    QString clause() const;
    QString joinClause() const;
private:
    const QVariant _manufacturerId;
    const QLatin1String _joinClause;
};

class CreateDateCriterion : public Criterion
{
public:
    enum Mode {
        DateFilterNone,
        DateFilterBefore,
        DateFilterOn,
        DateFilterAfter
    };
    explicit CreateDateCriterion(CreateDateCriterion::Mode mode, const QDate & dateUtc);
    QString clause() const;
private:
    const CreateDateCriterion::Mode _mode;
    const QDateTime _dateTimeUtc;
};
*/

class SelectQueryBuilder;

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
        FilterByCategory,
        FilterByStorage,
    };

    /*
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
    */

    static const int VISIBILITY_COLUMN_ROLE = Qt::UserRole+1;
    static const int FAKE_COLUMNS_INDEX = ColumnPartUnit;

    explicit PartsSqlTableModel(QObject *parent = 0);
    ~PartsSqlTableModel();
    int columnCount(const QModelIndex &index = QModelIndex()) const;
    QString selectStatement() const;
    bool insertRowIntoTable(const QSqlRecord &values);
    bool updateRowInTable(int row, const QSqlRecord &values);
    QVariant data(const QModelIndex &idx, int role = Qt::DisplayRole) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QStringList mimeTypes() const;
    QMimeData *mimeData(const QModelIndexList &indexes) const;
    Qt::DropActions supportedDropActions() const;
    inline QVariant lastInsertedId() const {return _lastInsertedId;}
    void setCriterion(SuportedFilters kind, FilterCriterion * criterion);
    void unsetCriterion(SuportedFilters kind);
    //void setFilter(SuportedFilters filter, const QVariant & value);
    bool updatePartAvgPrice(const QModelIndex &currentIndex, double partPrice);
    bool updatePartStock(const QModelIndex & currentIndex, int stockChange);

private:
    SelectQueryBuilder * _selectQueryBuilder;
    void setColumnName(int section, const QString & columnName);
    QVariant _lastInsertedId;

    /*
    const QLatin1String _baseSelectClause;
    const QLatin1String _distributorJoinClause;
    const QLatin1String _manufacturerJoinClause;        
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
    */
};

#endif // PARTSSQLTABLEMODEL_H
