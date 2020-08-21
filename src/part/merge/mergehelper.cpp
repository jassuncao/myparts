#include "mergehelper.h"
#include <QSqlError>
#include <QDebug>

#define CALL_MEMBER_FN(object,ptrToMember)  ((object).*(ptrToMember))

static void mergePartChilds(QSqlQuery &updateQuery, QSqlQuery &deleteQuery, PartDetails * partA, PartDetails *partB, MergeHelper::FieldMergeType mergeType);

QDebug operator<<(QDebug debug, const MergeHelper::FieldMergeType &mergeType)
{
    switch (mergeType) {
    case MergeHelper::None:
        debug << "None";
        break;
    case MergeHelper::UseA:
        debug << "UseA";
        break;
    case MergeHelper::UseB:
        debug << "UseB";
        break;
    case MergeHelper::Both:
        debug << "Both";
        break;
    default:
        debug << "Invalid";
        break;
    }
    return debug;
}

MergeHelper::MergeHelper(QSqlDatabase db) : _db(db), _initialized(false)
{
    _helperFns[PartName] = &MergeHelper::mergeName;
    _helperFns[PartDescription] = &MergeHelper::mergeDescription;
    _helperFns[PartMinimumStock] = &MergeHelper::mergeMinimumStock;
    _helperFns[PartComment] = &MergeHelper::mergeComment;
    _helperFns[PartCustomPartNumber] = &MergeHelper::mergeCustomPartNumber;
    _helperFns[PartUnit] = &MergeHelper::mergePartUnit;
    _helperFns[PartCategory] = &MergeHelper::mergePartCategory;
    _helperFns[PartPackage] = &MergeHelper::mergePartPackage;
    _helperFns[PartParameters] = &MergeHelper::mergePartParameters;
    _helperFns[PartDistributors] = &MergeHelper::mergePartDistributors;
    _helperFns[PartManufacturers] = &MergeHelper::mergePartManufacturers;
    _helperFns[PartAttachments] = &MergeHelper::mergePartAttachments;
}

QSharedPointer<PartDetails> MergeHelper::loadPart(const QVariant &partId)
{
    prepareQueries();

    _partDetailsQuery.bindValue(0, partId);
    if(!_partDetailsQuery.exec() || !_partDetailsQuery.next()){
        qWarning() << "Failed to execute query:" << _partDetailsQuery.lastError();
        return QSharedPointer<PartDetails>();
    }
    PartDetails * partDetails = new PartDetails;

    int idx = 0;
    partDetails->id = partId;
    partDetails->name = _partDetailsQuery.value(idx++);
    partDetails->description = _partDetailsQuery.value(idx++);
    partDetails->totalStock = _partDetailsQuery.value(idx++);
    partDetails->minimumStock = _partDetailsQuery.value(idx++);
    partDetails->averagePrice = _partDetailsQuery.value(idx++);
    partDetails->comment = _partDetailsQuery.value(idx++);
    partDetails->customPartNumber = _partDetailsQuery.value(idx++);
    partDetails->partUnit = _partDetailsQuery.value(idx++);
    partDetails->partUnitName = _partDetailsQuery.value(idx++);
    partDetails->category = _partDetailsQuery.value(idx++);
    partDetails->categoryName = _partDetailsQuery.value(idx++);
    partDetails->package = _partDetailsQuery.value(idx++);
    partDetails->packageName = _partDetailsQuery.value(idx++);
    partDetails->parametersCount = _partDetailsQuery.value(idx++).toInt();
    partDetails->distributorsCount = _partDetailsQuery.value(idx++).toInt();
    partDetails->manufacturersCount = _partDetailsQuery.value(idx++).toInt();
    partDetails->attachmentsCount = _partDetailsQuery.value(idx++).toInt();

    return QSharedPointer<PartDetails>(partDetails);

}

bool MergeHelper::mergeParts(const QVariant &partA, const QVariant &partB, const QList<QPair<MergeHelper::PartField, MergeHelper::FieldMergeType> > &strategy)
{
    prepareQueries();
    QSharedPointer<PartDetails> partAPtr = loadPart(partA);
    QSharedPointer<PartDetails> partBPtr = loadPart(partB); 
    bool success = false;

    PartDetails *const a = partAPtr.data();
    PartDetails *const b = partBPtr.data();

    //Apply the merge strategy for each field or part child.
    //Part A will keep the merge results.
    for(int i=0; i<strategy.size(); ++i){
        const QPair<MergeHelper::PartField, MergeHelper::FieldMergeType>  pair = strategy.at(i);
        CALL_MEMBER_FN(*this, _helperFns[pair.first]) (a, b, pair.second);
    }


    //Stock is always merged into Part A
    mergePartChilds(_updatePartStockQuery, _deletePartStockQuery, a, b, MergeHelper::Both);
    a->totalStock = a->totalStock.toDouble() + b->totalStock.toDouble();

    //Persist the changes made to Part A into the dabase
    _updatePartQuery.bindValue(0, a->name);
    _updatePartQuery.bindValue(1, a->description);
    _updatePartQuery.bindValue(2, a->totalStock);
    _updatePartQuery.bindValue(3, a->minimumStock);
    _updatePartQuery.bindValue(4, a->averagePrice);
    _updatePartQuery.bindValue(5, a->comment);
    _updatePartQuery.bindValue(6, a->customPartNumber);
    _updatePartQuery.bindValue(7, a->partUnit);
    _updatePartQuery.bindValue(8, a->category);
    _updatePartQuery.bindValue(9, a->package);
    _updatePartQuery.bindValue(10, a->id);
    success = _updatePartQuery.exec();
    if(_updatePartQuery.lastError().isValid()){
        qWarning()<<"Update part query failed: "<<_updatePartQuery.lastError().text();
    }

    //Delete Part B
     _deletePartQuery.bindValue(0, b->id);

     success = success && _deletePartQuery.exec();

     if(_deletePartQuery.lastError().isValid()){
         qWarning()<<"Delete part query failed: "<<_deletePartQuery.lastError().text();
     }

     return success;
}


void MergeHelper::prepareQueries()
{
    if(_initialized){
        return;
    }
    _partDetailsQuery.prepare("SELECT p.name, "
                              "p.description,  "
                              "p.totalStock,  "
                              "p.minimumStock, "
                              "p.averagePrice, "
                              "p.comment, "
                              "p.customPartNumber, "
                              "p.partUnit, "
                              "unit.name, "
                              "p.category, "
                              "cat.name, "
                              "p.package, "
                              "pkg.name, "
                              "(SELECT count(id) FROM part_parameter WHERE part=p.id) AS paramCount, "
                              "(SELECT count(id) FROM part_distributor WHERE part=p.id) AS distributorCount, "
                              "(SELECT count(id) FROM part_manufacturer WHERE part=p.id) AS manufacturerCount, "
                              "(SELECT count(id) FROM part_attachment WHERE part=p.id) AS attachmentCount "
                              "FROM part AS p "
                              "INNER JOIN part_unit AS unit ON p.partUnit = unit.id "
                              "INNER JOIN category AS cat ON p.category = cat.id "
                              "INNER JOIN package as pkg ON p.package = pkg.id "
                              "WHERE p.id = ?;");

    _updatePartQuery.prepare("UPDATE part SET name = ?, description = ?, totalStock = ?, "
                             "minimumStock = ?, averagePrice=?, comment=?, customPartNumber = ?,"
                             "partUnit = ?, category = ?, package = ? WHERE id=?");
    _updatePartStockQuery.prepare("UPDATE part_stock SET part=? WHERE part=?");
    _deletePartStockQuery.prepare("DELETE FROM part_stock WHERE part=?");

    _updatePartParamQuery.prepare("UPDATE part_parameter SET part=? WHERE part=?");
    _deletePartParamQuery.prepare("DELETE FROM part_parameter WHERE part=?");

    _updatePartDistributorQuery.prepare("UPDATE part_distributor SET part=? WHERE part=?");
    _deletePartDistributorQuery.prepare("DELETE FROM part_distributor WHERE part=?");

    _updatePartManufacturerQuery.prepare("UPDATE part_manufacturer SET part=? WHERE part=?");
    _deletePartManufacturerQuery.prepare("DELETE FROM part_manufacturer WHERE part=?");

    _updatePartAttachmentQuery.prepare("UPDATE part_attachment SET part=? WHERE part=?");
    _deletePartAttachmentQuery.prepare("DELETE FROM part_attachment  WHERE part=?");

    _deletePartQuery.prepare("DELETE FROM part WHERE id=?");
}

void MergeHelper::mergeName(PartDetails * partA, PartDetails *partB, MergeHelper::FieldMergeType mergeType)
{
    qDebug()<<"Merge name -> "<<mergeType;
    switch (mergeType) {
    case MergeHelper::UseB:
        partA->name = partB->name;
        break;
    default:
        break;
    }
}

void MergeHelper::mergeDescription(PartDetails *partA, PartDetails *partB, MergeHelper::FieldMergeType mergeType)
{
    qDebug()<<"Merge description -> "<<mergeType;
    switch (mergeType) {
    case MergeHelper::UseB:
        partA->description = partB->description;
        break;
    default:
        break;
    }
}

void MergeHelper::mergeMinimumStock(PartDetails *partA, PartDetails *partB, MergeHelper::FieldMergeType mergeType)
{
    qDebug()<<"Merge minimumStock -> "<<mergeType;
    switch (mergeType) {
    case MergeHelper::UseB:
        partA->minimumStock = partB->minimumStock;
        break;
    default:
        break;
    }
}

void MergeHelper::mergeComment(PartDetails *partA, PartDetails *partB, MergeHelper::FieldMergeType mergeType)
{
    qDebug()<<"Merge comment -> "<<mergeType;
    switch (mergeType) {
    case MergeHelper::UseB:
        partA->comment = partB->comment;
        break;
    default:
        break;
    }
}

void MergeHelper::mergeCustomPartNumber(PartDetails *partA, PartDetails *partB, MergeHelper::FieldMergeType mergeType)
{
    qDebug()<<"Merge customPartNumber -> "<<mergeType;
    switch (mergeType) {
    case MergeHelper::UseB:
        partA->customPartNumber = partB->customPartNumber;
        break;
    default:
        break;
    }
}

void MergeHelper::mergePartUnit(PartDetails *partA, PartDetails *partB, MergeHelper::FieldMergeType mergeType)
{
    qDebug()<<"Merge partUnit -> "<<mergeType;
    switch (mergeType) {
    case MergeHelper::UseB:
        partA->partUnit = partB->partUnit;
        break;
    default:
        break;
    }
}

void MergeHelper::mergePartCategory(PartDetails *partA, PartDetails *partB, MergeHelper::FieldMergeType mergeType)
{
    qDebug()<<"Merge category -> "<<mergeType;
    switch (mergeType) {
    case MergeHelper::UseB:
        partA->category = partB->category;
        break;
    default:
        break;
    }
}

void MergeHelper::mergePartPackage(PartDetails *partA, PartDetails *partB, MergeHelper::FieldMergeType mergeType)
{
    qDebug()<<"Merge package -> "<<mergeType;
    switch (mergeType) {
    case MergeHelper::UseB:
        partA->package = partB->package;
        break;
    default:
        break;
    }
}



void MergeHelper::mergePartParameters(PartDetails * partA, PartDetails *partB, MergeHelper::FieldMergeType mergeType)
{
    qDebug()<<"Merge PartParameters -> "<<mergeType;
    mergePartChilds(_updatePartParamQuery, _deletePartParamQuery, partA, partB, mergeType);
}

void MergeHelper::mergePartDistributors(PartDetails *partA, PartDetails *partB, MergeHelper::FieldMergeType mergeType)
{
    qDebug()<<"Merge PartDistributors -> "<<mergeType;
    mergePartChilds(_updatePartDistributorQuery, _deletePartDistributorQuery, partA, partB, mergeType);
}

void MergeHelper::mergePartManufacturers(PartDetails *partA, PartDetails *partB, MergeHelper::FieldMergeType mergeType)
{
    qDebug()<<"Merge PartManufacturers -> "<<mergeType;
    mergePartChilds(_updatePartManufacturerQuery, _deletePartManufacturerQuery, partA, partB, mergeType);
}

void MergeHelper::mergePartAttachments(PartDetails *partA, PartDetails *partB, MergeHelper::FieldMergeType mergeType)
{
    qDebug()<<"Merge PartAttachments -> "<<mergeType;
    mergePartChilds(_updatePartAttachmentQuery, _deletePartAttachmentQuery, partA, partB, mergeType);
}

static void deletePartChilds(QSqlQuery &deleteQuery, const QVariant &id)
{
    qDebug()<<"deleteQuery("<<deleteQuery.lastQuery()<<") id="<<id;
    deleteQuery.bindValue(0,id);
    deleteQuery.exec();
    if(deleteQuery.lastError().isValid()){
        qWarning()<<"deleteQuery failed: "<<deleteQuery.lastError().text();
    }
    else{
        qDebug()<<"Items deleted"<<deleteQuery.numRowsAffected();
    }
}

static void reparentPartChilds(QSqlQuery &updateQuery, const QVariant &from, const QVariant &to)
{
    qDebug()<<"updateQuery("<<updateQuery.lastQuery()<<") from="<<from<<" to="<<to;
    updateQuery.bindValue(0, to);
    updateQuery.bindValue(1, from);
    updateQuery.exec();
    if(updateQuery.lastError().isValid()){
        qWarning()<<"updateQuery failed: "<<updateQuery.lastError().text();
    }
    else{
        qDebug()<<"Items updated"<<updateQuery.numRowsAffected();
    }
}

static void mergePartChilds(QSqlQuery &updateQuery, QSqlQuery &deleteQuery, PartDetails * partA, PartDetails *partB, MergeHelper::FieldMergeType mergeType)
{        
    switch (mergeType) {
    case MergeHelper::UseA:
        //Do nothing for B. Delete cascade takes care of it
        break;
    case MergeHelper::UseB:
        deletePartChilds(deleteQuery, partA->id);
        reparentPartChilds(updateQuery, partB->id, partA->id);
        break;
    case MergeHelper::Both:
        reparentPartChilds(updateQuery, partB->id, partA->id);
        break;
    case MergeHelper::None:
        deletePartChilds(deleteQuery, partA->id);
        //Do nothing for B. Delete cascade takes care of it
        break;
    default:
        break;
    }   
}
