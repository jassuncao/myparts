#include "partunitcache.h"

#include <QSqlQuery>
#include <QVariant>
#include <QStandardItemModel>

struct PartUnitData
{
    PartUnitData(int id, const QString & name, const QString & abbreviation): id(id), name(name), abbreviation(abbreviation)
    {}
    int id;
    QString name;
    QString abbreviation;
};

PartUnitCache::PartUnitCache() : _model(0)
{   
}

PartUnitCache::~PartUnitCache()
{
    qDeleteAll(_entries);
    _entries.clear();
    _entriesHash.clear();
}

void PartUnitCache::refresh()
{    
    invalidate();
    QSqlQuery query("SELECT id, name, abbreviation, defaultUnit FROM part_unit ORDER BY name ASC");
    while(query.next()){
        int id = query.value(0).toInt();
        QString name = query.value(1).toString();
        QString abbreviation = query.value(2).toString();          
        PartUnitData * data = new PartUnitData(id, name, abbreviation);
        _entries.append(data);
        _entriesHash[id] = data;
    }
}

void PartUnitCache::invalidate()
{
    qDeleteAll(_entries);
    _entries.clear();
    _entriesHash.clear();
}

QAbstractItemModel *PartUnitCache::readOnlyModel()
{
    if(_model){
        return _model;
    }
    if(_entries.isEmpty()){
        refresh();
    }
    _model = new QStandardItemModel;    
    QStringList header;
    header << "Id" << "Name" << "Abbreviation";
    _model->setHorizontalHeaderLabels(header);
    QList<QStandardItem*> items;
    for (int row = 0; row < _entries.size(); ++row) {        
        PartUnitData * data = _entries.at(row);
        QStandardItem * idItem = new QStandardItem();
        idItem->setData(data->id);
        idItem->setEditable(false);
        items.append(idItem);

        QStandardItem * nameItem = new QStandardItem(data->name);
        nameItem->setEditable(false);
        items.append(nameItem);

        QStandardItem * abbreviationItem = new QStandardItem(data->abbreviation);
        abbreviationItem->setEditable(false);
        items.append(abbreviationItem);

        _model->appendRow(items);                
        items.clear();
     }    
    return _model;
}

QString PartUnitCache::getAbbreviation(const QVariant &unitId)
{
    bool ok;
    const int id = unitId.toInt(&ok);
    if(!ok){
       return QString();
    }
    if(_entries.isEmpty()){
        refresh();
    }
    PartUnitData * data = _entriesHash.value(id, 0);
    return data ? data->abbreviation : QString();
    /*
    for (int row = 0; row < _entries.size(); ++row) {
        PartUnitData * data = _entries.at(row);
        if(id == data->id){
            return data->abbreviation;
        }
    }
    */
}

