#ifndef PARTPARAMETERTABLEMODEL_H
#define PARTPARAMETERTABLEMODEL_H

#include <QObject>
#include <QList>
#include <QAbstractTableModel>
#include <QHash>
#include <QSqlQuery>
#include "parameter/parameter.h"
#include "core/sql/modelwithforeignkey.h"

struct PartParameterItem;

class PartParameterTableModel : public QAbstractTableModel, public IModelWithForeignKey
{    
    Q_OBJECT
public:
    enum ColumnsIndex {
        ColumnParameter,
        ColumnParameterMinValue,
        ColumnParameterValue,        
        ColumnParameterMaxValue,
    };
    explicit PartParameterTableModel(QObject *parent = 0);
    virtual ~PartParameterTableModel();
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    virtual QVariant data(const QModelIndex& index, int role) const;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role);
    bool insertRows(int row, int count, const QModelIndex&);
    bool removeRows(int row, int count, const QModelIndex&);
    void select();
    bool submitAll();
    void setCurrentPartId(const QVariant& partId);
    void cloneData();
    QAbstractListModel * relationModel(const int column) const;
    bool appendParameter(int paramId, const QVariant& value, const QVariant &minValue = QVariant(), const QVariant &maxValue = QVariant());
protected:
    bool loadParameters();
    virtual bool deleteItem(QVariant id);
    virtual bool saveItem(PartParameterItem* item);
    virtual bool loadItems();
private:
    QVariant _partId;
    QList<PartParameterItem*> _items;
    QList<QVariant> _deletedItems;
    QHash<int, Parameter> _parameters;
    QSqlQuery _selectQuery;
    QSqlQuery _insertQuery;
    QSqlQuery _updateQuery;
    QSqlQuery _deleteQuery;
public:
    static Parameter findParameter(const QString& key);
};





#endif // PARTPARAMETERTABLEMODEL_H
