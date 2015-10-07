#ifndef PARTPARAMETERTABLEMODEL_H
#define PARTPARAMETERTABLEMODEL_H

#include <QSqlRelationalTableModel>

class PartParameterTableModel : public QSqlRelationalTableModel
{
    Q_OBJECT
public:
    enum ColumnsIndex {
        ColumnId = 0,
        ColumnName,
        ColumnValue,
        ColumnUnit,
        ColumnPartId,
        ColumnDescription,
    };
    PartParameterTableModel(QObject *parent = 0, QSqlDatabase db = QSqlDatabase());
    ~PartParameterTableModel();
    void setSelectedPart(const QVariant & partId, bool refresh);
    void copyTo(QVector<QSqlRecord> & dest);
    void copyFrom(const QVector<QSqlRecord> & source);
protected:
    //bool insertRowIntoTable(const QSqlRecord &values);
private slots:
    void slotPrimeInsert(int, QSqlRecord &record);
    void slotBeforeInsert(QSqlRecord &record);
private:
     QVariant _selectedPartId;
};

#endif // PARTPARAMETERTABLEMODEL_H
