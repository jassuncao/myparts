#ifndef DATABASEHELPER_H
#define DATABASEHELPER_H

#include <QSqlDatabase>

class DatabaseHelper
{
public:
    DatabaseHelper(QSqlDatabase db = QSqlDatabase());
    bool createTables() const;
protected:
    bool runCreateScript(const QString& queryStr) const;
    bool createUnitTable() const;
    bool createCategoryTable() const;
    bool createStorageTable() const;
    bool createPartUnitTable() const;    
    bool createConditionTable() const;
    bool createDatatypeTable() const;
    bool createPackageTable() const;
    bool createPackageAttachmentTable() const;
    bool createPackagingTable() const;
    bool createParameterTable() const;
    bool createDistributorTable() const;
    bool createManufacturerTable() const;
    bool createPartTable() const;
    bool createPartDistributorTable() const;
    bool createPartManufacturerTable() const;
    bool createPartParameterTable() const;
    bool createPartAttachmentTable() const;
    bool createStockChangeTable() const;
    bool enableForeignKeys() const;
    bool tableExists(const char * tableName) const;
private:
    QSqlDatabase _db;
};

#endif // DATABASEHELPER_H
