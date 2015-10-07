#include "partmanufacturertablemodel.h"
#include <QSqlRecord>
#include <QDebug>
#include <QSqlField>
#include <QSqlError>
#include <QSqlQuery>

PartManufacturerTableModel::PartManufacturerTableModel(QObject *parent, QSqlDatabase db)
    : QSqlRelationalTableModel(parent, db)
{
    setEditStrategy(QSqlTableModel::OnManualSubmit);
    setTable("part_manufacturer");
    setRelation(ColumnManufacturerId,QSqlRelation("manufacturer","id","name"));
    setHeaderData(ColumnId, Qt::Horizontal, QObject::tr("ID"));
    setHeaderData(ColumnManufacturerId, Qt::Horizontal, QObject::tr("Manufacturer"));
    setHeaderData(ColumnPartNumber, Qt::Horizontal, QObject::tr("Part Number"));
    connect(this, SIGNAL(primeInsert(int,QSqlRecord&)), this, SLOT(slotPrimeInsert(int,QSqlRecord&)));
    connect(this, SIGNAL(beforeInsert(QSqlRecord&)), this, SLOT(slotBeforeInsert(QSqlRecord&)));
}

PartManufacturerTableModel::~PartManufacturerTableModel()
{
}

void PartManufacturerTableModel::copyTo(QVector<QSqlRecord> & dest)
{
    QSqlQuery q;
    q.prepare("SELECT manufacturer, partNumber FROM part_manufacturer WHERE part=:partId");
    q.bindValue(0, _selectedPartId);
    if(q.exec()){
        while(q.next()){
            dest.append(q.record());
        }
    }
    else{
        qWarning("Failed to execute PartManufacturerTableModel::copyTo query: %s", qPrintable(q.lastError().text()));
    }
}

void PartManufacturerTableModel::copyFrom(const QVector<QSqlRecord> & source)
{
    insertRows(0, source.size());
    for(int row = 0; row<source.size();++row){
        const QSqlRecord & rec = source.at(row);
        setData(index(row, ColumnManufacturerId), rec.value(0));
        setData(index(row, ColumnPartNumber), rec.value(1));
    }
}

void PartManufacturerTableModel::setSelectedPart(const QVariant & partId, bool refresh)
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

void PartManufacturerTableModel::slotPrimeInsert(int, QSqlRecord &record)
{
    if(_selectedPartId.isValid()){
        qDebug("Using primeInsert to set manufacturer part ID to %d", _selectedPartId.toInt());
        record.setValue(ColumnPartId, _selectedPartId);
    }
    else{
        qDebug("part ID is null");
        record.setNull(ColumnPartId);
    }
    record.setGenerated(ColumnPartId, true);
}

void PartManufacturerTableModel::slotBeforeInsert(QSqlRecord &record)
{
    if(record.isNull(ColumnPartId)){
        qDebug("Before insert with %d",_selectedPartId.toInt());
        record.setValue(ColumnPartId, _selectedPartId);
        record.setGenerated(ColumnPartId, true);
    }
}

