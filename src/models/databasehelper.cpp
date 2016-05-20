#include "databasehelper.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QFile>
#include <QStringBuilder>

DatabaseHelper::DatabaseHelper(QSqlDatabase db) :
    _db(db)
{
}

bool DatabaseHelper::createDatabase(void) const
{
    QFile f(":/scripts/SQLiteInit.sql");
    if(!f.open(QIODevice::ReadOnly | QIODevice::Text))
        qWarning() << "cannot open database initialization script";
    return execSqlScript(&f);
}

bool DatabaseHelper::loadInitialData(void) const
{
    QFile f(":/scripts/initialdata.sql");
    if(!f.open(QIODevice::ReadOnly | QIODevice::Text))
        qWarning() << "cannot open initial data script";
    return execSqlScript(&f);
}

bool DatabaseHelper::execSqlScript(QFile* file) const
{
    QTextStream in(file);
    QString line;
    QString sqlStatement = "";
    bool ok = true;
    int lineNumber = 0;
    while (!in.atEnd() && ok){
        line = in.readLine();
        lineNumber++;
        if(line.startsWith('#') || line.isEmpty())
            continue;
        sqlStatement.append(line.trimmed());
        if(sqlStatement.endsWith(";")){
            ok = execSqlStatement(sqlStatement, lineNumber);
            sqlStatement.clear();
        }
    }
    return ok;
}

bool DatabaseHelper::execSqlStatement(const QString& queryStr, const int lineNumber) const
{
    qDebug()<<"Executing SQL script:\n"<<queryStr;
    QSqlQuery query(_db);
    bool res = query.exec(queryStr);
    if(!res){
        qWarning()<<"Failed to execute script at line "<< lineNumber<<". Reason:"<<query.lastError();
    }
    return res;
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

