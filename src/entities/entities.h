#ifndef PARTENTITY_H
#define PARTENTITY_H

#include <dquest.h>

namespace Entities {

class PartUnitEntity : public DQModel
{
    DQ_MODEL
public:
    DQField<QString> name;
    DQField<QString> abbreviation;
    DQField<bool> defaultUnit;
    DQSharedList initialData() const;
};

class PartCategoryEntity : public DQModel
{
    DQ_MODEL
public:
    DQField<QString> name;
    DQField<QString> description;
    DQField<QString> categoryPath;
    DQField<int> lft;
    DQField<int> rgt;
    DQSharedList initialData() const;
};

class PartStorageEntity : public DQModel
{
    DQ_MODEL
public:
    DQField<QString> name;
    DQSharedList initialData() const;
};


class PartFootprintEntity : public DQModel
{
    DQ_MODEL
public:
    DQField<QString> name;
    DQField<QString> description;
    DQForeignKey<PartFootprintEntity> parent;
};

class PartConditionEntity : public DQModel
{
    DQ_MODEL
public:
    DQField<QString> value;
    DQField<QString> description;
    DQField<bool> defaultCondition;
    DQSharedList initialData() const;
};

class PartEntity : public DQModel
{
    DQ_MODEL
public:
    DQField<QString> name;
    DQField<QString> description;
    DQField<int> minimumStock;
    DQField<int> actualStock;
    DQField<double> averagePrice;
    DQField<QString> comment;
    DQField<QString> customPartNumber;
    DQField<QDateTime> createDate;    
    DQForeignKey<PartUnitEntity> partUnit;
    DQForeignKey<PartCategoryEntity> category;
    DQForeignKey<PartStorageEntity> storage;
    DQForeignKey<PartFootprintEntity> footprint;
    DQForeignKey<PartConditionEntity> condition;


    //Parameters
    //StockHistory
    //Manufacturers
    //Distributors
    //Images
    //Attachments
    DQSharedList initialData() const;
};

class StockChangeEntity : public DQModel
{
    DQ_MODEL
public:
    DQField<int> change;
    DQField<double> price;
    DQField<QDateTime> dateTime;
    DQField<QString> comment;
    DQForeignKey<PartEntity> part;
};

}  //namespace

DQ_DECLARE_MODEL(Entities::PartUnitEntity,
                     "part_unit", // the table name.
                     DQ_FIELD(name, DQUnique | DQNotNull),
                     DQ_FIELD(abbreviation, DQUnique | DQNotNull),
                     DQ_FIELD(defaultUnit )
                     )

DQ_DECLARE_MODEL(Entities::PartCategoryEntity,
                 "part_category", // the table name.
                 DQ_FIELD(name, DQNotNull),
                 DQ_FIELD(description),
                 DQ_FIELD(categoryPath),
                 DQ_FIELD(lft, DQNotNull),
                 DQ_FIELD(rgt, DQNotNull)
                 )

DQ_DECLARE_MODEL(Entities::PartStorageEntity,
                 "part_storage", // the table name.
                 DQ_FIELD(name, DQNotNull)
                 )

DQ_DECLARE_MODEL(Entities::PartFootprintEntity,
                 "part_footprint", // the table name.
                 DQ_FIELD(name, DQNotNull),
                 DQ_FIELD(description),
                 DQ_FIELD(parent)
                 )

DQ_DECLARE_MODEL(Entities::PartConditionEntity,
                 "part_condition", // the table name.
                 DQ_FIELD(value, DQNotNull),
                 DQ_FIELD(description),
                 DQ_FIELD(defaultCondition)
                 )

DQ_DECLARE_MODEL(Entities::PartEntity,
                 "part", // the table name.
                 DQ_FIELD(name, DQNotNull),
                 DQ_FIELD(description),
                 DQ_FIELD(actualStock),
                 DQ_FIELD(minimumStock),                 
                 DQ_FIELD(averagePrice),
                 DQ_FIELD(comment),
                 DQ_FIELD(customPartNumber),
                 DQ_FIELD(createDate, DQNotNull),
                 DQ_FIELD(partUnit , DQNotNull),
                 DQ_FIELD(category , DQNotNull),
                 DQ_FIELD(storage),
                 DQ_FIELD(condition)
                 )

DQ_DECLARE_MODEL(Entities::StockChangeEntity,
                 "stock_change", // the table name.
                 DQ_FIELD(change, DQNotNull),
                 DQ_FIELD(price),
                 DQ_FIELD(dateTime, DQNotNull),
                 DQ_FIELD(comment),
                 DQ_FIELD(part, DQNotNull | DQCascadeDelete)
                 )


#endif // PARTENTITY_H
