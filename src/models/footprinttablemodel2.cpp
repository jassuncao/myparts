#include "footprinttablemodel2.h"

FootprintTableModel2::FootprintTableModel2(QObject *parent, QSqlDatabase db) :
    AttachmentTableModel(ColumnImageFilename,"footprints", parent, db)
{
    setTable("part_footprint");
}

FootprintTableModel2::~FootprintTableModel2()
{
}

