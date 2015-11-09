#include "partssqlquerymodel2.h"
#include <QDebug>
#include <QSqlRecord>
#include <QSqlField>
#include <QSqlDriver>
#include <QSqlDatabase>
#include <QSqlError>
#include <QStringBuilder>
#include <QMimeData>
#include "partsdao.h"
#include <QSqlResult>
#include <QSqlQuery>

PartsSqlQueryModel2::PartsSqlQueryModel2(QObject *parent) :
    QSqlTableModel(parent)
{
    setTable("part");
    setColumnName(PartsSqlQueryModel2::ColumnName,tr("Name"));
    setColumnName(PartsSqlQueryModel2::ColumnDescription,tr("Description"));
    setColumnName(PartsSqlQueryModel2::ColumnActualStock,tr("Stock"));
    setColumnName(PartsSqlQueryModel2::ColumnMinStock,tr("Min. Stock"));
    setColumnName(PartsSqlQueryModel2::ColumnAvgPrice,tr("Avg. Price"));
    setColumnName(PartsSqlQueryModel2::ColumnCustomPartNumber,tr("Custom Part#"));
    setColumnName(PartsSqlQueryModel2::ColumnComment,tr("Comment"));
    setColumnName(PartsSqlQueryModel2::ColumnCreateDate,tr("Create Date"));
    setColumnName(PartsSqlQueryModel2::ColumnCategoryName,tr("Category"));
    setColumnName(PartsSqlQueryModel2::ColumnStorage,tr("Storage Location"));
    setColumnName(PartsSqlQueryModel2::ColumnCondition,tr("Condition"));
    setColumnName(PartsSqlQueryModel2::ColumnFootprintName,tr("Footprint"));
    /*
    setHeaderData(PartsSqlQueryModel2::ColumnName, Qt::Horizontal, tr("Name"));
    setHeaderData(PartsSqlQueryModel2::ColumnDescription, Qt::Horizontal, tr("Description"));
    setHeaderData(PartsSqlQueryModel2::ColumnActualStock, Qt::Horizontal, tr("Stock"));
    setHeaderData(PartsSqlQueryModel2::ColumnMinStock, Qt::Horizontal, tr("Min. Stock"));
    setHeaderData(PartsSqlQueryModel2::ColumnAvgPrice, Qt::Horizontal, tr("Avg. Price"));
    setHeaderData(PartsSqlQueryModel2::ColumnCustomPartNumber, Qt::Horizontal, tr("Custom Part#"));
    setHeaderData(PartsSqlQueryModel2::ColumnComment, Qt::Horizontal, tr("Comment"));
    setHeaderData(PartsSqlQueryModel2::ColumnCreateDate, Qt::Horizontal, tr("Create Date"));
    setHeaderData(PartsSqlQueryModel2::ColumnCategoryName, Qt::Horizontal, tr("Category"));
    setHeaderData(PartsSqlQueryModel2::ColumnStorage, Qt::Horizontal, tr("Storage Location"));
    setHeaderData(PartsSqlQueryModel2::ColumnCondition, Qt::Horizontal, tr("Condition"));
    setHeaderData(PartsSqlQueryModel2::ColumnFootprintName, Qt::Horizontal, tr("Footprint"));
    */
}

void PartsSqlQueryModel2::setColumnName(int section, const QString & columnName)
{
    setHeaderData(section, Qt::Horizontal, columnName, Qt::EditRole);
    setHeaderData(section, Qt::Horizontal, true, VISIBILITY_COLUMN_ROLE);
}

QString PartsSqlQueryModel2::selectStatement() const
{
    QString query("SELECT part.id, part.name, part.description, part.actualStock, part.minimumStock, part.averagePrice, part.comment, part.customPartNumber, "
                  "part.createDate, part.partUnit, part.category, part.storage, part.condition, "
                  "part.footprint, u.name AS unitName, c.name AS categoryName, s.name AS storageName, cond.value AS conditionValue, "
                  "footprint.name as footprintName "
                  "FROM part LEFT JOIN part_category c ON part.category=c.id "
                  "LEFT JOIN part_unit u ON part.partUnit=u.id "
                  "LEFT JOIN part_storage s ON part.storage=s.id "
                  "LEFT JOIN part_condition cond ON part.condition=cond.id "
                  "LEFT JOIN part_footprint footprint ON part.footprint=footprint.id ");

    if(!filter().isEmpty()){
        query.append(QLatin1String(" WHERE ")).append(filter());
    }

    QString orderBy = orderByClause();
    if (!orderBy.isEmpty())
        query = query.append(QLatin1Char(' ')).append(orderBy);
    qDebug()<<"Query is "<<query;
    return query;   
}

bool PartsSqlQueryModel2::insertRowIntoTable(const QSqlRecord &values)
{
    QSqlRecord other;
    for(int i=0; i< FAKE_COLUMNS_INDEX;++i){
        other.append(values.field(i));
    }
//    other.setGenerated(0,true);
/*
    qDebug()<<"Table is "<<tableName();
    QString stmnt = database().driver()->sqlStatement(QSqlDriver::InsertStatement, tableName(), other, true);
    qDebug()<<"STAT:"<<stmnt;

    for(int i=0; i< other.count();++i){
        qDebug()<<"field "<<other.field(i).name()<<" has "<<other.value(i);
    }
*/
    bool res = QSqlTableModel::insertRowIntoTable(other);
    _lastInsertedId =  query().lastInsertId();
    if(!res){
        qDebug()<<"Error "<<lastError().text();
    }
    else{        
        qDebug()<<"SUCCESS "<<_lastInsertedId;
    }
    return res;
}

bool PartsSqlQueryModel2::updateRowInTable(int row, const QSqlRecord &values)
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

Qt::ItemFlags PartsSqlQueryModel2::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flags = QSqlTableModel::flags(index);
    flags = flags | Qt::ItemIsDragEnabled;
    return flags;
}

Qt::DropActions PartsSqlQueryModel2::supportedDropActions() const
{
    return Qt::CopyAction | Qt::LinkAction;
}

QStringList PartsSqlQueryModel2::mimeTypes() const
{
    QStringList types;
    types << "myparts/part" << "text/plain" << "text/html";
    return types;
}

QMimeData * PartsSqlQueryModel2::mimeData(const QModelIndexList &indexes) const
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

/*
bool PartsSqlQueryModel2::deleteRowFromTable(int row)
{
    database().transaction();
    QSqlRecord rec = record(row);

    if(QSqlTableModel::deleteRowFromTable(row))
    {

        PartsDAO::cascadeDeletePart
    }
    database().commit();
}
*/


FilterBuilder::FilterBuilder():
    _categoryFilterMode(SubCategories),
    _storageLocationFilterMode(StorageLocationFilterAll),
    _stockFilterMode(AnyStockLevel),
    _dateFilterMode(DateFilterNone)
{
}

void FilterBuilder::setCategoryFilterMode(CategoryFilterMode mode)
{
    _categoryFilterMode = mode;
}

void FilterBuilder::setSelectedCategories(const QVector<QVariant> & selectedCategories)
{
    _selectedCategories = selectedCategories;
}

void FilterBuilder::setStorageLocationFilterMode(StorageLocationFilterMode mode)
{
    _storageLocationFilterMode = mode;
}

void FilterBuilder::setSelectedStorageLocations(const QVector<QVariant> & selectedStorageLocations)
{
    _selectedStorageLocations = selectedStorageLocations;
}

void FilterBuilder::setStockFilterMode(StockFilterMode mode)
{
    _stockFilterMode = mode;
}

void FilterBuilder::setTextFilter(const QString &textFilter)
{
    _textFilter = textFilter;
}

FilterBuilder::CategoryFilterMode FilterBuilder::categoryFilterMode() const
{
    return _categoryFilterMode;
}

FilterBuilder::StorageLocationFilterMode FilterBuilder::storageLocationFilterMode() const
{
    return _storageLocationFilterMode;
}

void FilterBuilder::setDateFilterMode(DateFilterMode dateFilterMode)
{
    _dateFilterMode = dateFilterMode;
}

void FilterBuilder::setSelectedDate(const QDate & date)
{
    QDateTime t(date);
    _selectedDateUtc = t.toUTC();
}

void FilterBuilder::setFilterByConditionEnabled(bool value)
{
    _filterByCondition = value;
}

void FilterBuilder::setFilterByFootprintEnabled(bool b)
{
    _filterByFootprint = b;
}

void FilterBuilder::setSelectedCondition(QVariant conditionId)
{
    _selectedConditionId = conditionId;
}

void FilterBuilder::setSelectedFootprint(QVariant footprintId)
{
    _selectedFootprintId = footprintId;
}

void FilterBuilder::setFilter(SuportedFilters filter, const QVariant & value)
{
    bool valid;
    switch(filter){
    case FilterByStock:
    {
        int v = value.toInt(&valid);
        StockFilterMode mode = valid ? (StockFilterMode)v : AnyStockLevel;
        setStockFilterMode(mode);
    }
        break;
    case FilterByCondition:
        if(value.isValid()){
            setSelectedCondition(value);
        }
        setFilterByConditionEnabled(value.isValid());
        break;
    case FilterByText:
        setTextFilter(value.toString());
        break;
    case FilterByFootprint:
        if(value.isValid()){
            setSelectedFootprint(value);
        }
        setFilterByFootprintEnabled(value.isValid());
        break;
    case FilterByDistributor:
        break;
    }

}


QString FilterBuilder::build() const {
    // Create the criterium to use in the query
    QStringList criterium;   

    if(!_textFilter.isEmpty()){        
        criterium.append(QString("(part.name LIKE '\%%1\%' OR part.description LIKE '\%%1\%')").arg(_textFilter));
    }

    //Category filter section

    qDebug("Category mode is %d",_categoryFilterMode);
    switch(_categoryFilterMode){
    case AllCategories:
        break;
    case SubCategories:
        if(_selectedCategories.size()>0){
            QString inStatement("c.id IN (");
            inStatement+=_selectedCategories.at(0).toString();

            for(int i=1; i<_selectedCategories.size();++i){
                inStatement+=QLatin1String(",");
                inStatement+=_selectedCategories.at(i).toString();
            }
            inStatement+=QLatin1Char(')');
            criterium.append(inStatement);
        }
        break;
    case SelectedCategory:
        if(_selectedCategories.size()>0){
            criterium.append(QString("c.id = %1").arg(_selectedCategories.at(0).toString()));
        }
        break;
    }

    qDebug("Storage location filter mode is %d",_storageLocationFilterMode);
    switch(_storageLocationFilterMode){
    case StorageLocationFilterAll:
        break;
    case StorageLocationFilterSub:
        if(_selectedStorageLocations.size()>0){
            QString inStatement("s.id IN (");
            inStatement+=_selectedStorageLocations.at(0).toString();

            for(int i=1; i<_selectedStorageLocations.size();++i){
                inStatement+=QLatin1String(",");
                inStatement+=_selectedStorageLocations.at(i).toString();
            }
            inStatement+=QLatin1Char(')');
            criterium.append(inStatement);
        }
        break;
    case StorageLocationFilterSelected:
        if(_selectedStorageLocations.size()>0){
            criterium.append(QString("s.id = %1").arg(_selectedStorageLocations.at(0).toString()));
        }
        break;
    }
/*
    if(_filterByStorage && _selectedStorageId.isValid()){
        criterium.append(QString("part.storage = %1").arg(_selectedStorageId.toInt()));
    }
*/
    //Stock filter section

    switch(_stockFilterMode){
    case AnyStockLevel:
        break;
    case StockLevelZero:
        criterium.append(QLatin1String("part.actualStock=0"));
        break;
    case StockLevelGreaterZero:
        criterium.append(QLatin1String("part.actualStock>0"));
        break;
    case StockLevelBelowMin:
        criterium.append(QLatin1String("part.actualStock<part.minimumStock"));
        break;
    }

    //Date filter section


    switch(_dateFilterMode){
        case DateFilterBefore:
    {
            criterium.append(QString("part.createDate<%1").arg(_selectedDateUtc.toTime_t()));
    }
            break;
        case DateFilterOn:
    {
            const QDateTime & periodStart = _selectedDateUtc;
            const QDateTime periodEnd = _selectedDateUtc.addDays(1);
            criterium.append(QString("part.createDate>=%1 && part.createDate<%2").arg(periodStart.toTime_t()).arg(periodEnd.toTime_t()));
    }
        break;
        case DateFilterAfter:
    {
            QDateTime nextDay = _selectedDateUtc.addDays(1);
            criterium.append(QString("part.createDate>=%1").arg(nextDay.toTime_t()));
    }
        break;
    default:
        break;
    }

    //Part condition section

    if(_filterByCondition && _selectedConditionId.isValid()){
        criterium.append(QString("part.condition = %1").arg(_selectedConditionId.toInt()));
    }

    //Part footprint section

    if(_filterByFootprint && _selectedFootprintId.isValid()){
        criterium.append(QString("part.footprint = %1").arg(_selectedFootprintId.toInt()));
    }


    //Join all filter sections
    QString whereClause;
    if(criterium.size()>0){
        whereClause.append(criterium.at(0));
        for(int i=1; i<criterium.size();++i){
            whereClause.append(QLatin1String(" AND ")).append(criterium.at(i));
        }
        whereClause.append(QLatin1Char(' '));
    }
    qDebug()<<"Filter clause is: "<<whereClause;
    return whereClause;
}




