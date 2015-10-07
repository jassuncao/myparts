#include "partdistributortablemodel.h"
#include <QSqlRecord>
#include <QDebug>
#include <QSqlField>
#include <QSqlError>
#include <QSqlQuery>

PartDistributorTableModel::PartDistributorTableModel(QObject *parent, QSqlDatabase db)
    : QSqlRelationalTableModel(parent, db)
{
    setEditStrategy(QSqlTableModel::OnManualSubmit);
    setTable("part_distributor");
    setRelation(ColumnDistributorId,QSqlRelation("distributor","id","name"));
    setRelation(ColumnPackagingId,QSqlRelation("packaging","id","name"));
    setHeaderData(ColumnId, Qt::Horizontal, QObject::tr("ID"));
    setHeaderData(ColumnDistributorId, Qt::Horizontal, QObject::tr("Distributor"));
    setHeaderData(ColumnPartNumber, Qt::Horizontal, QObject::tr("Part Number"));
    setHeaderData(ColumnMinimumOrder, Qt::Horizontal, QObject::tr("Minimum Order"));
    setHeaderData(ColumnUnitPrice, Qt::Horizontal, QObject::tr("Unit Price"));
    setHeaderData(ColumnPackagingId, Qt::Horizontal, QObject::tr("Packaging"));
    connect(this, SIGNAL(primeInsert(int,QSqlRecord&)), this, SLOT(slotPrimeInsert(int,QSqlRecord&)));
    connect(this, SIGNAL(beforeInsert(QSqlRecord&)), this, SLOT(slotBeforeInsert(QSqlRecord&)));
}

PartDistributorTableModel::~PartDistributorTableModel()
{
}

void PartDistributorTableModel::copyTo(QVector<QSqlRecord> & dest)
{
    QSqlQuery q;
    q.prepare("SELECT distributor, partNumber, minimumOrder, unitPrice, packaging FROM part_distributor WHERE part=:partId");
    q.bindValue(0, _selectedPartId);
    if(q.exec()){
        while(q.next()){
            dest.append(q.record());
        }
    }
    else{
        qWarning("Failed to execute PartDistributorTableModel::copyTo query: %s", qPrintable(q.lastError().text()));
    }
}

void PartDistributorTableModel::copyFrom(const QVector<QSqlRecord> & source)
{
    insertRows(0, source.size());
    for(int row = 0; row<source.size();++row){
        const QSqlRecord & rec = source.at(row);
        setData(index(row, ColumnDistributorId), rec.value(0));
        setData(index(row, ColumnPartNumber), rec.value(1));
        setData(index(row, ColumnMinimumOrder), rec.value(2));
        setData(index(row, ColumnUnitPrice), rec.value(3));
        setData(index(row, ColumnPackagingId), rec.value(4));
    }
}

void PartDistributorTableModel::setSelectedPart(const QVariant & partId, bool refresh)
{
    _selectedPartId = partId;
    if(_selectedPartId.isValid()){
        qDebug("Selected part is %d",partId.toInt());
        if(refresh){
            QString where("part=%1");
            setFilter(where.arg(QString::number(partId.toInt())));
            if(!query().isActive())
                select();
        }
    }
    else{
        qDebug("Selected part is invalid");
        if(refresh){
            setFilter("part IS NULL");
            if(!query().isActive())
                select();
        }
    }
}

void PartDistributorTableModel::slotPrimeInsert(int, QSqlRecord &record)
{
    if(_selectedPartId.isValid()){
        qDebug("Using primeInsert to set distributor part ID to %d", _selectedPartId.toInt());
        record.setValue(ColumnPartId, _selectedPartId);
    }
    else{
        qDebug("part ID is null");
        record.setNull(ColumnPartId);
    }
    record.setGenerated(ColumnPartId, true);
}

void PartDistributorTableModel::slotBeforeInsert(QSqlRecord &record)
{
    if(record.isNull(ColumnPartId)){
        qDebug("Before insert with %d",_selectedPartId.toInt());
        record.setValue(ColumnPartId, _selectedPartId);
        record.setGenerated(ColumnPartId, true);
    }
}

