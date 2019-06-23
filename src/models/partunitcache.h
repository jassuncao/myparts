#ifndef PARTUNITCACHE_H
#define PARTUNITCACHE_H

#include <QString>
#include <QList>
#include <QAbstractTableModel>
#include <QHash>

class QStandardItemModel;
struct PartUnitData;

class PartUnitCache
{
public:
    PartUnitCache();
    ~PartUnitCache();
    void refresh();
    void invalidate();
    QAbstractItemModel * readOnlyModel();
    QString getAbbreviation(const QVariant & unitId);
    //PartUnitData * defaultUnit
private:    
    QList<PartUnitData*> _entries;
    QHash<int,PartUnitData*> _entriesHash;
    QStandardItemModel * _model;
};

#endif // PARTUNITCACHE_H
