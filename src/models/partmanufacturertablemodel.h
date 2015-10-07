#ifndef PARTMANUFACTURERTABLEMODEL_H
#define PARTMANUFACTURERTABLEMODEL_H

#include <QSqlRelationalTableModel>

class PartManufacturerTableModel : public QSqlRelationalTableModel
{
    Q_OBJECT
public:
    enum ColumnsIndex {
        ColumnId = 0,
        ColumnPartId,
        ColumnManufacturerId,
        ColumnPartNumber
    };
    PartManufacturerTableModel(QObject *parent = 0, QSqlDatabase db = QSqlDatabase());
    ~PartManufacturerTableModel();
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

#endif // PARTMANUFACTURERTABLEMODEL_H
