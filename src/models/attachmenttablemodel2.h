#ifndef ATTACHMENTTABLEMODEL2_H
#define ATTACHMENTTABLEMODEL2_H

#include <QAbstractTableModel>
#include <QList>
#include <QVector>
#include <QSqlQuery>
#include <QModelIndex>

struct AttachmentItem {

    explicit AttachmentItem(int id=-1, const QString & url = QString(), const QString & description = QString(), const bool dirty=false) :
        _id(id), _url(url), _description(description), _dirty(dirty) {}

    int _id;
    QString _url;
    QString _description;
    bool _dirty;
};

class AttachmentTableModel2 : public QAbstractTableModel
{
    Q_OBJECT
public:
    enum ColumnsIndex {
        ColumnURL,
        ColumnDescription
    };
    AttachmentTableModel2(const QString & tableName, const QString & associationField, QObject *parent = 0);
    ~AttachmentTableModel2();

    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    bool appendRow(const QString & url, const QString & description);
    bool insertRows(int row, int count, const QModelIndex&);
    bool removeRows(int row, int count, const QModelIndex&);
    void load(const QVariant & parentId);
    void save(const QVariant & ownerId);
private:
    QVector<AttachmentItem> _items;
    QList<int> _deletedItems;
    QSqlQuery _selectAttachmentsQuery;
    QSqlQuery _deleteAttachmentsQuery;
    QSqlQuery _updateAttachmentsQuery;
    QSqlQuery _insertAttachmentsQuery;
};

#endif // ATTACHMENTTABLEMODEL2_H
