#include "databasehelper.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

DatabaseHelper::DatabaseHelper(QSqlDatabase db) :
    _db(db)
{
}

bool DatabaseHelper::runCreateScript(const QString& queryStr) const
{    
    qDebug()<<"Executing SQL script:\n"<<queryStr;
    QSqlQuery query(queryStr, _db);
    bool res = query.exec();
    if(!res){
        qWarning()<<"Failed to execute script. Reason:"<<query.lastError();
    }
    return res;
}

bool DatabaseHelper::createTables() const
{
    createUnitTable();
    createCategoryTable();
    createStorageTable();
    createPartUnitTable();
    createConditionTable();
    createDatatypeTable();
    createPackageTable();
    createPackageAttachmentTable();
    createPackagingTable();
    createParameterTable();
    createDistributorTable();
    createManufacturerTable();
    createPartTable();
    createPartDistributorTable();
    createPartManufacturerTable();
    createPartParameterTable();
    createStockChangeTable();
    enableForeignKeys();
    return false;
}

bool DatabaseHelper::createUnitTable() const
{
    const char * script = "CREATE TABLE IF NOT EXISTS unit (\
            id INTEGER PRIMARY KEY AUTOINCREMENT,\
            name TEXT NOT NULL UNIQUE,\
            symbol TEXT,\
            deletable BOOLEAN NOT NULL );";
    return runCreateScript(script);
}

bool DatabaseHelper::createCategoryTable() const
{
    const char * script1 = "CREATE TABLE IF NOT EXISTS category (\
            id INTEGER PRIMARY KEY AUTOINCREMENT,\
            name TEXT NOT NULL,\
            description TEXT ,\
            lft INTEGER NOT NULL,\
            rgt INTEGER NOT NULL\
            );";

    const char * script2 = "CREATE INDEX IF NOT EXISTS cat_lft_idx on category (lft);";
    const char * script3 = "CREATE INDEX IF NOT EXISTS cat_rgt_idx on category (rgt);";
    bool res = runCreateScript(script1);
    res = res && runCreateScript(script2);
    res = res && runCreateScript(script3);
    return res;
}

bool DatabaseHelper::createStorageTable() const
{
    const char * script1 = "CREATE TABLE IF NOT EXISTS storage (\
            id INTEGER PRIMARY KEY AUTOINCREMENT,\
            name TEXT NOT NULL,\
            description TEXT ,\
            lft INTEGER NOT NULL,\
            rgt INTEGER NOT NULL\
            );";

    const char * script2 = "CREATE INDEX IF NOT EXISTS storage_lft_idx on storage (lft);";
    const char * script3 = "CREATE INDEX IF NOT EXISTS storage_rgt_idx on storage (rgt);";
    bool res = runCreateScript(script1);
    res = res && runCreateScript(script2);
    res = res && runCreateScript(script3);
    return res;
}

bool DatabaseHelper::createPartUnitTable() const
{
    const char * script = "CREATE TABLE IF NOT EXISTS part_unit (\
            id INTEGER PRIMARY KEY AUTOINCREMENT,\
            name TEXT NOT NULL UNIQUE,\
            abbreviation TEXT NOT NULL UNIQUE,\
            defaultUnit BOOLEAN );";
    return runCreateScript(script);
}

bool DatabaseHelper::createConditionTable() const
{
    const char * script = "CREATE TABLE IF NOT EXISTS condition (\
            id INTEGER PRIMARY KEY AUTOINCREMENT,\
            value TEXT NOT NULL,\
            description TEXT ,\
            defaultCondition BOOLEAN );";
    return runCreateScript(script);
}

bool DatabaseHelper::createDatatypeTable() const
{
    const char * script = "CREATE TABLE IF NOT EXISTS datatype  (\
            id INTEGER PRIMARY KEY AUTOINCREMENT,\
            name TEXT NOT NULL UNIQUE );";
    return runCreateScript(script);
}

bool DatabaseHelper::createPackageTable() const
{
    const char * script = "CREATE TABLE IF NOT EXISTS package  (\
            id INTEGER PRIMARY KEY AUTOINCREMENT,\
            name TEXT NOT NULL,\
            description TEXT ,\
            imageFilename TEXT );";
    return runCreateScript(script);
}

bool DatabaseHelper::createPackagingTable() const
{
    const char * script = "CREATE TABLE IF NOT EXISTS packaging (\
            id INTEGER PRIMARY KEY AUTOINCREMENT,\
            name TEXT NOT NULL );";
    return runCreateScript(script);
}

bool DatabaseHelper::createDistributorTable() const
{
    const char * script = "CREATE TABLE IF NOT EXISTS distributor (\
            id INTEGER PRIMARY KEY AUTOINCREMENT, \
            name TEXT NOT NULL, \
            website TEXT, \
            comment TEXT );";
    return runCreateScript(script);
}

bool DatabaseHelper::createManufacturerTable() const
{
    const char * script = "CREATE TABLE IF NOT EXISTS manufacturer (\
            id INTEGER PRIMARY KEY AUTOINCREMENT, \
            name TEXT NOT NULL, \
            website TEXT, \
            comment TEXT );";
    return runCreateScript(script);
}

bool DatabaseHelper::createPackageAttachmentTable() const
{    
    const char * script = "CREATE TABLE IF NOT EXISTS package_attachment  (\
            id INTEGER PRIMARY KEY AUTOINCREMENT,\
            package INTEGER NOT NULL,\
            url TEXT NOT NULL,\
            description TEXT ,\
            FOREIGN KEY(package) REFERENCES package(id) ON DELETE CASCADE );";
    return runCreateScript(script);
}

bool DatabaseHelper::createParameterTable() const
{
    const char * script = "CREATE TABLE IF NOT EXISTS parameter  (\
            id INTEGER PRIMARY KEY AUTOINCREMENT,\
            key TEXT NOT NULL UNIQUE,\
            name TEXT NOT NULL,\
            datatype INTEGER ,\
            deletable BOOLEAN NOT NULL,\
            unit INTEGER ,\
            description TEXT ,\
            FOREIGN KEY(datatype) REFERENCES datatype(id),\
            FOREIGN KEY(unit) REFERENCES unit(id) );";
    return runCreateScript(script);
}

bool DatabaseHelper::createPartDistributorTable() const
{
    const char * script = "CREATE TABLE IF NOT EXISTS part_distributor (\
            id INTEGER PRIMARY KEY AUTOINCREMENT,\
            part INTEGER NOT NULL,\
            distributor INTEGER NOT NULL,\
            partNumber TEXT ,\
            minimumOrder INTEGER ,\
            unitPrice DOUBLE ,\
            packaging INTEGER ,\
            FOREIGN KEY(part) REFERENCES part(id) ON DELETE CASCADE,\
            FOREIGN KEY(distributor) REFERENCES distributor(id) ON DELETE CASCADE,\
            FOREIGN KEY(packaging) REFERENCES packaging(id) ON DELETE CASCADE );";
    return runCreateScript(script);
}

bool DatabaseHelper::createPartManufacturerTable() const
{
    const char * script = "CREATE TABLE IF NOT EXISTS part_manufacturer (\
            id INTEGER PRIMARY KEY AUTOINCREMENT,\
            part INTEGER NOT NULL,\
            manufacturer INTEGER NOT NULL,\
            partNumber TEXT NOT NULL,\
            FOREIGN KEY(part) REFERENCES part(id) ON DELETE CASCADE,\
            FOREIGN KEY(manufacturer) REFERENCES manufacturer(id) ON DELETE CASCADE ); ";
    return runCreateScript(script);
}

bool DatabaseHelper::createPartParameterTable() const
{
    const char * script = "CREATE TABLE IF NOT EXISTS part_parameter (\
            id INTEGER PRIMARY KEY AUTOINCREMENT,\
            part INTEGER NOT NULL,\
            parameter INTEGER NOT NULL,\
            numericValue DOUBLE ,\
            textValue TEXT ,\
            FOREIGN KEY(part) REFERENCES part(id) ON DELETE CASCADE,\
            FOREIGN KEY(parameter) REFERENCES parameter(id) );";
    return runCreateScript(script);
}

bool DatabaseHelper::createPartTable() const
{
    const char * script = "CREATE TABLE IF NOT EXISTS part (\
            id INTEGER PRIMARY KEY AUTOINCREMENT,\
            name TEXT NOT NULL,\
            description TEXT ,\
            actualStock INTEGER ,\
            minimumStock INTEGER ,\
            averagePrice DOUBLE ,\
            comment TEXT ,\
            customPartNumber TEXT ,\
            createDate DATETIME NOT NULL,\
            partUnit INTEGER NOT NULL,\
            category INTEGER NOT NULL,\
            storage INTEGER NOT NULL,\
            condition INTEGER ,\
            package INTEGER ,\
            FOREIGN KEY(partUnit) REFERENCES part_unit(id),\
            FOREIGN KEY(category) REFERENCES category(id),\
            FOREIGN KEY(storage) REFERENCES storage(id),\
            FOREIGN KEY(condition) REFERENCES condition(id),\
            FOREIGN KEY(package) REFERENCES package(id) );";
    return runCreateScript(script);
}

bool DatabaseHelper::createStockChangeTable() const
{
    const char * script = "CREATE TABLE IF NOT EXISTS stock_change (\
            id INTEGER PRIMARY KEY AUTOINCREMENT,\
            change INTEGER NOT NULL,\
            price DOUBLE ,\
            dateTime DATETIME NOT NULL,\
            comment TEXT ,\
            part INTEGER NOT NULL,\
            FOREIGN KEY(part) REFERENCES part(id) ON DELETE CASCADE );";
    return runCreateScript(script);
}

bool DatabaseHelper::createPartAttachmentTable() const
{
    const char * script = "CREATE TABLE IF NOT EXISTS part_attachment (\
            id INTEGER PRIMARY KEY AUTOINCREMENT,\
            part INTEGER NOT NULL,\
            url TEXT NOT NULL,\
            description TEXT ,\
            FOREIGN KEY(part) REFERENCES part(id) ON DELETE CASCADE );";
    return runCreateScript(script);
}

bool DatabaseHelper::enableForeignKeys() const
{
    const char * script = "PRAGMA foreign_keys = ON;";
    return runCreateScript(script);
}

bool DatabaseHelper::tableExists(const char * tableName) const
{
    bool exists = false;
    QString queryStr = QString("SELECT name FROM sqlite_master WHERE type='table' and name ='%1'").arg(tableName);
    QSqlQuery query(queryStr, _db);
    if(query.exec()){
        exists = query.next();
    }
    else{
        qWarning()<<"Failed to execute script. Reason:"<<query.lastError();
    }
    return exists;
}

