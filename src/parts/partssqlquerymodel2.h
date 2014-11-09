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
        ColumnPartUnit,
        ColumnCategoryName,
        ColumnStorage,
        ColumnCondition
    };

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
    QVariant _lastInsertedId;
};

class FilterBuilder {

public:
    enum CategoryFilteringMode {
        AllCategories=1,
        SubCategories=2,
        SelectedCategory=3
    };

    enum StockFilteringMode {
        AnyStockLevel,
        StockLevelZero,
        StockLevelGreaterZero,
        StockLevelBelowMin
    };

    enum DateFilteringMode {
        DateFilterNone,
        DateFilterBefore,
        DateFilterOn,
        DateFilterAfter
    };

    explicit FilterBuilder();

    QString build() const;
    void setCategoryFilteringMode(CategoryFilteringMode mode);
    CategoryFilteringMode categoryFilteringMode() const;
    void setSelectedCategories(const QVector<QVariant> & selectedCategories);
    void setFilterByStorage(bool value);
    void setSelectedStorage(QVariant storageId);
    void setStockFilteringMode(StockFilteringMode mode);
    void setTextFilter(const QString & textFilter);
    void setDateFilterMode(DateFilteringMode dateFilterMode);
    void setSelectedDate(const QDate & date);
    void setFilterByCondition(bool value);
    void setSelectedCondition(QVariant conditionId);

private:
    QString _textFilter;
    CategoryFilteringMode _categoryFilteringMode;
    QVector<QVariant> _selectedCategories;
    bool _filterByStorage;
    QVariant _selectedStorageId;
    StockFilteringMode _stockFilteringMode;
    DateFilteringMode _dateFilterMode;
    QDateTime _selectedDateUtc;
    bool _filterByCondition;
    QVariant _selectedConditionId;
};

#endif // PARTSSQLQUERYMODEL2_H
