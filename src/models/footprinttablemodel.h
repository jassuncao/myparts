#ifndef FOOTPRINTTABLEMODEL_H
#define FOOTPRINTTABLEMODEL_H

#include <QSqlTableModel>
#include <QSqlRecord>

class FootprintTableModel : public QSqlTableModel
{
    Q_OBJECT
public:    

    FootprintTableModel(QObject *parent = 0, QSqlDatabase db = QSqlDatabase());
    ~FootprintTableModel();
    //virtual bool setData(const QModelIndex &index, const QVariant &value, int role);
    //virtual QString selectStatement() const;
    virtual bool insertRowIntoTable(const QSqlRecord &values);
    virtual bool updateRowInTable(int row, const QSqlRecord &values);
    virtual bool deleteRowFromTable(int row);
private:
    enum ColumnsIndex {
        ColumnId = 0,
        ColumnName,
        ColumnDescription,
        ColumnFilename
    };
    //void handleFilenameChanges(QSqlRecord &rec);    
};

#endif // FOOTPRINTTABLEMODEL_H
