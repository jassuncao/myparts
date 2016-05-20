#ifndef DATABASEHELPER_H
#define DATABASEHELPER_H

#include <QSqlDatabase>

class QFile;

class DatabaseHelper
{
public:
    DatabaseHelper(QSqlDatabase db = QSqlDatabase());    
    bool createDatabase(void) const;
    bool loadInitialData(void) const;
private:
    bool execSqlStatement(const QString& queryStr, const int lineNumber) const;
    bool execSqlScript(QFile *file) const;
    bool tableExists(const char * tableName) const;
    QSqlDatabase _db;
};

#endif // DATABASEHELPER_H
