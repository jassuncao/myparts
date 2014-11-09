#ifndef PARTSSQLQUERYMODEL_H
#define PARTSSQLQUERYMODEL_H

#include <QSqlQueryModel>
#include <QSqlRecord>
#include <QStringList>
#include <QMimeData>
#include <QVector>
#include <QVariant>

namespace Parts {
    enum ColumnsIndex {
        ColumnIndex = 0,
        ColumnName,
        ColumnDescription,
        ColumnStock,
        ColumnMinStock,
        ColumnCustomPartNumber,
        ColumnComment,
        ColumnCreateDate,
        ColumnCategory,
        ColumnStorage,
        ColumnCondition
    };

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
}


class PartsSqlQueryModel : public QSqlQueryModel
{
    Q_OBJECT
public:
    explicit PartsSqlQueryModel(QObject *parent = 0);
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QStringList mimeTypes() const;
    QMimeData *mimeData(const QModelIndexList &indexes) const;
    Qt::DropActions supportedDropActions() const;
    void setDatabase(const QSqlDatabase & db);
    
public slots:
    void select();
    virtual void setSort (int column, Qt::SortOrder order);
    virtual void sort (int column, Qt::SortOrder order);

    void setCategoryFilteringMode(Parts::CategoryFilteringMode mode);
    Parts::CategoryFilteringMode categoryFilteringMode() const;
    void setSelectedCategories(const QVector<QVariant> & selectedCategories);
    void setFilterByStorage(bool value);
    void setSelectedStorage(QVariant storageId);
    void setStockFilteringMode(Parts::StockFilteringMode mode);
    void setTextFilter(const QString & textFilter);
private:
    QString _queryClause;
    QSqlDatabase _queryDB;
    int _sortKeyColumn;
    Qt::SortOrder _sortOrder;

    QString _textFilter;

    Parts::CategoryFilteringMode _categoryFilteringMode;
    QVector<QVariant> _selectedCategories;
    //QVariant _selectedCategoryId;

    bool _filterByStorage;
    QVariant _selectedStorageId;

    Parts::StockFilteringMode _stockFilteringMode;
};

inline void PartsSqlQueryModel::setDatabase(const QSqlDatabase &db)
{
    _queryDB = db;
}

inline void PartsSqlQueryModel::setCategoryFilteringMode(Parts::CategoryFilteringMode mode)
{
    _categoryFilteringMode = mode;
}

/*
inline void PartsSqlQueryModel::setSelectedCategory(QVariant categoryId)
{
    _selectedCategories = QVector<QVariant>(1,categoryId);
    //_selectedCategoryId = categoryId;
}
*/

inline void PartsSqlQueryModel::setSelectedCategories(const QVector<QVariant> & selectedCategories)
{
    _selectedCategories = selectedCategories;
}

inline void PartsSqlQueryModel::setFilterByStorage(bool value)
{
    _filterByStorage = value;
}

inline void PartsSqlQueryModel::setSelectedStorage(QVariant storageId)
{
    _selectedStorageId = storageId;
}

inline void PartsSqlQueryModel::setStockFilteringMode(Parts::StockFilteringMode mode)
{
    _stockFilteringMode = mode;
}

inline void PartsSqlQueryModel::setTextFilter(const QString &textFilter)
{
    _textFilter = textFilter;
}

inline Parts::CategoryFilteringMode PartsSqlQueryModel::categoryFilteringMode() const
{
    return _categoryFilteringMode;
}

#endif // PARTSSQLQUERYMODEL_H
