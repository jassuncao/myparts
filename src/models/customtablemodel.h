#ifndef CUSTOMTABLEMODEL_H
#define CUSTOMTABLEMODEL_H

#include <QAbstractTableModel>
#include <QAbstractListModel>
#include <QList>
#include <QVector>
#include <QHash>
#include <QMap>
#include <QSqlQuery>
#include <QStyledItemDelegate>
#include <QComboBox>

class TableItem {
public:
    explicit TableItem(QVector<QVariant> data, const QVariant & id = QVariant(), bool dirty=true);
    void setData(int column, const QVariant & value);
    QVariant data(int column) const;

    inline bool dirty() const { return _dirty; }
    void resetDirty() { _dirty = false; }

    inline QVariant id() const {return _id;}
    void setId(QVariant id);
private:
    QVector<QVariant> _data;
    QVariant _id;
    bool _dirty;
};

class TableRelation : public QAbstractListModel{
public:
    explicit TableRelation(const QString & tableName, const QString & indexField, const QString & displayField);
    ~TableRelation();
    QVariant displayValue(const QVariant foreignId) const;
    bool validId(const QVariant foreignId) const;
    inline bool initialized() const { return _initialized; }
    QAbstractListModel * model();    
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;
    void populateDictionary();
private:
    const QString _tableName;
    const QString _displayField;
    const QString _indexField;
    bool _initialized;
    QHash<int, QVariant> _lookupDictionary;
    QList<QPair<QString,int> > _items;
    QSqlQuery _selectQuery;
};


class CustomTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit CustomTableModel(int maxColumns, QObject *parent = 0);
    virtual ~CustomTableModel();
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role = Qt::EditRole);
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    bool insertRows(int row, int count, const QModelIndex&);
    bool removeRows(int row, int count, const QModelIndex&);    
    void select();
    bool submitAll();
    void createRelation(const int column, const QString & tableName, const QString & indexField, const QString & displayField);
    QAbstractListModel * relationModel(const int column) const;
    void cloneData();
protected:
    virtual TableItem * createBlankItem() const;
    virtual bool deleteItem(QVariant id) = 0;
    virtual bool saveItem(TableItem* item) = 0;
    virtual bool loadItems(QList<TableItem*> & dest) = 0;
    virtual bool doInsertRow(int row, TableItem* item);
    bool columnIsRelation(const int column) const;

    const int _maxColumns;
    QList<TableItem*> _items;
    QList<QVariant> _deletedItems;
    QVector<QHash<int, QVariant> > _headers;
    QVector<TableRelation*> _relations;
};

class SimpleSqlTableModel : public CustomTableModel
{
    Q_OBJECT
public:
    explicit SimpleSqlTableModel(const QString &tableName, const QStringList & fieldNames,
                                 const QStringList & columnNames, const QString & foreignKeyField = QString(), QObject *parent = 0);
    void setCurrentForeignKey(const QVariant & foreignKey);
    void sort(int column, Qt::SortOrder order = Qt::AscendingOrder);
protected:
    virtual bool deleteItem(QVariant id);
    virtual bool saveItem(TableItem* item) ;
    virtual bool loadItems(QList<TableItem *> &dest);
    QString generateSelectStatement(const QString & tableName,  const QStringList & fieldNames, const QString & foreignKeyField, int sortColumn, Qt::SortOrder order);
    const QString _tableName;
    const QStringList _fieldNames;    
    const QString _foreignKeyField;
    QVariant _foreignKey;
    QSqlQuery _selectQuery;
    QSqlQuery _insertQuery;
    QSqlQuery _updateQuery;
    QSqlQuery _deleteQuery;
    int _sortColumn;
    Qt::SortOrder _order;
};

class CustomTableRelationDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit CustomTableRelationDelegate(QObject *parent = 0)
        : QStyledItemDelegate(parent)
    {}

    ~CustomTableRelationDelegate()
    {}

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
    {
        const CustomTableModel *tableModel = qobject_cast<const CustomTableModel *>(index.model());
        QAbstractListModel *childModel = tableModel ? tableModel->relationModel(index.column()) : 0;
        if (!childModel)
            return QStyledItemDelegate::createEditor(parent, option, index);

        QComboBox *combo = new QComboBox(parent);
        combo->setModel(childModel);
        combo->installEventFilter(const_cast<CustomTableRelationDelegate *>(this));
        return combo;
    }

    void setEditorData(QWidget *editor, const QModelIndex &index) const
    {
        const CustomTableModel *tableModel = qobject_cast<const CustomTableModel *>(index.model());
        QComboBox *combo = qobject_cast<QComboBox *>(editor);
        if (!tableModel || !combo) {
            QStyledItemDelegate::setEditorData(editor, index);
            return;
        }
        combo->setCurrentIndex(combo->findText(tableModel->data(index, Qt::DisplayRole).toString()));
    }

    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
    {
        if (!index.isValid())
            return;

        CustomTableModel *tableModel = qobject_cast<CustomTableModel *>(model);
        QAbstractListModel *childModel = tableModel ? tableModel->relationModel(index.column()) : 0;
        QComboBox *combo = qobject_cast<QComboBox *>(editor);
        if (!tableModel || !childModel || !combo) {
            QStyledItemDelegate::setModelData(editor, model, index);
            return;
        }

        int currentItem = combo->currentIndex();
        QVariant value = childModel->data(childModel->index(currentItem), Qt::EditRole);
        tableModel->setData(index, value, Qt::EditRole);
    }
};


class AttachmentTableModel3 : public SimpleSqlTableModel
{
    Q_OBJECT
public:
    enum ColumnsIndex {
        ColumnURL,
        ColumnDescription
    };

    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool appendRow(const QString &url, const QString & description);

    static AttachmentTableModel3 * createNewPackageAttachmentModel(QObject *parent);
    static AttachmentTableModel3 * createNewPartAttachmentModel(QObject *parent);

protected:
    explicit AttachmentTableModel3(const QString &tableName, const QStringList & fieldNames,
                                            const QStringList & columnNames, const QString & foreignKeyField, QObject *parent = 0);
};


class PartManufacturerTableModel2 : public SimpleSqlTableModel
{
    Q_OBJECT
public:
    enum ColumnsIndex {
        ColumnManufacturer,
        ColumnPartNumber
    };

    inline void setCurrentPartId(const QVariant & partId) { setCurrentForeignKey(partId); }

    static PartManufacturerTableModel2 * createNew(QObject *parent);

protected:
    explicit PartManufacturerTableModel2(const QStringList & fieldNames, const QStringList & columnNames, QObject *parent = 0);
};

class PartDistributorTableModel2 : public SimpleSqlTableModel
{
    Q_OBJECT
public:
    enum ColumnsIndex {
        ColumnDistributor,
        ColumnPartNumber,
        ColumnMinimumOrder,
        ColumnUnitPrice,
        ColumnPackaging,
    };

    inline void setCurrentPartId(const QVariant & partId) { setCurrentForeignKey(partId); }

    static PartDistributorTableModel2 * createNew(QObject *parent);
protected:
    explicit PartDistributorTableModel2(const QStringList & fieldNames, const QStringList & columnNames, QObject *parent = 0);
};

class PartParametersTableModel3 : public SimpleSqlTableModel
{
    Q_OBJECT
public:
    enum ColumnsIndex {
        ColumnName,
        ColumnValue,
        ColumnUnit,
        ColumnDescription,
        HiddenColumnUnitSymbol,
    };

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    inline void setCurrentPartId(const QVariant & partId) { setCurrentForeignKey(partId); }

    static PartParametersTableModel3 * createNew(QObject *parent);
protected:
    explicit PartParametersTableModel3(const QStringList & fieldNames, const QStringList & columnNames, QObject *parent = 0);
};

class PartStockTableModel : public SimpleSqlTableModel
{
    Q_OBJECT
public:
    enum ColumnsIndex {
        ColumnDateTime,
        ColumnChange,
        ColumnPrice,
        ColumnComment
    };

    inline void setCurrentPartId(const QVariant & partId) { setCurrentForeignKey(partId); }
    static PartStockTableModel * createNew(QObject *parent);
    bool appendRow(const int quantity, const QVariant & price, const QString & comment);
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
protected:
    explicit PartStockTableModel(const QStringList & fieldNames, const QStringList & columnNames, QObject *parent = 0);
};


#endif // CUSTOMTABLEMODEL_H
