#include "partparametertablemodel.h"
#include <QSqlRecord>
#include <QDebug>
#include <QSqlField>
#include <QSqlError>
#include <QSqlQuery>


PartParameterTableModel::PartParameterTableModel(QObject *parent, QSqlDatabase db)
    : QSqlRelationalTableModel(parent, db)
{
    setEditStrategy(QSqlTableModel::OnManualSubmit);
    setTable("part_parameter");
    setRelation(ColumnUnit,QSqlRelation("unit","id","name"));
    setHeaderData(ColumnId, Qt::Horizontal, QObject::tr("ID"));
    setHeaderData(ColumnName, Qt::Horizontal, QObject::tr("Name"));
    setHeaderData(ColumnDescription, Qt::Horizontal, QObject::tr("Description"));
    setHeaderData(ColumnValue, Qt::Horizontal, QObject::tr("Value"));
    setHeaderData(ColumnUnit, Qt::Horizontal, QObject::tr("Unit"));
    setHeaderData(ColumnPartId, Qt::Horizontal, QObject::tr("PartId"));
    connect(this, SIGNAL(primeInsert(int,QSqlRecord&)), this, SLOT(slotPrimeInsert(int,QSqlRecord&)));
    connect(this, SIGNAL(beforeInsert(QSqlRecord&)), this, SLOT(slotBeforeInsert(QSqlRecord&)));
}

PartParameterTableModel::~PartParameterTableModel()
{
}

void PartParameterTableModel::copyTo(QVector<QSqlRecord> & dest)
{
    QSqlQuery q;
    q.prepare("SELECT name, value, unit, description FROM part_parameter WHERE part=:partId");
    q.bindValue(0, _selectedPartId);
    if(q.exec()){
        while(q.next()){
            dest.append(q.record());
        }
    }
    else{
        qWarning("Failed to execute PartParameterTableModel::copyTo query: %s", qPrintable(q.lastError().text()));
    }
}

void PartParameterTableModel::copyFrom(const QVector<QSqlRecord> & source)
{
    insertRows(0, source.size());
    for(int row = 0; row<source.size();++row){
        const QSqlRecord & rec = source.at(row);
        setData(index(row, ColumnName), rec.value(0));
        setData(index(row, ColumnValue), rec.value(1));
        setData(index(row, ColumnUnit), rec.value(2));
        setData(index(row, ColumnDescription), rec.value(3));
    }
}

void PartParameterTableModel::setSelectedPart(const QVariant & partId, bool refresh)
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

void PartParameterTableModel::slotPrimeInsert(int, QSqlRecord &record)
{
    if(_selectedPartId.isValid()){
        qDebug("Using primeInsert to set parameter part ID to %d", _selectedPartId.toInt());
        record.setValue(ColumnPartId, _selectedPartId);
    }
    else{
        qDebug("part ID is null");
        record.setNull(ColumnPartId);
    }
    record.setGenerated(ColumnPartId, true);
}

void PartParameterTableModel::slotBeforeInsert(QSqlRecord &record)
{
    if(record.isNull(ColumnPartId)){
        qDebug("Before insert with %d",_selectedPartId.toInt());
        record.setValue(ColumnPartId, _selectedPartId);
        record.setGenerated(ColumnPartId, true);
    }
}

//bool PartParameterTableModel::insertRowIntoTable(const QSqlRecord &values)
//{
//    qDebug()<<"Insert ";
//    int count = values.count();
//    for(int i=0; i<count; ++i){
//        qDebug()<<"Field"<<values.fieldName(i)<<"="<<values.field(i).value();
//    }
//    bool res = QSqlRelationalTableModel::insertRowIntoTable(values);
//    qDebug()<<"Insert res "<<res;
//    if(!res){
//        qDebug()<<"Last error "<<lastError();
//    }
//    return res;
//}







