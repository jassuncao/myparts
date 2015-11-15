#ifndef STOCKHISTORYMODEL_H
#define STOCKHISTORYMODEL_H

#include <QSqlQueryModel>

class StockHistoryModel : public QSqlQueryModel
{
    Q_OBJECT
public:

    enum ColumnsIndex {
        ColumnAction = 0,
        ColumnDateTime,
        ColumnChange,
        ColumnPrice,
        ColumnComment
    };

    explicit StockHistoryModel(QObject *parent = 0);
    void setSelectedPart(const QVariant & partId);
    virtual void setSort (int column, Qt::SortOrder order);
    virtual void sort (int column, Qt::SortOrder order);
signals:
    
public slots:
    void select();
private:
    int _sortKeyColumn;
    Qt::SortOrder _sortOrder;
    QVariant _partId;
};

#endif // STOCKHISTORYMODEL_H
