#include "parametermodel.h"
#include "util/proxymodelnoneentry.h"
#include <QAbstractListModel>
#include <QPair>

enum DatatypeEnum {
    DATATYPE_INTEGER = 1,
    DATATYPE_DECIMAL = 2,
    DATATYPE_TEXT = 3
};

#define MAX_DATATYPE DATATYPE_TEXT

typedef QPair<int,QVariant> DatatypePair;

class DatatypeRelation: public QAbstractListModel, public ModelRelation {
public:
    explicit DatatypeRelation(QObject *parent = 0) : QAbstractListModel(parent)
    {
        _items.append(DatatypePair(DATATYPE_INTEGER, displayValue(DATATYPE_INTEGER)));
        _items.append(DatatypePair(DATATYPE_DECIMAL, displayValue(DATATYPE_DECIMAL)));
        _items.append(DatatypePair(DATATYPE_TEXT, displayValue(DATATYPE_TEXT)));
    }

    virtual ~DatatypeRelation(){
    }

    virtual int rowCount(const QModelIndex &) const
    {
        return _items.size();
    }

    virtual QVariant data(const QModelIndex &index, int role) const
    {
        if (index.row() < 0 || index.row() >= _items.size())
            return QVariant();
        if (role == Qt::DisplayRole)
            return _items.at(index.row()).second;
        if (role == Qt::EditRole)
            return _items.at(index.row()).first;
        return QVariant();
    }

    virtual QVariant displayValue(const QVariant foreignId) const
    {
        DatatypeEnum datatype = static_cast<DatatypeEnum>(foreignId.toInt());
        switch(datatype){
        case DATATYPE_INTEGER:
            return tr("Integer");
        case DATATYPE_DECIMAL:
            return tr("Decimal");
        case DATATYPE_TEXT:
            return tr("Text");
        default:
            return QVariant();
        }
    }

    virtual bool validId(const QVariant foreignId) const
    {
        if(!foreignId.isValid())
            return false;
        int id = foreignId.toInt();
        return id>=DATATYPE_INTEGER && id<=MAX_DATATYPE;
    }
    virtual bool initialized() const {
        return true;
    }
    virtual void populateDictionary() {}
    virtual QAbstractListModel * model()
    {
        return this;
    }

private:
    QList<DatatypePair > _items;
};

ParameterModel::ParameterModel(const QStringList &fieldNames, const QStringList &columnNames, QObject *parent)
    : SimpleSqlTableModel("parameter", fieldNames, columnNames, QString(), parent)
{
    registerRelation(ColumnUnit, new TableRelation("unit", "id", "name", true));
    registerRelation(ColumnDatatype, new DatatypeRelation());
}

ParameterModel * ParameterModel::createNew(QObject * parent)
{
    QStringList fieldNames;
    QStringList columnNames;
    fieldNames<<"name"<<"key"<<"datatype"<<"unit"<<"description"<<"deletable";
    columnNames<<tr("Name")<<tr("Key")<<tr("Datatype")<<tr("Unit")<<tr("Description");
    return new ParameterModel(fieldNames, columnNames, parent);
}

TableItem * ParameterModel::createBlankItem() const
{
    TableItem* item = new TableItem(QVector<QVariant>(_maxColumns));
    item->setData(ColumnDeletable, QVariant(true));
    return item;
}

QAbstractItemModel * ParameterModel::relationModel(const int column) const
{    
    QAbstractItemModel * listModel = SimpleSqlTableModel::relationModel(column);
    if(column==ColumnUnit){
         ProxyModelNoneEntry * proxyModel = new ProxyModelNoneEntry(QString(), QVariant(), listModel);
         proxyModel->setSourceModel(listModel);
         return proxyModel;
    }
    else{
        return listModel;
    }
}
