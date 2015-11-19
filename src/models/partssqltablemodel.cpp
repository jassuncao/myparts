#include "partssqltablemodel.h"
#include <QDebug>
#include <QSqlRecord>
#include <QSqlField>
#include <QSqlDriver>
#include <QSqlError>
#include <QStringBuilder>
#include <QMimeData>
#include <QSqlResult>
#include <QSqlQuery>

static const char * BASE_CLAUSE = "SELECT part.id, part.name, part.description, part.actualStock, part.minimumStock, part.averagePrice, part.comment, part.customPartNumber, "
"part.createDate, part.partUnit, part.category, part.storage, part.condition, "
"part.footprint, u.name AS unitName, c.name AS categoryName, s.name AS storageName, cond.value AS conditionValue, "
"footprint.name as footprintName "
"FROM part LEFT JOIN part_category c ON part.category=c.id "
"LEFT JOIN part_unit u ON part.partUnit=u.id "
"LEFT JOIN part_storage s ON part.storage=s.id "
"LEFT JOIN part_condition cond ON part.condition=cond.id "
"LEFT JOIN part_footprint footprint ON part.footprint=footprint.id ";

static const char * DISTRIBUTOR_JOIN_CLAUSE = "INNER JOIN part_distributor d ON part.id=d.part ";
static const char * MANUFACTURER_JOIN_CLAUSE = "INNER JOIN part_manufacturer m ON part.id=m.part ";

QString Criterion::joinClause() const
{
    return QString();
}

StockCriterion::StockCriterion(StockCriterion::Mode mode) :
    _mode(mode)
{
}

QString StockCriterion::clause() const
{
    switch(_mode){
    case AnyStockLevel:
        return QString();
    case StockLevelZero:
        return QLatin1String("part.actualStock=0");
    case StockLevelGreaterZero:
        return QLatin1String("part.actualStock>0");
    case StockLevelBelowMin:
        return QLatin1String("part.actualStock<part.minimumStock");
    }
    return QString();
}

BasicForeignKeyCriterion::BasicForeignKeyCriterion(const QString & name, const QVariant & value) :
    _foreignKeyName(name), _foreignKeyValue(value)
{}

QString BasicForeignKeyCriterion::clause() const
{
    if(_foreignKeyValue.isValid() && _foreignKeyValue.canConvert(QVariant::Int)){
        return QString("part.%1 = %2").arg(_foreignKeyName).arg(_foreignKeyValue.toInt());
    }
    return QString();
}

DistributorCriterion::DistributorCriterion(QVariant & distributorId) :
    _distributorId(distributorId),
    _joinClause(DISTRIBUTOR_JOIN_CLAUSE)
{
}

QString DistributorCriterion::clause() const
{
    if(_distributorId.isValid() && _distributorId.canConvert(QVariant::Int)){
        return QString("d.distributor = %1").arg(_distributorId.toInt());
    }
    return QString();
}

QString DistributorCriterion::joinClause() const
{
    if(_distributorId.isValid() && _distributorId.canConvert(QVariant::Int)){
        return _joinClause;
    }
    return QString();
}

ManufacturerCriterion::ManufacturerCriterion(QVariant & manufacturerId) :
    _manufacturerId(manufacturerId),
    _joinClause(MANUFACTURER_JOIN_CLAUSE)
{
}

QString ManufacturerCriterion::clause() const
{
    if(_manufacturerId.isValid() && _manufacturerId.canConvert(QVariant::Int)){
        return QString("m.manufacturer = %1").arg(_manufacturerId.toInt());
    }
    return QString();
}

QString ManufacturerCriterion::joinClause() const
{
    if(_manufacturerId.isValid() && _manufacturerId.canConvert(QVariant::Int)){
        return _joinClause;
    }
    return QString();
}


CreateDateCriterion::CreateDateCriterion(CreateDateCriterion::Mode mode, const QDate &dateUtc) :
    _mode(mode),
    _dateTimeUtc(dateUtc)
{
}


QString CreateDateCriterion::clause() const
{
    if(!_dateTimeUtc.isValid())
        return QString();

    switch(_mode){
        case DateFilterNone:
            return QString();
        case DateFilterBefore:
            return QString("part.createDate<%1").arg(_dateTimeUtc.toTime_t());
        case DateFilterOn:
    {
            const QDateTime & periodStart = _dateTimeUtc;
            const QDateTime periodEnd = _dateTimeUtc.addDays(1);
            return QString("part.createDate>=%1 && part.createDate<%2").arg(periodStart.toTime_t()).arg(periodEnd.toTime_t());
    }
        break;
    case DateFilterAfter:
    {
            QDateTime nextDay = _dateTimeUtc.addDays(1);
            return QString("part.createDate>=%1").arg(nextDay.toTime_t());
    }
        break;
    }

    return QString();
}


PartsSqlTableModel::PartsSqlTableModel(QObject *parent) :
    QSqlTableModel(parent),
    _baseSelectClause(BASE_CLAUSE),
    _distributorJoinClause(DISTRIBUTOR_JOIN_CLAUSE),
    _manufacturerJoinClause(MANUFACTURER_JOIN_CLAUSE),
    _categoryFilterMode(SubCategories),
    _storageLocationFilterMode(StorageLocationFilterAll),
    _stockFilterMode(AnyStockLevel),
    _dateFilterMode(DateFilterNone)
{
    setTable("part");
    qDebug()<<"Column Count "<<columnCount();
    setColumnName(ColumnName,tr("Name"));
    setColumnName(ColumnDescription,tr("Description"));
    setColumnName(ColumnActualStock,tr("Stock"));
    setColumnName(ColumnMinStock,tr("Min. Stock"));
    setColumnName(ColumnAvgPrice,tr("Avg. Price"));
    setColumnName(ColumnCustomPartNumber,tr("Custom Part#"));
    setColumnName(ColumnComment,tr("Comment"));
    setColumnName(ColumnCreateDate,tr("Create Date"));
    setColumnName(ColumnCategoryName,tr("Category"));
    setColumnName(ColumnStorage,tr("Storage Location"));
    setColumnName(ColumnCondition,tr("Condition"));
    setColumnName(ColumnFootprintName,tr("Footprint"));
}

PartsSqlTableModel::~PartsSqlTableModel()
{
    qDeleteAll(_criterions);
}

void PartsSqlTableModel::setColumnName(int section, const QString & columnName)
{
    setHeaderData(section, Qt::Horizontal, columnName, Qt::EditRole);
    setHeaderData(section, Qt::Horizontal, true, VISIBILITY_COLUMN_ROLE);
}

int PartsSqlTableModel::columnCount(const QModelIndex &index) const
{
    return index.isValid() ? 0 : LAST_COLUMN;
}

QString PartsSqlTableModel::selectStatement2() const
{

}

QString PartsSqlTableModel::selectStatement() const
{
    QStringList clauses;
    //Keeps the criterium used for the WHERE clause
    QStringList criterium;

    clauses.append(_baseSelectClause);

    //Distributor filter section
    if(_selectedDistributorId.isValid() && _selectedDistributorId.canConvert(QVariant::Int)){
        clauses.append(_distributorJoinClause);
        criterium.append(QString("d.distributor = %1").arg(_selectedDistributorId.toInt()));
    }

    //Manufacturer filter section
    if(_selectedManufacturerId.isValid() && _selectedManufacturerId.canConvert(QVariant::Int)){
        clauses.append(_manufacturerJoinClause);
        criterium.append(QString("m.manufacturer = %1").arg(_selectedManufacturerId.toInt()));
    }

    //Text filter section
    if(_textFilter.isValid() && _textFilter.canConvert(QVariant::String)){
        criterium.append(QString("(part.name LIKE '\%%1\%' OR part.description LIKE '\%%1\%')").arg(_textFilter.toString()));
    }

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

    if(_selectedConditionId.isValid() && _selectedConditionId.canConvert(QVariant::Int)){
        criterium.append(QString("part.condition = %1").arg(_selectedConditionId.toInt()));
    }

    //Part footprint section

    if(_selectedFootprintId.isValid() && _selectedFootprintId.canConvert(QVariant::Int)){
        criterium.append(QString("part.footprint = %1").arg(_selectedFootprintId.toInt()));
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

    //Join the criterium as an where clause
    if(criterium.size()>0){
        clauses.append(QLatin1String("WHERE"));
        clauses.append(criterium.join(" AND "));
    }

    QString orderBy = orderByClause();
    if (!orderBy.isEmpty())
        clauses.append(orderBy);

    const QString statement = clauses.join(QLatin1String(" "));

    qDebug()<<"Select statement is "<<statement;
    return statement;
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
    types << "myparts/part" << "text/plain" << "text/html";
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

void PartsSqlTableModel::setFilter(SuportedFilters filter, const QVariant & value)
{
    bool valid;
    switch(filter){
    case FilterByStock:
    {
        int v = value.toInt(&valid);
        _stockFilterMode = valid ? (StockFilterMode)v : AnyStockLevel;
    }
        break;
    case FilterByCondition:
        _selectedConditionId = value;
        break;
    case FilterByText:
        _textFilter = value;
        break;
    case FilterByFootprint:
        _selectedFootprintId = value;
        break;
    case FilterByDistributor:
        _selectedDistributorId = value;
        break;
    case FilterByManufacturer:
        _selectedManufacturerId = value;
        break;
    }
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
    const QModelIndex & colIndex = index(row, ColumnActualStock);
    QVariant value = data(colIndex, Qt::EditRole);
    int currentStock = value.toInt();
    currentStock+=stockChange;
    return setData(colIndex, currentStock, Qt::EditRole);
}
