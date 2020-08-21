#include "attachmenttablemodel.h"
#include <QMimeData>
#include <QDebug>

AttachmentTableModel::AttachmentTableModel(const QString &tableName, const QStringList & fieldNames,
                                             const QStringList & columnNames, const QString & foreignKeyField, QObject *parent)
    : SimpleSqlTableModel(tableName, fieldNames, columnNames, foreignKeyField, parent)
{
}

Qt::ItemFlags AttachmentTableModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags theFlags = SimpleSqlTableModel::flags(index) | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
    if(index.column()==ColumnURL){
        theFlags = theFlags & (~Qt::ItemIsEditable);
    }
    return theFlags;
}

bool AttachmentTableModel::appendRow(const QString & url, const QString & description)
{
    TableItem * item = createBlankItem();
    item->setData(ColumnURL, url);
    item->setData(ColumnDescription, description);
    doInsertRow(rowCount(), item);
    return true;
}

Qt::DropActions AttachmentTableModel::supportedDropActions() const
{
    return Qt::MoveAction | Qt::CopyAction | Qt::LinkAction;
}

bool AttachmentTableModel::dropMimeData(const QMimeData *data, Qt::DropAction, int row, int column, const QModelIndex &)
{
    bool success = false;
    qDebug()<<"Dropping "<<data<<" At "<<row<<","<<column;
    if(data->hasUrls()){
        int newRow = rowCount();
        success = insertRows(newRow, data->urls().size(), QModelIndex());
        if(success){
            foreach(QUrl url, data->urls()) {
                const QModelIndex idx = index(newRow, ColumnURL);
                setData(idx, url.toString());
                newRow++;
            }
        }
        else{
            qWarning()<<"Failed to append rows to attachments model";
        }
    }
    return success;
}

AttachmentTableModel * AttachmentTableModel::createNewPartAttachmentModel(QObject * parent)
{
    QStringList fieldNames;
    QStringList columnNames;
    fieldNames<<"url"<<"description";
    columnNames<<tr("Location")<<tr("Description");
    return new AttachmentTableModel("part_attachment", fieldNames, columnNames, "part", parent);
}

AttachmentTableModel * AttachmentTableModel::createNewPackageAttachmentModel(QObject * parent)
{
    QStringList fieldNames;
    QStringList columnNames;
    fieldNames<<"url"<<"description";
    columnNames<<tr("Location")<<tr("Description");
    return new AttachmentTableModel("package_attachment", fieldNames, columnNames, "package", parent);
}

AttachmentTableModel * AttachmentTableModel::createNewProjectAttachmentModel(QObject * parent)
{
    QStringList fieldNames;
    QStringList columnNames;
    fieldNames<<"url"<<"description";
    columnNames<<tr("Location")<<tr("Description");
    return new AttachmentTableModel("project_attachment", fieldNames, columnNames, "project", parent);
}
