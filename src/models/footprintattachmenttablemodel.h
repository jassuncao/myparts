#ifndef FOOTPRINTATTACHMENTTABLEMODEL_H
#define FOOTPRINTATTACHMENTTABLEMODEL_H

#include "attachmenttablemodel.h"

/*
 * Notes to myself
 * Requirements:
 * -Support remote attachments (remote links)
 * -Support local attachments (local links)
 * Future:
 * -Support local managed attachments
 */

class FootprintAttachmentTableModel : public QSqlTableModel
{
    Q_OBJECT
public:
    enum ColumnsIndex {
        ColumnId = 0,
        ColumnFootprintId,
        ColumnURL,
        ColumnDescription
    };

    FootprintAttachmentTableModel(QObject *parent = 0, QSqlDatabase db = QSqlDatabase());
    ~FootprintAttachmentTableModel();
    void setSelectedFootprint(const QVariant & footprintId, bool refresh);
    //QVariant headerData(int section, Qt::Orientation orientation, int role) const;
private slots:
    void slotPrimeInsert(int, QSqlRecord &record);
    void slotBeforeInsert(QSqlRecord &record);
private:
    QVariant _selectedFootprintId;
};

#endif // FOOTPRINTATTACHMENTTABLEMODEL_H
