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
        qWarning() << "cannot open resource file";
    return execSqlScript(&f);
}

bool DatabaseHelper::execSqlScript(QFile* file) const
{
    QTextStream in(file);
    QString line;
    QString sqlStatement = "";
    bool ok = true;
    while (!in.atEnd() && ok){
        line = in.readLine();
        if(line.startsWith('#') || line.isEmpty())
            continue;
        sqlStatement.append(line);
        if(sqlStatement.trimmed().endsWith(";")){
            ok = execSqlStatement(sqlStatement);
            sqlStatement.clear();
        }
    }
    return ok;
}

bool DatabaseHelper::execSqlStatement(const QString& queryStr) const
{
    qDebug()<<"Executing SQL script:\n"<<queryStr;
    QSqlQuery query(queryStr, _db);
    bool res = query.exec();
    if(!res){
        qWarning()<<"Failed to execute script. Reason:"<<query.lastError();
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

