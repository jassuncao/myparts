#include "partattachmenttablemodel.h"

PartAttachmentTableModel::PartAttachmentTableModel(QObject *parent, QSqlDatabase db) :
    AttachmentTableModel(ColumnFilename,"attachments", parent, db)
{
    setTable("part_attachment");
}

PartAttachmentTableModel::~PartAttachmentTableModel()
{
}

