#include "footprintattachmenttablemodel.h"
#include <QSqlQuery>
#include <QDebug>

FootprintAttachmentTableModel::FootprintAttachmentTableModel(QObject *parent, QSqlDatabase db) :
    QSqlTableModel(parent, db)
{
    setTable("footprint_attachment");
    setHeaderData(FootprintAttachmentTableModel::ColumnId, Qt::Horizontal, QObject::tr("ID"));
    setHeaderData(FootprintAttachmentTableModel::ColumnFootprintId, Qt::Horizontal, QObject::tr("Footprint ID"));
    setHeaderData(FootprintAttachmentTableModel::ColumnURL, Qt::Horizontal, QObject::tr("Resource"));
    setHeaderData(FootprintAttachmentTableModel::ColumnDescription, Qt::Horizontal, QObject::tr("Description"));
    connect(this, SIGNAL(primeInsert(int,QSqlRecord&)), this, SLOT(slotPrimeInsert(int,QSqlRecord&)));
    connect(this, SIGNAL(beforeInsert(QSqlRecord&)), this, SLOT(slotBeforeInsert(QSqlRecord&)));
}

FootprintAttachmentTableModel::~FootprintAttachmentTableModel()
{
}

/*
QVariant FootprintAttachmentTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    qDebug()<<"Header data for section "<<section;
    return QSqlTableModel::headerData(section, orientation, role);
}
*/

void FootprintAttachmentTableModel::setSelectedFootprint(const QVariant & footprintId, bool refresh)
{
    _selectedFootprintId = footprintId;
    if(_selectedFootprintId.isValid()){
        qDebug("Selected footprint is %d",_selectedFootprintId.toInt());
        if(refresh){
            QString where("footprint=%1");
            setFilter(where.arg(QString::number(_selectedFootprintId.toInt())));
            if(!query().isActive())
                select();
        }
    }
    else{
        qDebug("Selected footprint is invalid");
        if(refresh){
            setFilter("footprint IS NULL");
            if(!query().isActive())
                select();
        }
    }
}

void FootprintAttachmentTableModel::slotPrimeInsert(int, QSqlRecord &record)
{
    if(_selectedFootprintId.isValid()){
        qDebug("Using primeInsert to set attachment footprint ID to %d", _selectedFootprintId.toInt());
        record.setValue(ColumnFootprintId, _selectedFootprintId);
    }
    else{
        qDebug("footprint ID is null");
        record.setNull(ColumnFootprintId);
    }
    record.setGenerated(ColumnFootprintId, true);
}

void FootprintAttachmentTableModel::slotBeforeInsert(QSqlRecord &record)
{
    if(record.isNull(ColumnFootprintId)){
        qDebug("Before insert with %d", _selectedFootprintId.toInt());
        record.setValue(ColumnFootprintId, _selectedFootprintId);
        record.setGenerated(ColumnFootprintId, true);
    }
}
