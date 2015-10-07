#ifndef PARTATTACHMENTTABLEMODEL_H
#define PARTATTACHMENTTABLEMODEL_H

#include "attachmenttablemodel.h"

class PartAttachmentTableModel : public AttachmentTableModel
{
    Q_OBJECT
public:
    typedef enum {
        ColumnId = 0,
        ColumnPartId,
        ColumnFilename,
        ColumnSize,
        ColumnDescription
    } Columns;

    PartAttachmentTableModel(QObject *parent = 0, QSqlDatabase db = QSqlDatabase());
    ~PartAttachmentTableModel();
};

#endif // PARTATTACHMENTTABLEMODEL_H
