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
            deletable BOOLEAN NOT NULL\
            );";
    return runCreateScript(script);
}

bool DatabaseHelper::createPartUnitTable() const
{
    const char * script = "CREATE TABLE part_unit (\
            id INTEGER PRIMARY KEY AUTOINCREMENT,\
            name TEXT NOT NULL UNIQUE,\
            abbreviation TEXT NOT NULL UNIQUE,\
            defaultUnit BOOLEAN\
            );";
    return runCreateScript(script);
}


