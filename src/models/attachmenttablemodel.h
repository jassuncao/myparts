#ifndef ATTACHMENTTABLEMODEL_H
#define ATTACHMENTTABLEMODEL_H

#include <QSqlTableModel>
#include <QSqlRecord>
#include <QDir>

class AttachmentTableModel : public QSqlTableModel
{
    Q_OBJECT
protected:
    AttachmentTableModel(int filenameColumnIdx, QString targetPrefix, QObject *parent = 0, QSqlDatabase db = QSqlDatabase());
public:    
    ~AttachmentTableModel();
    virtual bool insertRowIntoTable(const QSqlRecord &values);
    virtual bool updateRowInTable(int row, const QSqlRecord &values);
    virtual bool deleteRowFromTable(int row);
private:
    const int _filenameColumnIdx;
    QString _targetPrefix;
    QDir _targetDir;
};


struct PartFootprintColumn {
    typedef enum {
        ColumnId = 0,
        ColumnName,
        ColumnDescription,
        ColumnFilename
    } Index;
};


struct PartAttachmentColumn {
    typedef enum {
        ColumnId = 0,
        ColumnPartId,
        ColumnFilename,
        ColumnSize,
        ColumnDescription
    } Index;
};

#endif // ATTACHMENTTABLEMODEL_H
