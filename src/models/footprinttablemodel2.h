#ifndef FOOTPRINTTABLEMODEL2_H
#define FOOTPRINTTABLEMODEL2_H

#include "attachmenttablemodel.h"

class FootprintTableModel2 : public AttachmentTableModel
{
    Q_OBJECT
public:
    enum ColumnsIndex {
        ColumnId = 0,
        ColumnName,
        ColumnDescription,
        ColumnImageFilename
    };

    FootprintTableModel2(QObject *parent = 0, QSqlDatabase db = QSqlDatabase());
    ~FootprintTableModel2();
};

#endif // FOOTPRINTTABLEMODEL2_H
