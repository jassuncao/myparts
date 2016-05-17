#include "databasehelper.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

DatabaseHelper::DatabaseHelper(QSqlDatabase db) :
    _db(db)
{
}

bool DatabaseHelper::runCreateScript(const char * script) const
{
    const QString queryStr(script);
    qDebug()<<"Executing SQL script:\n"<<queryStr;
    QSqlQuery query(queryStr, _db);
    bool res = query.exec();
    if(!res){
        qWarning()<<"Failed to execute script. Reason:"<<query.lastError();
    }
    return res;
}

bool DatabaseHelper::createUnitTable() const
{
    const char * script = "CREATE TABLE unit (\
            id INTEGER PRIMARY KEY AUTOINCREMENT,\
            name TEXT NOT NULL UNIQUE,\
            symbol TEXT,\
            deletable BOOLEAN NOT NULL );";
    return runCreateScript(script);
}

bool DatabaseHelper::createPartUnitTable() const
{
    const char * script = "CREATE TABLE part_unit (\
            id INTEGER PRIMARY KEY AUTOINCREMENT,\
            name TEXT NOT NULL UNIQUE,\
            abbreviation TEXT NOT NULL UNIQUE,\
            defaultUnit BOOLEAN );";
    return runCreateScript(script);
}

bool DatabaseHelper::createCategoryTable() const
{
    const char * script = "CREATE TABLE category (\
            id INTEGER PRIMARY KEY AUTOINCREMENT,\
            name TEXT NOT NULL );";
    return runCreateScript(script);
}

bool DatabaseHelper::createConditionTable() const
{
    const char * script = "CREATE TABLE condition (\
            id INTEGER PRIMARY KEY AUTOINCREMENT,\
            value TEXT NOT NULL );";
    return runCreateScript(script);
}

bool DatabaseHelper::createDatatypeTable() const
{
    const char * script = "CREATE TABLE datatype  (\
            id INTEGER PRIMARY KEY AUTOINCREMENT,\
            name TEXT NOT NULL UNIQUE );";
    return runCreateScript(script);
}

bool DatabaseHelper::createPackageTable() const
{
    const char * script = "CREATE TABLE package (\
            id INTEGER PRIMARY KEY AUTOINCREMENT,\
            name TEXT NOT NULL );";
    return runCreateScript(script);
}

bool DatabaseHelper::createPackagingTable() const
{
    const char * script = "CREATE TABLE packaging (\
            id INTEGER PRIMARY KEY AUTOINCREMENT,\
            name TEXT NOT NULL );";
    return runCreateScript(script);
}

bool DatabaseHelper::createDistributorTable() const
{
    const char * script = "CREATE TABLE distributor (\
            id INTEGER PRIMARY KEY AUTOINCREMENT,\
            name TEXT NOT NULL );";
    return runCreateScript(script);
}

bool DatabaseHelper::createManufacturerTable() const
{
    const char * script = "CREATE TABLE manufacturer (\
            id INTEGER PRIMARY KEY AUTOINCREMENT,\
            name TEXT NOT NULL );";
    return runCreateScript(script);
}

bool DatabaseHelper::createPackageAttachmentTable() const
{
    const char * script = "CREATE TABLE package_attachment  (\
            id INTEGER PRIMARY KEY AUTOINCREMENT,\
            package INTEGER NOT NULL,\
            FOREIGN KEY(package) REFERENCES package(id) ON DELETE CASCADE);";
    return runCreateScript(script);
}

bool DatabaseHelper::createParameterTable() const
{
    const char * script = "CREATE TABLE parameter (\
            id INTEGER PRIMARY KEY AUTOINCREMENT,\
            key TEXT NOT NULL UNIQUE);";
    return runCreateScript(script);
}

bool DatabaseHelper::createPartDistributorTable() const
{
    const char * script = "CREATE TABLE part_distributor (\
            id INTEGER PRIMARY KEY AUTOINCREMENT,\
            part INTEGER NOT NULL,\
            FOREIGN KEY(part) REFERENCES part(id) ON DELETE CASCADE );";
    return runCreateScript(script);
}

bool DatabaseHelper::createPartManufacturerTable() const
{
    const char * script = "CREATE TABLE part_manufacturer (\
            id INTEGER PRIMARY KEY AUTOINCREMENT,\
            part INTEGER NOT NULL,\
            FOREIGN KEY(part) REFERENCES part(id) ON DELETE CASCADE );";
    return runCreateScript(script);
}

bool DatabaseHelper::createPartParameterTable() const
{
    const char * script = "CREATE TABLE part_parameter (\
            id INTEGER PRIMARY KEY AUTOINCREMENT,\
            part INTEGER NOT NULL,\
            FOREIGN KEY(part) REFERENCES part(id) ON DELETE CASCADE );";
    return runCreateScript(script);
}

bool DatabaseHelper::createPartTable() const
{
    const char * script = "CREATE TABLE part (\
            id INTEGER PRIMARY KEY AUTOINCREMENT,\
            name TEXT NOT NULL );";
    return runCreateScript(script);
}
