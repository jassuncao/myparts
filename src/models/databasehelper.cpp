#include "databasehelper.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QFile>
#include <QStringBuilder>
#include "core/csv/qxtcsvmodel.h"

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
    return execSqlScript(&f) && loadPackagesFromCSV();
}

int findCsvColumn(const QxtCsvModel * tableModel, const QString & colName){
    for(int col; col < tableModel->columnCount(); ++col){
        const QString name = tableModel->headerText(col);
        if(colName.compare(name, Qt::CaseInsensitive) == 0){
            return col;
        }
    }
    return -1;
}

bool DatabaseHelper::loadPackagesFromCSV() const {
    QxtCsvModel csvModel;
    csvModel.setSource(":/csv/res/packages.csv", true);
    int nameCol = findCsvColumn(&csvModel, "name");
    int descriptionCol = findCsvColumn(&csvModel, "description");
    if(nameCol < 0 || descriptionCol < 0){
        qDebug()<<"Can not load packages.csv. Some columns are missing";
        return false;
    }

    QSqlQuery query("INSERT INTO package (name, description) VALUES(?,?)", _db);
    int rows = csvModel.rowCount();
    for(int row = 0; row < rows; ++row){
        QString name = csvModel.text(row, nameCol);
        QString description = csvModel.text(row, descriptionCol);
        query.bindValue(0, name);
        query.bindValue(1, description);
        bool res = query.exec();
        if(!res){
            qWarning()<<"Failed to insert package. Reason:"<<query.lastError();
            return false;
        }
    }
    return true;
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

