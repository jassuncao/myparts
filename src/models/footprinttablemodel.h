#ifndef FOOTPRINTTABLEMODEL_H
#define FOOTPRINTTABLEMODEL_H

#include <QSqlTableModel>
#include <QSqlRecord>
#include <QDir>

class FootprintTableModel : public QSqlTableModel
{
    Q_OBJECT
public:    
    enum ColumnsIndex {
        ColumnId = 0,
        ColumnName,
        ColumnDescription,
        ColumnFilename
    };

    FootprintTableModel(QObject *parent = 0, QSqlDatabase db = QSqlDatabase());
    ~FootprintTableModel(); 
    virtual bool insertRowIntoTable(const QSqlRecord &values);
    virtual bool updateRowInTable(int row, const QSqlRecord &values);
    virtual bool deleteRowFromTable(int row);
private:
    QDir _targetDir;
};

#endif // FOOTPRINTTABLEMODEL_H
