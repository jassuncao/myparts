#ifndef PARTDISTRIBUTORTABLEMODEL_H
#define PARTDISTRIBUTORTABLEMODEL_H

#include <QSqlRelationalTableModel>

class PartDistributorTableModel : public QSqlRelationalTableModel
{
    Q_OBJECT
public:
    enum ColumnsIndex {
        ColumnId = 0,
        ColumnPartId,
        ColumnDistributorId,
        ColumnPartNumber,
        ColumnMinimumOrder,
        ColumnUnitPrice,
        ColumnPackagingId,
    };
    PartDistributorTableModel(QObject *parent = 0, QSqlDatabase db = QSqlDatabase());
    ~PartDistributorTableModel();
    void setSelectedPart(const QVariant & partId, bool refresh);
    void copyTo(QVector<QSqlRecord> & dest);
    void copyFrom(const QVector<QSqlRecord> & source);
protected:
private slots:
    void slotPrimeInsert(int, QSqlRecord &record);
    void slotBeforeInsert(QSqlRecord &record);
private:
     QVariant _selectedPartId;
};

#endif // PARTDISTRIBUTORTABLEMODEL_H
