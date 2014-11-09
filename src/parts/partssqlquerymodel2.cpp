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
}

QString PartsSqlQueryModel2::selectStatement() const
{
    QString query("SELECT part.id, part.name, part.description, part.actualStock, part.minimumStock, part.averagePrice, part.comment, part.customPartNumber, "
                  "part.createDate, part.partUnit, part.category, part.storage, part.condition, u.name AS unitName, c.name AS categoryName, s.name AS storageName, cond.value AS conditionValue "
                  "FROM part LEFT JOIN part_category c ON part.category=c.id "
                  "LEFT JOIN part_unit u ON part.partUnit=u.id "
                  "LEFT JOIN part_storage s ON part.storage=s.id "
                  "LEFT JOIN part_condition cond ON part.condition=cond.id ");    

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
    _categoryFilteringMode(SubCategories),
    _stockFilteringMode(AnyStockLevel),
    _dateFilterMode(DateFilterNone)
{
}

void FilterBuilder::setCategoryFilteringMode(CategoryFilteringMode mode)
{
    _categoryFilteringMode = mode;
}

void FilterBuilder::setSelectedCategories(const QVector<QVariant> & selectedCategories)
{
    _selectedCategories = selectedCategories;
}

void FilterBuilder::setFilterByStorage(bool value)
{
    _filterByStorage = value;
}

void FilterBuilder::setSelectedStorage(QVariant storageId)
{
    _selectedStorageId = storageId;
}

void FilterBuilder::setStockFilteringMode(StockFilteringMode mode)
{
    _stockFilteringMode = mode;
}

void FilterBuilder::setTextFilter(const QString &textFilter)
{
    _textFilter = textFilter;
}

FilterBuilder::CategoryFilteringMode FilterBuilder::categoryFilteringMode() const
{
    return _categoryFilteringMode;
}

void FilterBuilder::setDateFilterMode(DateFilteringMode dateFilterMode)
{
    _dateFilterMode = dateFilterMode;
}

void FilterBuilder::setSelectedDate(const QDate & date)
{
    QDateTime t(date);
    _selectedDateUtc = t.toUTC();
}

void FilterBuilder::setFilterByCondition(bool value)
{
    _filterByCondition = value;
}

void FilterBuilder::setSelectedCondition(QVariant conditionId)
{
    _selectedConditionId = conditionId;
}


QString FilterBuilder::build() const {
    // Create the criterium to use in the query
    QStringList criterium;   

    if(!_textFilter.isEmpty()){        
        criterium.append(QString("(part.name LIKE '\%%1\%' OR part.description LIKE '\%%1\%')").arg(_textFilter));
    }

    //Category filter section

    qDebug("Category mode is %d",_categoryFilteringMode);
    switch(_categoryFilteringMode){
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

    if(_filterByStorage && _selectedStorageId.isValid()){
        criterium.append(QString("part.storage = %1").arg(_selectedStorageId.toInt()));
    }

    //Stock filter section

    switch(_stockFilteringMode){
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



