#include "filterbuilder.h"

#include <QStringList>
#include <QDebug>

StockCriterion::StockCriterion(StockCriterion::Mode mode) :
    mode(mode)
{}

void StockCriterion::accept(FilterVisitor *visitor)
{
    visitor->visit(this);
}


DistributorCriterion::DistributorCriterion(QVariant & distributorId) :
    distributorId(distributorId)
{}

void DistributorCriterion::accept(FilterVisitor *visitor)
{
    visitor->visit(this);
}

ManufacturerCriterion::ManufacturerCriterion(QVariant & manufacturerId) :
    manufacturerId(manufacturerId)
{}

void ManufacturerCriterion::accept(FilterVisitor *visitor)
{
    visitor->visit(this);
}

CreateDateCriterion::CreateDateCriterion(CreateDateCriterion::Mode mode, const QDate & dateUtc) :
    mode(mode), dateTimeUtc(dateUtc)
{}

void CreateDateCriterion::accept(FilterVisitor *visitor)
{
    visitor->visit(this);
}

ConditionCriterion::ConditionCriterion(QVariant & conditionId) :
    conditionId(conditionId)
{}

void ConditionCriterion::accept(FilterVisitor *visitor)
{
    visitor->visit(this);
}

FootprintCriterion::FootprintCriterion(const QVariant &footprintId) :
    footprintId(footprintId)
{}

void FootprintCriterion::accept(FilterVisitor *visitor)
{
    visitor->visit(this);
}

TextCriterion::TextCriterion(const QString & text) :
    text(text)
{}

void TextCriterion::accept(FilterVisitor *visitor)
{
    visitor->visit(this);
}

CategoryCriterion::CategoryCriterion(CategoryCriterion::Mode mode, const QVector<QVariant> &selectedCategories) :
    mode(mode), selectedCategories(selectedCategories)
{
}

void CategoryCriterion::accept(FilterVisitor * visitor)
{
    visitor->visit(this);
}

StorageCriterion::StorageCriterion(StorageCriterion::Mode mode, const QVector<QVariant> & selectedStorages) :
    mode(mode), selectedStorages(selectedStorages)
{
}

void StorageCriterion::accept(FilterVisitor * visitor)
{
    visitor->visit(this);
}


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

class CriterionClause {
public:
    explicit CriterionClause(const QString & whereStatement=QString(), const QString & joinStatement=QString());
    QString whereStatement() const;
    void setWhereStatement(const QString & statement);
    QString joinStatement() const;
    bool isEnabled() const;
private:
    QString _whereStatement;
    const QString _joinStatement;
};

CriterionClause::CriterionClause(const QString & whereStatement, const QString & joinStatement)
    : _whereStatement(whereStatement), _joinStatement(joinStatement)
{
}

QString CriterionClause::whereStatement() const
{
    return _whereStatement;
}

void CriterionClause::setWhereStatement(const QString & statement)
{
    _whereStatement = statement;
}

QString CriterionClause::joinStatement() const
{
    return _joinStatement;
}

bool CriterionClause::isEnabled() const
{
    return !_whereStatement.isEmpty();
}


class QueryBuilder : FilterVisitor
{
    const QString _baseSelectClause;
    CriterionClause _stockClause;
    CriterionClause _distributorClause;
    CriterionClause _manufacturerClause;
    CriterionClause _createDateClause;
    CriterionClause _conditionClause;
    CriterionClause _footprintClause;
    CriterionClause _textClause;
    CriterionClause _categoryClause;
    CriterionClause _storageClause;
    QList<CriterionClause*> _criterions;
public:       
    QueryBuilder() :
        _baseSelectClause(BASE_CLAUSE),
        _distributorClause(CriterionClause(QString(), DISTRIBUTOR_JOIN_CLAUSE)),
        _manufacturerClause(CriterionClause(QString(), MANUFACTURER_JOIN_CLAUSE))
    {
        _criterions.append(&_stockClause);
        _criterions.append(&_distributorClause);
        _criterions.append(&_manufacturerClause);
        _criterions.append(&_createDateClause);
        _criterions.append(&_conditionClause);
        _criterions.append(&_footprintClause);
        _criterions.append(&_textClause);
        _criterions.append(&_categoryClause);
        _criterions.append(&_storageClause);
    }

    void addCriterion(FilterCriterion & criterion)
    {
        criterion.accept(this);
    }

    void visit(StockCriterion * criterion)
    {
        switch(criterion->mode)
        {
        case StockCriterion::AnyStockLevel:
            _stockClause.setWhereStatement(QString());
            break;
        case StockCriterion::StockLevelZero:
            _stockClause.setWhereStatement(QLatin1String("part.actualStock=0"));
            break;
        case StockCriterion::StockLevelGreaterZero:
            _stockClause.setWhereStatement(QLatin1String("part.actualStock>0"));
            break;
        case StockCriterion::StockLevelBelowMin:
            _stockClause.setWhereStatement(QLatin1String("part.actualStock<0"));
            break;
        }
    }

    void visit(DistributorCriterion * criterion)
    {
        if(criterion->distributorId.isValid() && criterion->distributorId.canConvert(QVariant::Int)){
            _distributorClause.setWhereStatement(QString("d.distributor = %1").arg(criterion->distributorId.toInt()));
        }
        else{
            _distributorClause.setWhereStatement(QString());
        }
    }

    void visit(ManufacturerCriterion * criterion)
    {
        if(criterion->manufacturerId.isValid() && criterion->manufacturerId.canConvert(QVariant::Int)){
            _manufacturerClause.setWhereStatement(QString("m.manufacturer = %1").arg(criterion->manufacturerId.toInt()));
        }
        else{
            _manufacturerClause.setWhereStatement(QString());
        }
    }

    void visit(CreateDateCriterion * criterion)
    {
        switch(criterion->mode){
        case CreateDateCriterion::DateFilterBefore:
        {
            _createDateClause.setWhereStatement(QString("part.createDate<%1").arg(criterion->dateTimeUtc.toTime_t()));
        }
            break;
        case CreateDateCriterion::DateFilterOn:
        {
            const QDateTime & periodStart = criterion->dateTimeUtc;
            const QDateTime periodEnd = criterion->dateTimeUtc.addDays(1);
            _createDateClause.setWhereStatement(QString("part.createDate>=%1 && part.createDate<%2").arg(periodStart.toTime_t()).arg(periodEnd.toTime_t()));
        }
            break;
        case CreateDateCriterion::DateFilterAfter:
        {
            QDateTime nextDay = criterion->dateTimeUtc.addDays(1);
            _createDateClause.setWhereStatement(QString("part.createDate>=%1").arg(nextDay.toTime_t()));
        }
            break;
        default:
            _createDateClause.setWhereStatement(QString());
            break;
        }
    }

    void visit(ConditionCriterion * criterion)
    {
        if(criterion->conditionId.isValid() && criterion->conditionId.canConvert(QVariant::Int)){
            _conditionClause.setWhereStatement(QString("part.condition = %1").arg(criterion->conditionId.toInt()));
        }
        else{
            _conditionClause.setWhereStatement(QString());
        }
    }

    void visit(FootprintCriterion * criterion)
    {
        if(criterion->footprintId.isValid() && criterion->footprintId.canConvert(QVariant::Int)){
            _footprintClause.setWhereStatement(QString("part.footprint = %1").arg(criterion->footprintId.toInt()));
        }
        else{
            _footprintClause.setWhereStatement(QString());
        }
    }

    void visit(TextCriterion * criterion)
    {
        if(!criterion->text.isEmpty()){
            _textClause.setWhereStatement(QString("(part.name LIKE '\%%1\%' OR part.description LIKE '\%%1\%')").arg(criterion->text));
        }
        else{
            _textClause.setWhereStatement(QString());
        }
    }

    void visit(CategoryCriterion * criterion)
    {
        QString statement;
        const QVector<QVariant> & selectCategories = criterion->selectedCategories;

        switch(criterion->mode){
        case CategoryCriterion::Disabled:
            break;
        case CategoryCriterion::SelectedAndChilds:
            if(selectCategories.size()>0){
                QString inStatement("c.id IN (");
                inStatement+=selectCategories.at(0).toString();

                for(int i=1; i<selectCategories.size();++i){
                    inStatement+=QLatin1String(",");
                    inStatement+=selectCategories.at(i).toString();
                }
                inStatement+=QLatin1Char(')');
                statement = inStatement;
            }
            break;
        case CategoryCriterion::SelectedOnly:
            if(criterion->selectedCategories.size()>0){
                statement = QString("c.id = %1").arg(selectCategories.at(0).toString());
            }
            break;
        }
        _categoryClause.setWhereStatement(statement);
    }

    void visit(StorageCriterion * criterion)
    {
        QString statement;
        const QVector<QVariant> & selectStorage = criterion->selectedStorages;

        switch(criterion->mode){
        case StorageCriterion::Disabled:
            break;
        case StorageCriterion::SelectedAndChilds:
            if(selectStorage.size()>0){
                QString inStatement("s.id IN (");
                inStatement+=selectStorage.at(0).toString();

                for(int i=1; i<selectStorage.size();++i){
                    inStatement+=QLatin1String(",");
                    inStatement+=selectStorage.at(i).toString();
                }
                inStatement+=QLatin1Char(')');
                statement = inStatement;
            }
            break;
        case StorageCriterion::SelectedOnly:
            if(selectStorage.size()>0){
                statement = QString("s.id = %1").arg(selectStorage.at(0).toString());
            }
            break;
        }
        _storageClause.setWhereStatement(statement);
    }

    QString generateSelectStatement()
    {
        QStringList clauses;
        //Keeps the criterium used for the WHERE clause
        QStringList criterium;

        clauses.append(_baseSelectClause);

        CriterionClause* criterion;
        foreach (criterion, _criterions) {
            if(criterion->isEnabled()){
                criterium.append(criterion->whereStatement());
                if(!criterion->joinStatement().isEmpty())
                    clauses.append(criterion->joinStatement());
            }
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
};
