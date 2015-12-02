#include "partsquerybuilder.h"

#include <QStringList>
#include <QStringBuilder>

static const char * BASE_CLAUSE = "SELECT part.id, part.name, part.description, part.actualStock, part.minimumStock, part.averagePrice, part.comment, part.customPartNumber, "
"part.createDate, part.partUnit, part.category, part.storage, part.condition, "
"part.package, u.name AS unitName, c.name AS categoryName, s.name AS storageName, cond.value AS conditionValue, "
"package.name as packageName "
"FROM part LEFT JOIN category c ON part.category=c.id "
"LEFT JOIN part_unit u ON part.partUnit=u.id "
"LEFT JOIN storage s ON part.storage=s.id "
"LEFT JOIN condition cond ON part.condition=cond.id "
"LEFT JOIN package package ON part.package=package.id ";

static const char * DISTRIBUTOR_JOIN_CLAUSE = "INNER JOIN distributor d ON part.id=d.part ";
static const char * MANUFACTURER_JOIN_CLAUSE = "INNER JOIN manufacturer m ON part.id=m.part ";

DateCriterionValue::DateCriterionValue() :
    _mode(DateFilterNone), _dateTimeUtc(QDateTime())
{

}

DateCriterionValue::DateCriterionValue(Mode mode, QDateTime dateTime)
    : _mode(mode), _dateTimeUtc(dateTime)
{
}

DateCriterionValue::DateCriterionValue(const DateCriterionValue &other) :
    _mode(other._mode), _dateTimeUtc(other._dateTimeUtc)
{
}

DateCriterionValue::~DateCriterionValue()
{
}

QDebug operator<<(QDebug dbg, const DateCriterionValue &value)
{
    dbg.nospace()<<"DateCriterionValue(mode="<<value.mode()<<", date="<<value.date()<<")";
    return dbg.maybeSpace();
}

NodeCriterionValue::NodeCriterionValue() :
    _mode(All), _selectedNodes(QList<int>())
{}

NodeCriterionValue::NodeCriterionValue(Mode mode, QList<int> selectedNodes) :
    _mode(mode), _selectedNodes(selectedNodes)
{}

NodeCriterionValue::NodeCriterionValue(const NodeCriterionValue &other) :
    _mode(other._mode), _selectedNodes(other._selectedNodes)
{}

NodeCriterionValue::~NodeCriterionValue()
{}


class Criterion
{
public:
    virtual QString clause() const = 0;
    virtual QString joinClause() const;
    virtual void setValue(const QVariant & value) = 0;
};

QString Criterion::joinClause() const
{
    return QString();
}

class StockCriterion : public Criterion
{
public:
    explicit StockCriterion();
    QString clause() const;
    void setValue(const QVariant & value);
private:
    PartsQueryBuilder::StockFilterMode _mode;
};

StockCriterion::StockCriterion()
    : _mode(PartsQueryBuilder::AnyStockLevel)
{}

void StockCriterion::setValue(const QVariant &value)
{
    if(value.isValid() && value.canConvert(QVariant::Int))
    {
        _mode = (PartsQueryBuilder::StockFilterMode) value.toInt();
    }
    else
    {
        _mode = PartsQueryBuilder::AnyStockLevel;
    }
}

QString StockCriterion::clause() const
{
    switch(_mode){
        case PartsQueryBuilder::AnyStockLevel:
            return QString();
        case PartsQueryBuilder::StockLevelZero:
            return QLatin1String("part.actualStock=0");
        case PartsQueryBuilder::StockLevelGreaterZero:
            return QLatin1String("part.actualStock>0");
        case PartsQueryBuilder::StockLevelBelowMin:
            return QLatin1String("part.actualStock<part.minimumStock");
    }
    return QString();
}

class BasicForeignKeyCriterion : public Criterion
{
public:
    explicit BasicForeignKeyCriterion(const QString & foreignKeyName);
    QString clause() const;
    void setValue(const QVariant & value);
private:
    const QString _foreignKeyName;
    QVariant _foreignKeyValue;
};

BasicForeignKeyCriterion::BasicForeignKeyCriterion(const QString & foreignKeyName) :
    _foreignKeyName(foreignKeyName), _foreignKeyValue(QVariant())
{}

void BasicForeignKeyCriterion::setValue(const QVariant & value)
{
    _foreignKeyValue = value;
}

QString BasicForeignKeyCriterion::clause() const
{
    if(_foreignKeyValue.isValid() && _foreignKeyValue.canConvert(QVariant::Int)){
        return QString("part.%1 = %2").arg(_foreignKeyName).arg(_foreignKeyValue.toInt());
    }
    return QString();
}

class DistributorCriterion : public Criterion
{
public:
    explicit DistributorCriterion();
    QString clause() const;
    QString joinClause() const;
    void setValue(const QVariant & value);
private:
    const QLatin1String _joinClause;
    QVariant _distributorId;
};

DistributorCriterion::DistributorCriterion() :
    _joinClause(DISTRIBUTOR_JOIN_CLAUSE),
    _distributorId(QVariant())
{
}

void DistributorCriterion::setValue(const QVariant & distributorId)
{
    _distributorId = distributorId;
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

class ManufacturerCriterion : public Criterion
{
public:
    explicit ManufacturerCriterion();
    QString clause() const;
    QString joinClause() const;
    void setValue(const QVariant & value);
private:
    const QLatin1String _joinClause;
    QVariant _manufacturerId;
};

ManufacturerCriterion::ManufacturerCriterion() :
    _joinClause(MANUFACTURER_JOIN_CLAUSE),
    _manufacturerId(QVariant())
{}

void ManufacturerCriterion::setValue(const QVariant & value)
{
    _manufacturerId = value;
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

class CreateDateCriterion : public Criterion
{
public:
    explicit CreateDateCriterion();
    QString clause() const;
    void setValue(const QVariant & value);
private:
    DateCriterionValue _value;    
};

CreateDateCriterion::CreateDateCriterion() :
    _value(DateCriterionValue())
{
}

QString CreateDateCriterion::clause() const
{
    switch(_value.mode()){
    case DateCriterionValue::DateFilterBefore:
    {
        return QString("part.createDate<%1").arg(_value.date().toTime_t());
    }
        break;
    case DateCriterionValue::DateFilterOn:
    {
        const QDateTime & periodStart = _value.date();
        const QDateTime periodEnd = _value.date().addDays(1);
        return QString("part.createDate>=%1 && part.createDate<%2").arg(periodStart.toTime_t()).arg(periodEnd.toTime_t());
    }
        break;
    case DateCriterionValue::DateFilterAfter:
    {
        QDateTime nextDay = _value.date().addDays(1);
        return QString("part.createDate>=%1").arg(nextDay.toTime_t());
    }
        break;
    default:
        return QString();
        break;
    }
}

void CreateDateCriterion::setValue(const QVariant & value)
{
    if(value.isValid() && value.canConvert<DateCriterionValue>())
        _value = value.value<DateCriterionValue>();
    else
        _value = DateCriterionValue();
}

class TextCriterion : public Criterion
{
public:
    explicit TextCriterion();
    QString clause() const;
    void setValue(const QVariant & value);
private:
    QString _text;
};

TextCriterion::TextCriterion()
{}

void TextCriterion::setValue(const QVariant & value)
{
    if(value.isValid() && value.canConvert(QVariant::String)){
        _text = value.toString();
    }
    else{
        _text = QString();
    }
}

QString TextCriterion::clause() const
{
    if(!_text.isEmpty()){
        return QString("(part.name LIKE '\%%1\%' OR part.description LIKE '\%%1\%')").arg(_text);
    }
    return QString();
}

class NodeCriterion : public Criterion
{
public:
    explicit NodeCriterion(const QString & foreignKeyName);
    QString clause() const;
    void setValue(const QVariant & value);
private:
    const QString _foreignKeyName;
    NodeCriterionValue _value;
};

NodeCriterion::NodeCriterion(const QString & foreignKeyName) :
    _foreignKeyName(foreignKeyName), _value(NodeCriterionValue())
{}

void NodeCriterion::setValue(const QVariant & value)
{
    if(value.isValid() && value.canConvert<NodeCriterionValue>())
    {
        _value = value.value<NodeCriterionValue>();
    }
    else{
        _value = NodeCriterionValue();
    }
}

QString NodeCriterion::clause() const
{
    const QList<int> selectedIds = _value.selectedNodes();
    switch(_value.mode()){
    case NodeCriterionValue::All:
        break;

    case NodeCriterionValue::IncludeNodeChilds:
        if(selectedIds.size()>0){
            QString inStatement = QString("%1 IN (%2").arg(_foreignKeyName).arg(selectedIds.at(0));

            for(int i=1; i<selectedIds.size();++i){
                inStatement+=QString(", %1").arg(selectedIds.at(i));
            }
            inStatement+=QLatin1Char(')');
            return inStatement;
        }
        break;
    case NodeCriterionValue::SelectedNode:
        if(selectedIds.size()>0){
            return QString("%1 = %2").arg(_foreignKeyName).arg(selectedIds.at(0));
        }
        break;
    }
    return QString();
}

PartsQueryBuilder::PartsQueryBuilder() :
    _baseSelectClause(BASE_CLAUSE),
    _criterionChanged(true),
    _sortColumn(0),
    _sortOrder(Qt::AscendingOrder)
{
    _columnNames.append(QLatin1String("part.id"));
    _columnNames.append(QLatin1String("part.name"));
    _columnNames.append(QLatin1String("part.description"));
    _columnNames.append(QLatin1String("part.actualStock"));
    _columnNames.append(QLatin1String("part.minimumStock"));
    _columnNames.append(QLatin1String("part.averagePrice"));
    _columnNames.append(QLatin1String("part.comment"));
    _columnNames.append(QLatin1String("part.customPartNumber"));
    _columnNames.append(QLatin1String("part.createDate"));
    _columnNames.append(QLatin1String("part.partUnit"));
    _columnNames.append(QLatin1String("part.category"));
    _columnNames.append(QLatin1String("part.storage"));
    _columnNames.append(QLatin1String("part.condition"));
    _columnNames.append(QLatin1String("part.package"));
    _columnNames.append(QLatin1String("u.name"));
    _columnNames.append(QLatin1String("c.name"));
    _columnNames.append(QLatin1String("s.name"));
    _columnNames.append(QLatin1String("cond.value"));
    _columnNames.append(QLatin1String("package.name"));

    _criterions[FilterByStock] = new StockCriterion();
    _criterions[FilterByCondition] = new BasicForeignKeyCriterion("condition");
    _criterions[FilterByDistributor] = new DistributorCriterion();
    _criterions[FilterByManufacturer] = new ManufacturerCriterion();
    _criterions[FilterByPackage] = new BasicForeignKeyCriterion("package");
    _criterions[FilterByText] = new TextCriterion();
    _criterions[FilterByCategory] = new NodeCriterion("part.category");
    _criterions[FilterByStorage] = new NodeCriterion("part.storage");

}

PartsQueryBuilder::~PartsQueryBuilder()
{
    qDeleteAll(_criterions);
}

void PartsQueryBuilder::setFilter(SupportedFilters type, const QVariant & value)
{
    Criterion* criterion = _criterions.value((int)type, 0);
    if(criterion){
        criterion->setValue(value);
        _criterionChanged = true;
    }
    else{
        qWarning()<<"No criterion available for type "<<type;
    }
}

void PartsQueryBuilder::setSort(int column, Qt::SortOrder order)
{
    _sortColumn = column;
    _sortOrder = order;
}

QString PartsQueryBuilder::orderByClause() const
{
    QString s;
    if(_sortColumn>=0 && _sortColumn<_columnNames.size()){
        QString dir = _sortOrder== Qt::AscendingOrder ? QLatin1String(" ASC") : QLatin1String(" DESC");
        s.append(QLatin1String("ORDER BY ")).append(_columnNames.at(_sortColumn)).append(dir);
    }
    return s;
}

QString PartsQueryBuilder::buildQuery() const
{
    QStringList clauses;
    //Keeps the criterium used for the WHERE clause
    QStringList criteriumClauses;

    clauses.append(_baseSelectClause);

    Criterion* criterion;
    foreach (criterion, _criterions) {
        const QString & whereClause = criterion->clause();
        if(!whereClause.isEmpty()){
            criteriumClauses.append(whereClause);
            const QString & joinClause = criterion->joinClause();
            if(!joinClause.isEmpty())
                clauses.append(joinClause);
        }
    }

    //Join the criterium as an where clause
    if(criteriumClauses.size()>0){
        clauses.append(QLatin1String("WHERE"));
        clauses.append(criteriumClauses.join(" AND "));
    }

    const QString & orderBy = orderByClause();
    if (!orderBy.isEmpty())
        clauses.append(orderBy);

    const QString statement = clauses.join(QLatin1String(" "));

    qDebug()<<"Select statement is "<<statement;
    return statement;
}

