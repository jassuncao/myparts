#include "attachmenttablemodel2.h"

#include <QSqlError>
#include <QStandardItemModel>
#include <QStyleOptionComboBox>
#include <QDebug>

namespace {
    const int MaxColumns = 2;
}

AttachmentTableModel2::AttachmentTableModel2(const QString & tableName, const QString & associationField, QObject *parent) : QAbstractTableModel(parent)
{
    QString selectStmt("SELECT id, url, description FROM %1 WHERE %2 = ?");
    QString insertStmt("INSERT INTO %1 (url, description, %2) VALUES (?,?,?)");
    QString updateStmt("UPDATE %1 SET url=?, description=? WHERE id = ?");
    QString deleteStmt("DELETE FROM %1 WHERE id = ?");


    _selectAttachmentsQuery.prepare(selectStmt.arg(tableName).arg(associationField));
    _insertAttachmentsQuery.prepare(insertStmt.arg(tableName).arg(associationField));
    _updateAttachmentsQuery.prepare(updateStmt.arg(tableName));
    _deleteAttachmentsQuery.prepare(deleteStmt.arg(tableName));
}

AttachmentTableModel2::~AttachmentTableModel2()
{

}

QVariant AttachmentTableModel2::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();
    if (orientation == Qt::Horizontal) {
        switch (section) {
            case ColumnURL: return tr("Location");
            case ColumnDescription: return tr("Description");
            default: Q_ASSERT(false);
        }
    }
    return section + 1;
}

int AttachmentTableModel2::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : _items.count();
}


int AttachmentTableModel2::columnCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : MaxColumns;
}

Qt::ItemFlags AttachmentTableModel2::flags(const QModelIndex &index) const
{
    Qt::ItemFlags theFlags = QAbstractTableModel::flags(index);
    if (index.isValid()){
        theFlags |= Qt::ItemIsSelectable | Qt::ItemIsEnabled;
        if(index.column()!=ColumnURL){
            theFlags |=Qt::ItemIsEditable;
        }
    }
    return theFlags;
}

QVariant AttachmentTableModel2::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() ||
        index.row() < 0 || index.row() >= _items.count() ||
        index.column() < 0 || index.column() >= MaxColumns) {
        return QVariant();
    }

    const AttachmentItem &item = _items.at(index.row());
    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        switch (index.column()) {
            case ColumnURL: return item._url;
            case ColumnDescription: return item._description;
            default: Q_ASSERT(false);
        }
    }
    return QVariant();
}

bool AttachmentTableModel2::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || role != Qt::EditRole ||
        index.row() < 0 || index.row() >= _items.count() ||
        index.column() < 0 || index.column() >= MaxColumns){
        return false;
    }
    AttachmentItem &item = _items[index.row()];
    switch (index.column()) {
        case ColumnURL: {
            item._url = value.toString();
            break;
        }
        case ColumnDescription: {
            item._description = value.toString();
            break;
        }
        default: Q_ASSERT(false);
    }
    item._dirty = true;
    emit dataChanged(index, index);
    return true;
}

bool AttachmentTableModel2::appendRow(const QString & url, const QString & description)
{
    int row = _items.count();
    beginInsertRows(QModelIndex(), row, row);
        _items.append(AttachmentItem(-1, url, description, true));
    endInsertRows();
    return true;
}

bool AttachmentTableModel2::insertRows(int row, int count, const QModelIndex&)
{
    beginInsertRows(QModelIndex(), row, row + count - 1);
    for (int i = 0; i < count; ++i) {
        _items.insert(row, AttachmentItem());
    }
    endInsertRows();
    return true;
}


bool AttachmentTableModel2::removeRows(int row, int count, const QModelIndex&)
{
    beginRemoveRows(QModelIndex(), row, row + count - 1);
    for (int i = 0; i < count; ++i) {
        const AttachmentItem & item = _items.at(row);
        if(item._id>0){
            _deletedItems.append(item._id);
            _items.remove(row);
        }
    }
    endRemoveRows();
    return true;
}


void AttachmentTableModel2::load(const QVariant & ownerId)
{
    beginResetModel();
    _items.clear();
    if(ownerId.isValid()){
        _selectAttachmentsQuery.bindValue(0, ownerId);
        if(_selectAttachmentsQuery.exec()){
            while(_selectAttachmentsQuery.next())
            {
                QVariant id = _selectAttachmentsQuery.value(0);
                QVariant url = _selectAttachmentsQuery.value(1);
                QVariant description = _selectAttachmentsQuery.value(2);

                AttachmentItem item(id.toInt(), url.toString(), description.toString());
                _items.append(item);
            }
        }
        else{
            qWarning()<<"Failed to execute query. Reason "<<_selectAttachmentsQuery.lastError().driverText();
        }

    }
    else{
        qDebug()<<"Owner entity id is invalid";
    }
    endResetModel();
}

void AttachmentTableModel2::save(const QVariant & ownerId)
{
    while (!_deletedItems.isEmpty()){
        int attachmentId = _deletedItems.takeFirst();
        qDebug()<<"Deleting attachment "<<attachmentId;
        _deleteAttachmentsQuery.bindValue(0, attachmentId);
        if(!_deleteAttachmentsQuery.exec()){
            qWarning()<<"Failed to execute delete query. Reason: "<<_deleteAttachmentsQuery.lastError().driverText();
        }
    }

    for(int i=0; i<_items.count(); ++i){
        AttachmentItem &item = _items[i];
        if(item._dirty){
            if(item._id<0){
                //Brand new item. Insert
                qDebug()<<"Inserting new attachment ";
                _insertAttachmentsQuery.bindValue(0, item._url);
                _insertAttachmentsQuery.bindValue(1, item._description);
                _insertAttachmentsQuery.bindValue(2, ownerId);
                if(_insertAttachmentsQuery.exec()){
                    item._dirty = false;
                }
                else{
                    qWarning()<<"Failed to execute insert query. Reason: "<<_insertAttachmentsQuery.lastError().driverText();
                }
            }
            else{
                //Edited item. Update
                qDebug()<<"Updating attachment "<<item._id;
                _updateAttachmentsQuery.bindValue(0, item._url);
                _updateAttachmentsQuery.bindValue(1, item._description);
                _updateAttachmentsQuery.bindValue(2, item._id);
                if(_updateAttachmentsQuery.exec()){
                    item._dirty = false;
                }
                else{
                    qWarning()<<"Failed to execute update query. Reason: "<<_updateAttachmentsQuery.lastError().driverText();
                }
            }
        }
    }
}

