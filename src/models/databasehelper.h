#ifndef DATABASEHELPER_H
#define DATABASEHELPER_H

#include <QSqlDatabase>

class DatabaseHelper
{
public:
    DatabaseHelper(QSqlDatabase db = QSqlDatabase());
protected:
    bool runCreateScript(const char * script) const;
    bool createUnitTable() const;
    bool createPartUnitTable() const;
private:
    QSqlDatabase _db;
};

#endif // DATABASEHELPER_H
