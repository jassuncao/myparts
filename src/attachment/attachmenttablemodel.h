#ifndef ATTACHMENTTABLEMODEL_H
#define ATTACHMENTTABLEMODEL_H

#include "core/sql/customtablemodel.h"

class AttachmentTableModel : public SimpleSqlTableModel
{
    Q_OBJECT
public:
    enum ColumnsIndex {
        ColumnURL,
        ColumnDescription
    };

    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool appendRow(const QString &url, const QString & description);
    inline void setCurrentPartId(const QVariant & partId) { setCurrentForeignKey(partId); }
    Qt::DropActions supportedDropActions() const;
    bool dropMimeData(const QMimeData *data, Qt::DropAction, int row, int column, const QModelIndex &);

    static AttachmentTableModel * createNewPackageAttachmentModel(QObject *parent);
    static AttachmentTableModel * createNewPartAttachmentModel(QObject *parent);
    static AttachmentTableModel * createNewProjectAttachmentModel(QObject *parent);

protected:
    explicit AttachmentTableModel(const QString &tableName, const QStringList & fieldNames,
                                            const QStringList & columnNames, const QString & foreignKeyField, QObject *parent = 0);
};

#endif // ATTACHMENTTABLEMODEL_H
