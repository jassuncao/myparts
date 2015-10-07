#ifndef UNITTABLEMODEL_H
#define UNITTABLEMODEL_H

#include <QSqlTableModel>
#include <QSqlQuery>

class UnitTableModel : public QSqlTableModel
{
    Q_OBJECT
public:
    enum ColumnsIndex {
        ColumnId = 0,
        ColumnName,
        ColumnSymbol,
        ColumnPrefixes
    };
    static const int FAKE_COLUMNS_INDEX = ColumnPrefixes;
    UnitTableModel(QObject *parent = 0, QSqlDatabase db = QSqlDatabase());
    ~UnitTableModel();
    virtual QString selectStatement() const;
    virtual QVariant data(const QModelIndex &idx, int role = Qt::DisplayRole) const;
    virtual bool insertRowIntoTable(const QSqlRecord &values);
    virtual bool updateRowInTable(int row, const QSqlRecord &values);
private:
    void updatePrefixAssociation();
    QSqlQuery _deletePrefixAssociationQuery;
    QSqlQuery _insertPrefixAssociationQuery;
    void saveUnitPrefixes(const QVariant & unitId, const QVariant &prefixesVar);
};

#endif // UNITTABLEMODEL_H
