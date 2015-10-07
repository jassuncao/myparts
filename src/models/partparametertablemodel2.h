#ifndef PARTPARAMETERTABLEMODEL2_H
#define PARTPARAMETERTABLEMODEL2_H

#include <QAbstractTableModel>
#include <QList>
#include <QVector>
#include <QHash>
#include <QSqlQuery>
#include <QModelIndex>
#include "models/comboboxlookupmodel.h"

struct PartParameterItem {

    explicit PartParameterItem(int id=-1, const QString & name = QString(), const double value = 0,
                               const int unit = -1, const QString & description = QString(), const bool dirty=false) :
        id(id), name(name), value(value), unit(unit), description(description), dirty(dirty) {}

    int id;
    QString name;
    double value;
    int unit;
    QString description;
    bool dirty;
};

struct ParamUnitItem {
    explicit ParamUnitItem(int id=-1, const QString & name = QString(), const QString & symbol = QString()) :
        id(id), name(name), symbol(symbol) {}
    int id;
    QString name;
    QString symbol;
};

class ComboBoxLookupModel;

class PartParameterTableModel2 : public QAbstractTableModel
{
    Q_OBJECT
public:
    enum ColumnsIndex {
        ColumnName,
        ColumnValue,
        ColumnUnit,
        ColumnDescription,
    };

    explicit PartParameterTableModel2(QObject *parent = 0);
    ~PartParameterTableModel2();

    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    bool insertRows(int row, int count, const QModelIndex&);
    bool removeRows(int row, int count, const QModelIndex&);
    void load(const QVariant & partId);
    void save(const QVariant & partId);
    ComboBoxLookupModel * unitsModel() const;
private:
    void loadUnitsCache();
    QString getUnitName(int unitId) const;
    QVariant getValue(double value, int unitId) const;

    QVector<PartParameterItem> _items;
    QList<int> _deletedItems;
    QSqlQuery _selectQuery;
    QSqlQuery _deleteQuery;
    QSqlQuery _updateQuery;
    QSqlQuery _insertQuery;
    QSqlQuery _selectUnitsQuery;
    QHash<int, ParamUnitItem> _unitsCache;
    bool _unitsCacheReady;
    ComboBoxLookupModel * _lookupModel;
};

#endif // PARTPARAMETERTABLEMODEL2_H
