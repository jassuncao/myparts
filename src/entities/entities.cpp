#include "entities.h"
#include <dquest.h>

namespace Entities {

DQSharedList PartUnitEntity::initialData() const
{
    DQList<Entities::PartUnitEntity> initialUnits;
    DQListWriter writer(&initialUnits);
    writer << "Pieces" << "pcs" << true
           << "Centimeter" << "cm" << false
           << "Meters" << "m" << false;
    return initialUnits;
}

DQSharedList PartCategoryEntity::initialData() const
{
    DQList<Entities::PartCategoryEntity> initialCategories;
    DQListWriter writer(&initialCategories);
    writer << "Root Category" << "" << "" << 1 << 20
           << "Active Components" << "Active Components" << "" << 2 << 17
             << "Semiconductors" << "Semiconductors" << "" << 3 << 12
              << "Transistors" << "Transistors" << ""  << 4 << 9
                 << "NPN" << "" << ""  << 5 << 6
                 << "PNP" << "" << ""  << 7 << 8
              << "Integrated Circuits" << "" << ""  << 10 << 11
             << "Optoelectronics" << "" << ""  <<13 << 14
             << "Others" << "" << ""  <<15 << 16
           << "Passive Components" << "Passive Components" << ""  << 18 << 19
              ;
    return initialCategories;
}

/*
DQSharedList PartCategoryEntity::initialData() const
{          
    DQList<Entities::PartCategoryEntity> initialCategories;
    DQListWriter writer(&initialCategories);
    writer << "Active Components" << "Active Components" << "" << 1 << 16
             << "Semiconductors" << "Semiconductors" << "" << 2 << 11
              << "Transistors" << "Transistors" << ""  << 3 << 8
                 << "NPN" << "" << ""  << 4 << 5
                 << "PNP" << "" << ""  << 6 << 7
              << "Integrated Circuits" << "" << ""  << 9 << 10
             << "Optoelectronics" << "" << ""  <<12 << 13
             << "Others" << "" << ""  <<14 << 15
           << "Passive Components" << "Passive Components" << ""  << 17 << 18
              ;
    return initialCategories;
}
*/

DQSharedList PartEntity::initialData() const
{
/*
 *
 *   DQ_FIELD(name, DQNotNull),
     DQ_FIELD(description),
     DQ_FIELD(minimumStock),
     DQ_FIELD(actualStock),
     DQ_FIELD(comment),
     DQ_FIELD(customPartNumber),
     DQ_FIELD(createDate, DQNotNull),
     DQ_FIELD(partUnit , DQNotNull),
     DQ_FIELD(category , DQNotNull),
     DQ_FIELD(storage),
     DQ_FIELD(condition)
    */

    QDateTime now = QDateTime::currentDateTimeUtc();
    //QVariant now = QVariant(QDateTime::currentDateTimeUtc().toTime_t());
    DQList<Entities::PartEntity> initialParts;
    DQListWriter writer(&initialParts);
    writer << "BC547A" << "Transistor NPN TO-92 45V 0,1A 0,5W" << 30  << 0 << 0.5 << "Comment 1" << "PART01" << now.toTime_t() << 1 << 4 << writer.next()
           << "BC547B" << "Transistor NPN TO-92 45V 0,1A 0,5W" << 30 << 10 << 0.8 << "Comment 2" << "PART02" << now.addDays(-1).toTime_t() << 1 << 4 << writer.next()
           << "BC548" << "Transistor PNP TO-92 45V 0,1A 0,5W"  << 50 << 20 << 1.2 << "Comment 3" << "PART03" << now.addDays(-2).toTime_t() << 1 << 5 << writer.next();
    return initialParts;
}

DQSharedList PartStorageEntity::initialData() const
{
    DQList<Entities::PartStorageEntity> initialLocations;
    DQListWriter writer(&initialLocations);
    writer << "Cabinet 1" << "Cabinet 2" << "Cabinet 3" << "Cabinet 4";
    return initialLocations;
}

DQSharedList PartConditionEntity::initialData() const
{
    DQList<Entities::PartConditionEntity> initialCondition;
    DQListWriter writer(&initialCondition);
    writer << "New" << "Brand New" << true
           << "Used" << "Used" << false
           << "Damaged" << "Damaged" << false;
    return initialCondition;
}

}
