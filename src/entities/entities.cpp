#include "entities.h"
#include <dquest.h>

namespace Entities {

DQSharedList PartUnitEntity::initialData() const
{
    DQList<Entities::PartUnitEntity> initialUnits;
    DQListWriter writer(&initialUnits);
    writer << "Pieces" << "pcs" << 1
           << "Centimeter" << "cm" << 0
           << "Meters" << "m" << 0;
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
    writer << "Storage Root" << "" << 1 << 44
            << "Cabinet 1" << "" << 2 << 27
                << "Drawer 1" << "" << 3 << 14
                    << "Compartment 1" << "" << 4 << 5
                    << "Compartment 2" << "" << 6 << 7
                    << "Compartment 3" << "" << 8 << 9
                    << "Compartment 4" << "" << 10 << 11
                    << "Compartment 5" << "" << 12 << 13
                << "Drawer 2" << "" << 15 << 26
                    << "Compartment 1" << "" << 16 << 17
                    << "Compartment 2" << "" << 18 << 19
                    << "Compartment 3" << "" << 20 << 21
                    << "Compartment 4" << "" << 22 << 23
                    << "Compartment 5" << "" << 24 << 25
            << "Cabinet 2" << "" << 28 << 43
                << "Drawer 1" << "" << 29 << 40
                    << "Compartment 1" << "" << 30 << 31
                    << "Compartment 2" << "" << 32 << 33
                    << "Compartment 3" << "" << 34 << 35
                    << "Compartment 4" << "" << 36 << 37
                    << "Compartment 5" << "" << 38 << 39
                << "Drawer 2" << "" << 41 << 42;
    return initialLocations;
}

DQSharedList PartConditionEntity::initialData() const
{
    DQList<Entities::PartConditionEntity> initialCondition;
    DQListWriter writer(&initialCondition);
    writer << "New" << "Brand New" << 1
           << "Used" << "Used" << 0
           << "Damaged" << "Damaged" << 0;
    return initialCondition;
}

DQSharedList FootprintEntity::initialData() const
{
    DQList<Entities::FootprintEntity> initialCondition;
    DQListWriter writer(&initialCondition);
    writer << "0805" <<  "0805 (2.0 x 1.2mm)" << "0805_2015-03-01.png"
           << "0603" <<  "0603 (1.6 x 0.8mm)" << "0603_2015-03-01.png"
           << "0402" <<  "0402 (1.0 x 0.5mm)" << "0402_2015-03-01.png";
    return initialCondition;
}

DQSharedList SiPrefix::initialData() const
{
    DQList<Entities::SiPrefix> initialData;
    DQListWriter writer(&initialData);
    writer << "yotta" <<  "Y" << 24
           << "zetta" <<  "Z" << 21
           << "exa" <<  "E" << 18
           << "peta" <<  "P" << 15
           << "tera" <<  "T" << 12
           << "giga" <<  "G" << 9
           << "mega" <<  "M" << 6
           << "kilo" <<  "k" << 3
           //<< "hecto" <<  "h" << 2
           //<< "deca" <<  "da" << 1
           << "-" <<  "" << 0
           //<< "deci" <<  "d" << -1
           //<< "centi" <<  "c" << -2
           << "milli" <<  "m" << -3
           << "micro" <<  "u" << -6
           << "nano" <<  "n" << -9
           << "pico" <<  "p" << -12
           << "femto" <<  "f" << -15
           << "atto" <<  "a" << -18
           << "zepto" <<  "z" << -21
           << "yocto" <<  "y" << -24;

    return initialData;
}


DQSharedList Unit::initialData() const
{
    DQList<Entities::Unit> initialData;
    DQListWriter writer(&initialData);
    writer << "Volt" <<  "V"
           << "Ampere" << "A"
           << "Ohm" << "R";

    return initialData;
}


DQSharedList PartParameterEntity::initialData() const
{
    DQList<Entities::PartParameterEntity> initialData;
    DQListWriter writer(&initialData);
    writer << "resistance" << 1000 << 3 << 1 <<  "Resistance " ;

    return initialData;
}

DQSharedList DistributorEntity::initialData() const
{
    DQList<Entities::DistributorEntity> initialData;
    DQListWriter writer(&initialData);
    writer << "Farnell" << "www.farnell.com" << "A comment"
              << "Digikey" << "www.digikey.com" << "My Favorite";
    for(int i=0; i<500;++i){
        writer<<(QString("Distributor %1").arg(i))<<"www.distributor.com"<<"Comment";
    }
    return initialData;
}

DQSharedList PackagingEntity::initialData() const
{
    DQList<Entities::PackagingEntity> initialData;
    DQListWriter writer(&initialData);
    writer << "Single" << "Tape & Reel" << "Cut Tape"
              << "Tube" << "Digi-Reel®";    
    return initialData;
}

DQSharedList PartDistributorEntity::initialData() const
{
    DQList<Entities::PartDistributorEntity> initialData;
    DQListWriter writer(&initialData);
    writer << 1 << 1 << "R100"<< 1 << 0.02 << 1;
    return initialData;
}

DQSharedList ManufacturerEntity::initialData() const
{
    DQList<Entities::ManufacturerEntity> initialData;
    DQListWriter writer(&initialData);
    writer << "Texas Instruments" << "www.ti.com" << "A comment"
              << "Linear Technology" << "www.linear.com" << "Another comment";
    return initialData;
}

DQSharedList PartManufacturerEntity::initialData() const
{
    DQList<Entities::PartManufacturerEntity> initialData;
    DQListWriter writer(&initialData);
    writer << 1 << 1 << "R100";
    return initialData;
}

}
