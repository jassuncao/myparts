#ifndef PARTSSQLTABLEMODEL_H
#define PARTSSQLTABLEMODEL_H

#include <QSqlTableModel>

class PartsQueryBuilder;

class PartsSqlTableModel : public QSqlTableModel
{
    Q_OBJECT
public:    
    enum ColumnsIndex {
        ColumnId = 0,
        ColumnName,
        ColumnDescription,
        ColumnTotalStock,
        ColumnMinStock,
        ColumnAvgPrice,
        ColumnComment,
        ColumnCustomPartNumber,
        ColumnCreateDate,
        ColumnPartUnitId,
        ColumnCategoryId,
        //ColumnStorageId,
        //ColumnConditionId,
        ColumnPackageId,
        ColumnPartUnit,
        ColumnCategoryName,
        //ColumnStorage,
        //ColumnCondition,
        ColumnPackageName,
        LAST_COLUMN
    };

    static const int VISIBILITY_COLUMN_ROLE = Qt::UserRole+1;    
    static const int FAKE_COLUMNS_INDEX = ColumnPartUnit;

    explicit PartsSqlTableModel(PartsQueryBuilder * partsQueryBuilder, QObject *parent = 0);
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
    bool updatePartAvgPrice(const QModelIndex &currentIndex, double partPrice);
    bool updatePartStock(const QModelIndex & currentIndex, int stockChange);

    //Sets the part stock and logs an empty entry in stock_change.
    //bool setPartStock(const QModelIndex & currentIndex, int newStockValue);
    void updatePartsCategory(QVector<int> parts, int categoryId);
    //void updatePartsStorage(QVector<int> parts, int storageId);
    void setSort(int column, Qt::SortOrder order);
    QModelIndex findIndex(QVariant partId);
    PartsQueryBuilder * queryBuilder() const;
Q_SIGNALS:
    void beforeSubmit();
    void afterSubmit();
public Q_SLOTS:
    bool submitAll();
    void filterChanged();
private:
    PartsQueryBuilder * _partsQueryBuilder;
    void setColumnName(int section, const QString & columnName);
    QVariant _lastInsertedId;   
};

#endif // PARTSSQLTABLEMODEL_H
