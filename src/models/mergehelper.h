#ifndef MERGEHELPER_H
#define MERGEHELPER_H

#include <QVariant>
#include <QSqlQuery>
#include <QSqlDatabase>
#include <QSharedPointer>
#include <QList>
#include <QPair>

struct PartDetails {
    QVariant id;
    QVariant name;
    QVariant description;
    QVariant totalStock;
    QVariant minimumStock;
    QVariant averagePrice;
    QVariant comment;
    QVariant customPartNumber;
    QVariant partUnit;
    QVariant partUnitName;
    QVariant category;
    QVariant categoryName;
    QVariant package;
    QVariant packageName;
    int parametersCount;
    int distributorsCount;
    int manufacturersCount;
    int attachmentsCount;
};

class MergeHelper
{
public:    

    enum PartField{
        PartName,
        PartDescription,
        PartMinimumStock,
        PartComment,
        PartCustomPartNumber,
        PartUnit,
        PartCategory,
        PartPackage,
        PartParameters,
        PartDistributors,
        PartManufacturers,
        PartAttachments,
        N_FIELDS
    };

    enum FieldMergeType {
        None = 0,
        UseA = 1,
        UseB = 2,
        Both = 3
    };    

    MergeHelper(QSqlDatabase db = QSqlDatabase());
    QSharedPointer<PartDetails> loadPart(const QVariant &partId);    
    bool mergeParts(const QVariant & partA, const QVariant & partB, const QList<QPair<PartField,FieldMergeType> > & strategy);
private:
    typedef void (MergeHelper::*MergeHelperFn)(PartDetails * partA, PartDetails *partB, MergeHelper::FieldMergeType mergeType);
    void mergeName(PartDetails * partA, PartDetails *partB, MergeHelper::FieldMergeType mergeType);
    void mergeDescription(PartDetails * partA, PartDetails *partB, MergeHelper::FieldMergeType mergeType);
    void mergeMinimumStock(PartDetails * partA, PartDetails *partB, MergeHelper::FieldMergeType mergeType);
    void mergeComment(PartDetails * partA, PartDetails *partB, MergeHelper::FieldMergeType mergeType);
    void mergeCustomPartNumber(PartDetails * partA, PartDetails *partB, MergeHelper::FieldMergeType mergeType);
    void mergePartUnit(PartDetails * partA, PartDetails *partB, MergeHelper::FieldMergeType mergeType);
    void mergePartCategory(PartDetails * partA, PartDetails *partB, MergeHelper::FieldMergeType mergeType);
    void mergePartPackage(PartDetails * partA, PartDetails *partB, MergeHelper::FieldMergeType mergeType);
    void mergePartParameters(PartDetails * partA, PartDetails *partB, MergeHelper::FieldMergeType mergeType);
    void mergePartDistributors(PartDetails * partA, PartDetails *partB, MergeHelper::FieldMergeType mergeType);
    void mergePartManufacturers(PartDetails * partA, PartDetails *partB, MergeHelper::FieldMergeType mergeType);
    void mergePartAttachments(PartDetails * partA, PartDetails *partB, MergeHelper::FieldMergeType mergeType);

     void prepareQueries();     
     QSqlDatabase _db;
     bool _initialized;
     QSqlQuery _partDetailsQuery;
     QSqlQuery _updatePartQuery;
     QSqlQuery _deletePartQuery;
     QSqlQuery _updatePartStockQuery;
     QSqlQuery _deletePartStockQuery;
     QSqlQuery _updatePartParamQuery;
     QSqlQuery _deletePartParamQuery;
     QSqlQuery _updatePartDistributorQuery;
     QSqlQuery _deletePartDistributorQuery;
     QSqlQuery _updatePartManufacturerQuery;
     QSqlQuery _deletePartManufacturerQuery;
     QSqlQuery _updatePartAttachmentQuery;
     QSqlQuery _deletePartAttachmentQuery;
     MergeHelperFn _helperFns[N_FIELDS];
};

#endif // MERGEHELPER_H
