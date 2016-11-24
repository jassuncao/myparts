#ifndef BASICENTITYTABLEMODEL_H
#define BASICENTITYTABLEMODEL_H

#include <QSqlTableModel>

class BasicEntityTableModel : public QSqlTableModel
{    
public:
    explicit BasicEntityTableModel(QObject *parent = 0, QSqlDatabase db = QSqlDatabase());    
    virtual void setTextFilter(const QString& text) ;
    virtual int getNameColumn() const = 0;    
    virtual QModelIndex findIndexOfKeyValue(const QVariant& keyValue) = 0;
    virtual QVariant keyValue(int row) = 0;
    virtual QVariant lastInsertedId() const;
protected:
    virtual QModelIndex findIndexOf(const QVariant& value, int columnIndex, int role = Qt::EditRole);
};

class CompanyTableModel : public BasicEntityTableModel {
public:
    enum ColumnsIndex {
        ColumnId = 0,
        ColumnName,
        ColumnWebsite,
        ColumnComment
    };
    explicit CompanyTableModel(QObject *parent = 0, QSqlDatabase db = QSqlDatabase());

    //virtual void setTextFilter(const QString& text) ;
    int getNameColumn() const;
    //QModelIndex rootIndex(int row) const;
    QModelIndex findIndexOfKeyValue(const QVariant& keyValue);
    QVariant keyValue(int row);
};

class PackageTableModel : public BasicEntityTableModel {
public:
    enum ColumnsIndex {
        ColumnId = 0,
        ColumnName,
        ColumnDescription,
        ColumnFilename
    };
    explicit PackageTableModel(QObject *parent = 0, QSqlDatabase db = QSqlDatabase());
    //virtual void setTextFilter(const QString& text) ;
    int getNameColumn() const;
    //QModelIndex rootIndex(int row) const;
    QModelIndex findIndexOfKeyValue(const QVariant& keyValue);
    QVariant keyValue(int row);
};

#endif // BASICENTITYTABLEMODEL_H
