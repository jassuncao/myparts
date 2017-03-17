#include "partssqltablemodel.h"
#include <QDebug>
#include <QSqlRecord>
#include <QSqlField>
#include <QSqlDriver>
#include <QSqlError>
#include <QMimeData>
#include <QSqlResult>
#include <QSqlQuery>
#include <QHash>
#include "partsquerybuilder.h"
#include "partmimedata.h"

PartsSqlTableModel::PartsSqlTableModel(PartsQueryBuilder * partsQueryBuilder, QObject *parent) :
    QSqlTableModel(parent),
    _partsQueryBuilder(partsQueryBuilder)
{
    setTable("part");
    qDebug()<<"Column Count "<<columnCount();
    setColumnName(ColumnName,tr("Name"));
    setColumnName(ColumnDescription,tr("Description"));
    setColumnName(ColumnTotalStock,tr("Stock"));
    setColumnName(ColumnMinStock,tr("Min. Stock"));
    setColumnName(ColumnAvgPrice,tr("Avg. Price"));
    setColumnName(ColumnCustomPartNumber,tr("Custom Part#"));
    setColumnName(ColumnComment,tr("Comment"));
    setColumnName(ColumnCreateDate,tr("Create Date"));
    setColumnName(ColumnCategoryName,tr("Category")); 
    setColumnName(ColumnPackageName,tr("Package"));
}

PartsSqlTableModel::~PartsSqlTableModel()
{
}

PartsQueryBuilder * PartsSqlTableModel::queryBuilder() const
{
    return _partsQueryBuilder;
}

void PartsSqlTableModel::setColumnName(int section, const QString & columnLabel)
{
    setHeaderData(section, Qt::Horizontal, columnLabel, Qt::EditRole);
    setHeaderData(section, Qt::Horizontal, true, VISIBILITY_COLUMN_ROLE);
}

int PartsSqlTableModel::columnCount(const QModelIndex &index) const
{
    return index.isValid() ? 0 : LAST_COLUMN;
}

void PartsSqlTableModel::setSort(int column, Qt::SortOrder order)
{
    if(_partsQueryBuilder){
        _partsQueryBuilder->setSort(column, order);
    }
}

QString PartsSqlTableModel::selectStatement() const
{
    if(_partsQueryBuilder){
        return _partsQueryBuilder->buildQuery();
    }
    return QSqlTableModel::selectStatement();
}

bool PartsSqlTableModel::insertRowIntoTable(const QSqlRecord &values)
{
    QSqlRecord copy;
    for(int i=0; i< FAKE_COLUMNS_INDEX;++i){
        copy.append(values.field(i));
    }
    bool res = QSqlTableModel::insertRowIntoTable(copy);

    _lastInsertedId =  query().lastInsertId();

    if(!res){
        qDebug()<<"Error "<<lastError().text();
    }
    else{
        qDebug()<<"SUCCESS "<<_lastInsertedId;
    }
    return res;
}

bool PartsSqlTableModel::updateRowInTable(int row, const QSqlRecord &values)
{
    bool res = QSqlTableModel::updateRowInTable(row, values);
    if(!res){
        qDebug()<<"Update Error in row "<<row<<" "<<lastError().text();
    }
    else{
        qDebug()<<"Update SUCCESS ";
    }
    return res;
}

QVariant PartsSqlTableModel::data(const QModelIndex &idx, int role) const
{
    const QVariant & value = QSqlTableModel::data(idx, role);
    if(idx.column()==ColumnCreateDate && value.isValid() && (role==Qt::EditRole || role==Qt::DisplayRole)){
        return QDateTime::fromTime_t(value.toUInt());
    }
    return value;
}

Qt::ItemFlags PartsSqlTableModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flags = QSqlTableModel::flags(index);
    flags = flags | Qt::ItemIsDragEnabled;
    return flags;
}

Qt::DropActions PartsSqlTableModel::supportedDropActions() const
{
    return Qt::CopyAction | Qt::LinkAction;
}

QStringList PartsSqlTableModel::mimeTypes() const
{
    QStringList types;    
    types << PartMimeData::PART_MIME_TYPE << "text/plain" << "text/html";
    return types;
}

QMimeData * PartsSqlTableModel::mimeData(const QModelIndexList &indexes) const
{
    QMimeData * mimeData = new QMimeData;
    QByteArray encodedData;
    QDataStream stream(&encodedData, QIODevice::WriteOnly);
    QVector<int> partIds;
    foreach (const QModelIndex &index, indexes) {
        if (index.isValid()) {
            QSqlRecord recordData = record(index.row());
            QVariant partId = recordData.value(ColumnId);
            partIds.append(partId.toInt());
        }
    }
    stream << partIds;
    mimeData->setData("myparts/part", encodedData);
    return mimeData;
}

bool PartsSqlTableModel::updatePartAvgPrice(const QModelIndex & currentIndex, double partPrice)
{
    int row = currentIndex.row();
    const QModelIndex & colIndex = index(row, ColumnAvgPrice);
    QVariant value = data(colIndex, Qt::EditRole);
    bool ok;
    double currentAvgPrice = value.toDouble(&ok);
    if(ok && currentAvgPrice!=0){
        currentAvgPrice = (currentAvgPrice + partPrice) / 2.0;
    }
    else {
        currentAvgPrice = partPrice;
    }
    return setData(colIndex, currentAvgPrice, Qt::EditRole);
}

bool PartsSqlTableModel::updatePartStock(const QModelIndex & currentIndex, int stockChange)
{
    int row = currentIndex.row();
    const QModelIndex & colIndex = index(row, ColumnTotalStock);
    QVariant value = data(colIndex, Qt::EditRole);
    int currentStock = value.toInt();
    currentStock+=stockChange;
    return setData(colIndex, currentStock, Qt::EditRole);
}

/*
bool PartsSqlTableModel::setPartStock(const QModelIndex & currentIndex, int newStockValue)
{
    QVariant partId = index(currentIndex.row(), PartsSqlTableModel::ColumnId).data(Qt::EditRole);
    const QModelIndex stockModelIndex = index(currentIndex.row(), PartsSqlTableModel::ColumnActualStock);
    QVariant oldValue = stockModelIndex.data(Qt::EditRole);
    int stockChange = newStockValue - oldValue.toInt();    

    database().transaction();

    QSqlQuery query(database());
    query.prepare("INSERT INTO stock_change (change, dateTime, part) "
                  "VALUES(?,?,?)");
    query.bindValue(0, stockChange);
    query.bindValue(1, QDateTime::currentDateTimeUtc().toTime_t());
    query.bindValue(2, partId);
    bool res = query.exec();

    if(res){
        setData(stockModelIndex, newStockValue);
        res = submitAll();
    }

    if(res){
        database().commit();
    }
    else{
        database().rollback();
    }
    return res;
}
*/

void PartsSqlTableModel::updatePartsCategory(QVector<int> parts, int categoryId)
{
    QSqlQuery query(database());
    database().transaction();
    query.prepare("UPDATE part SET category=? WHERE id=?");
    query.bindValue(0, categoryId);
    int partId;
    foreach (partId, parts) {
        query.bindValue(1,partId);
        query.exec();
    }
    database().commit();
}

/*
void PartsSqlTableModel::updatePartsStorage(QVector<int> parts, int storageId)
{
    QSqlQuery query(database());
    database().transaction();
    query.prepare("UPDATE part SET storage=? WHERE id=?");
    query.bindValue(0, storageId);
    int partId;
    foreach (partId, parts) {
        query.bindValue(1, partId);
        query.exec();
    }
    database().commit();
}
*/

QModelIndex PartsSqlTableModel::findIndex(QVariant partId)
{
    int row = 0;
    int count = rowCount();
    while(row<count){
        QModelIndex rowIndex = index(row, ColumnId);
        QVariant other = data(rowIndex, Qt::EditRole);
        if(partId==other){
            return rowIndex;
        }
        ++row;
    }
    return QModelIndex();
}

bool PartsSqlTableModel::submitAll()
{
    emit beforeSubmit();
    bool res = QSqlTableModel::submitAll();
    emit afterSubmit();
    return res;
}

void PartsSqlTableModel::filterChanged()
{
    select();
}


