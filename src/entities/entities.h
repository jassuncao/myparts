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

class CategoryEntity : public DQModel
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

class StorageEntity : public DQModel
{
    DQ_MODEL
public:
    DQField<QString> name;
    DQField<QString> path;
    DQField<int> lft;
    DQField<int> rgt;
    DQSharedList initialData() const;
};


class PackageEntity : public DQModel
{
    DQ_MODEL
public:
    DQField<QString> name;
    DQField<QString> description;
    DQField<QString> imageFilename;
    DQSharedList initialData() const;
};

class ConditionEntity : public DQModel
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
    DQForeignKey<CategoryEntity> category;
    DQForeignKey<StorageEntity> storage;
    DQForeignKey<PackageEntity> package;
    DQForeignKey<ConditionEntity> condition;

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

class SiPrefix : public DQModel
{
    DQ_MODEL
public:
    DQField<QString> prefix;
    DQField<QString> symbol;
    DQField<int> power;
    DQSharedList initialData() const;
};


class Unit : public DQModel
{
    DQ_MODEL
public:
    DQField<QString> name;
    DQField<QString> symbol;
    DQSharedList initialData() const;
};

class UnitPrefix : public DQModel
{
    DQ_MODEL
public:
    DQForeignKey<Unit> unit;
    DQForeignKey<SiPrefix> siPrefix;
};

class PartParameterEntity : public DQModel
{
    DQ_MODEL
public:
    DQField<QString> name;
    DQField<QString> description;
    DQField<double> value;
    DQForeignKey<PartEntity> part;
    DQForeignKey<Unit> unit;
    DQSharedList initialData() const;
};

class DistributorEntity : public DQModel
{
    DQ_MODEL
public:
    DQField<QString> name;
    DQField<QString> website;
    DQField<QString> comment;
    DQSharedList initialData() const;
};

class PackagingEntity : public DQModel
{
    DQ_MODEL
public:
    DQField<QString> name;
    DQSharedList initialData() const;
};

class PartDistributorEntity : public DQModel
{
    DQ_MODEL
public:
    DQForeignKey<PartEntity> part;
    DQForeignKey<DistributorEntity> distributor;
    DQField<QString> partNumber;
    DQField<int> minimumOrder;
    DQField<double> unitPrice;
    DQForeignKey<PackagingEntity> packaging;
    DQSharedList initialData() const;
};

class ManufacturerEntity : public DQModel
{
    DQ_MODEL
public:
    DQField<QString> name;
    DQField<QString> website;
    DQField<QString> comment;
    DQSharedList initialData() const;
};

class PartManufacturerEntity : public DQModel
{
    DQ_MODEL
public:
    DQForeignKey<PartEntity> part;
    DQForeignKey<ManufacturerEntity> manufacturer;
    DQField<QString> partNumber;
    DQSharedList initialData() const;
};

class PartAttachmentEntity : public DQModel
{
    DQ_MODEL
public:
    DQForeignKey<PartEntity> part;
    DQField<QString> url;
    DQField<QString> description;
};

class PackageAttachmentEntity : public DQModel
{
    DQ_MODEL
public:
    DQForeignKey<PackageEntity> package;
    DQField<QString> url;
    DQField<QString> description;
};

}  //namespace

DQ_DECLARE_MODEL(Entities::PartUnitEntity,
                     "part_unit", // the table name.
                     DQ_FIELD(name, DQUnique | DQNotNull),
                     DQ_FIELD(abbreviation, DQUnique | DQNotNull),
                     DQ_FIELD(defaultUnit )
                     )

DQ_DECLARE_MODEL(Entities::CategoryEntity,
                 "category", // the table name.
                 DQ_FIELD(name, DQNotNull),
                 DQ_FIELD(description),
                 DQ_FIELD(categoryPath),
                 DQ_FIELD(lft, DQNotNull),
                 DQ_FIELD(rgt, DQNotNull)
                 )

DQ_DECLARE_MODEL(Entities::StorageEntity,
                 "storage", // the table name.
                 DQ_FIELD(name, DQNotNull),
                 DQ_FIELD(path),
                 DQ_FIELD(lft, DQNotNull),
                 DQ_FIELD(rgt, DQNotNull)
                 )

DQ_DECLARE_MODEL(Entities::PackageEntity,
                 "package", // the table name.
                 DQ_FIELD(name, DQNotNull),
                 DQ_FIELD(description),
                 DQ_FIELD(imageFilename)
                 )

DQ_DECLARE_MODEL(Entities::ConditionEntity,
                 "condition", // the table name.
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
                 DQ_FIELD(storage , DQNotNull),
                 DQ_FIELD(condition),
                 DQ_FIELD(package)
                 )

DQ_DECLARE_MODEL(Entities::StockChangeEntity,
                 "stock_change", // the table name.
                 DQ_FIELD(change, DQNotNull),
                 DQ_FIELD(price),
                 DQ_FIELD(dateTime, DQNotNull),
                 DQ_FIELD(comment),
                 DQ_FIELD(part, DQNotNull | DQCascadeDelete)
                 )

DQ_DECLARE_MODEL(Entities::SiPrefix,
                 "si_prefix", // the table name.
                 DQ_FIELD(prefix, DQNotNull),
                 DQ_FIELD(symbol, DQNotNull),
                 DQ_FIELD(power, DQNotNull)
                 )

DQ_DECLARE_MODEL(Entities::Unit,
                 "unit", // the table name.
                 DQ_FIELD(name, DQNotNull),
                 DQ_FIELD(symbol)
                 )

DQ_DECLARE_MODEL(Entities::UnitPrefix,
                 "unit_prefix", // the table name.
                 DQ_FIELD(unit, DQNotNull | DQCascadeDelete),
                 DQ_FIELD(siPrefix, DQNotNull | DQCascadeDelete)
                 )

DQ_DECLARE_MODEL(Entities::PartParameterEntity,
                 "part_parameter", // the table name.
                 DQ_FIELD(name, DQNotNull),
                 DQ_FIELD(value),
                 DQ_FIELD(unit, DQNotNull),
                 DQ_FIELD(part, DQNotNull | DQCascadeDelete),
                 DQ_FIELD(description)
                 )

DQ_DECLARE_MODEL(Entities::DistributorEntity,
                 "distributor", // the table name.
                 DQ_FIELD(name, DQNotNull),
                 DQ_FIELD(website),
                 DQ_FIELD(comment)
                 )

DQ_DECLARE_MODEL(Entities::PackagingEntity,
                 "packaging", // the table name.
                 DQ_FIELD(name, DQNotNull)
                 )

DQ_DECLARE_MODEL(Entities::PartDistributorEntity,
                 "part_distributor", // the table name.
                 DQ_FIELD(part, DQNotNull | DQCascadeDelete),
                 DQ_FIELD(distributor, DQNotNull | DQCascadeDelete),
                 DQ_FIELD(partNumber),
                 DQ_FIELD(minimumOrder),
                 DQ_FIELD(unitPrice),
                 DQ_FIELD(packaging, DQCascadeDelete)
                 )

DQ_DECLARE_MODEL(Entities::ManufacturerEntity,
                 "manufacturer", // the table name.
                 DQ_FIELD(name, DQNotNull),
                 DQ_FIELD(website),
                 DQ_FIELD(comment)
                 )

DQ_DECLARE_MODEL(Entities::PartManufacturerEntity,
                 "part_manufacturer", // the table name.
                 DQ_FIELD(part, DQNotNull | DQCascadeDelete),
                 DQ_FIELD(manufacturer, DQNotNull | DQCascadeDelete),
                 DQ_FIELD(partNumber, DQNotNull)
                 )

DQ_DECLARE_MODEL(Entities::PartAttachmentEntity,
                 "part_attachment", // the table name.
                 DQ_FIELD(part, DQNotNull | DQCascadeDelete),
                 DQ_FIELD(url, DQNotNull),
                 DQ_FIELD(description)
                 )

DQ_DECLARE_MODEL(Entities::PackageAttachmentEntity,
                 "package_attachment", // the table name.
                 DQ_FIELD(package, DQNotNull | DQCascadeDelete),
                 DQ_FIELD(url, DQNotNull),
                 DQ_FIELD(description)
                 )


#endif // PARTENTITY_H
