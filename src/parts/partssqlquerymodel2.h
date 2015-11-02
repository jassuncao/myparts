#ifndef PARTSSQLQUERYMODEL2_H
#define PARTSSQLQUERYMODEL2_H

#include <QSqlRelationalTableModel>
#include <QVector>
#include <QDate>

class PartsSqlQueryModel2 : public QSqlTableModel
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
        ColumnFootprintName
    };
    static const int VISIBILITY_COLUMN_ROLE = Qt::UserRole+1;
    static const int FAKE_COLUMNS_INDEX = ColumnPartUnit;    

    explicit PartsSqlQueryModel2(QObject *parent = 0);
    QString selectStatement() const;
    bool insertRowIntoTable(const QSqlRecord &values);
    bool updateRowInTable(int row, const QSqlRecord &values);
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QStringList mimeTypes() const;
    QMimeData *mimeData(const QModelIndexList &indexes) const;
    Qt::DropActions supportedDropActions() const;
    inline QVariant lastInsertedId() const {return _lastInsertedId;}
signals:
    
public slots:

protected:
    //virtual bool deleteRowFromTable(int row);
private:
    void setColumnName(int section, const QString & columnName);
    QVariant _lastInsertedId;
};

class FilterBuilder {

public:
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

    explicit FilterBuilder();

    QString build() const;
    void setCategoryFilterMode(CategoryFilterMode mode);
    void setStorageLocationFilterMode(StorageLocationFilterMode mode);
    CategoryFilterMode categoryFilterMode() const;
    StorageLocationFilterMode storageLocationFilterMode() const;
    void setSelectedCategories(const QVector<QVariant> & selectedCategories);    
    void setSelectedStorageLocations(const QVector<QVariant> & selectedStorageLocations);
    void setStockFilterMode(StockFilterMode mode);
    void setTextFilter(const QString & textFilter);
    void setDateFilterMode(DateFilterMode dateFilterMode);
    void setSelectedDate(const QDate & date);
    void setFilterByConditionEnabled(bool value);
    void setSelectedCondition(QVariant conditionId);

private:
    QString _textFilter;
    CategoryFilterMode _categoryFilterMode;
    QVector<QVariant> _selectedCategories;
    StorageLocationFilterMode _storageLocationFilterMode;
    QVector<QVariant> _selectedStorageLocations;
    StockFilterMode _stockFilterMode;
    DateFilterMode _dateFilterMode;
    QDateTime _selectedDateUtc;
    bool _filterByCondition;
    QVariant _selectedConditionId;
};


#endif // PARTSSQLQUERYMODEL2_H
