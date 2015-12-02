#ifndef PACKAGETABLEMODEL_H
#define PACKAGETABLEMODEL_H

#include <QSqlTableModel>
#include <QSqlRecord>
#include <QDir>

class PackageTableModel : public QSqlTableModel
{
    Q_OBJECT
public:    
    enum ColumnsIndex {
        ColumnId = 0,
        ColumnName,
        ColumnDescription,
        ColumnFilename
    };

    PackageTableModel(QObject *parent = 0, QSqlDatabase db = QSqlDatabase());
    ~PackageTableModel();
    virtual bool insertRowIntoTable(const QSqlRecord &values);
    virtual bool updateRowInTable(int row, const QSqlRecord &values);
    virtual bool deleteRowFromTable(int row);
private:
    QDir _targetDir;
};

#endif // PACKAGETABLEMODEL_H
