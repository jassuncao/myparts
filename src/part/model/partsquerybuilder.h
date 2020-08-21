#ifndef PARTSQUERYBUILDER_H
#define PARTSQUERYBUILDER_H

#include <QVariant>
#include <QHash>
#include <QStringList>
#include <QDateTime>
#include <QMetaType>
#include <QDebug>


class Criterion;

class DateCriterionValue
{
public:
    enum Mode {
        DateFilterNone,
        DateFilterBefore,
        DateFilterOn,
        DateFilterAfter
    };
    DateCriterionValue();
    DateCriterionValue(Mode mode, QDateTime dateTime);
    DateCriterionValue(const DateCriterionValue &other);
    ~DateCriterionValue();

    Mode mode() const { return _mode;}
    QDateTime date() const { return _dateTimeUtc;}
private:
    Mode _mode;
    QDateTime _dateTimeUtc;
};

class NodeCriterionValue
{
public:
    enum Mode {
        All=1,
        IncludeNodeChilds=2,
        SelectedNode=3
    };
    NodeCriterionValue();
    NodeCriterionValue(Mode mode, QList<int> selectedNodes);
    NodeCriterionValue(const NodeCriterionValue &other);
    ~NodeCriterionValue();
    Mode mode() const { return _mode; }
    QList<int> selectedNodes() const { return _selectedNodes; }
private:
    Mode _mode;
    QList<int> _selectedNodes;
};

QDebug operator<<(QDebug dbg, const DateCriterionValue &value);

Q_DECLARE_METATYPE(DateCriterionValue)
Q_DECLARE_METATYPE(NodeCriterionValue)

class PartsQueryBuilder
{
public:
    enum SupportedFilters{
        FilterByStock,
        FilterByCondition,
        FilterByDistributor,
        FilterByManufacturer,
        FilterByPackage,
        FilterByText,
        FilterByCategory,
        FilterByStorage
    };

    enum StockFilterMode {
        AnyStockLevel,
        StockLevelZero,
        StockLevelGreaterZero,
        StockLevelBelowMin
    };

    enum DateFilterMode {
        DateFilterNone,
        DateFilterBefore,
        DateFilterOn,
        DateFilterAfter
    };

    explicit PartsQueryBuilder();
    ~PartsQueryBuilder();

    void setFilter(SupportedFilters type, const QVariant & value);    
    void setSort(int column, Qt::SortOrder order);
    QString buildQuery() const;
private:    
    QString orderByClause() const;
    const QLatin1String _baseSelectClause;
    QStringList _columnNames;
    QList<Criterion*> _criterions;
    QHash<int,Criterion*> _criterionsMap;
    bool _criterionChanged;
    int _sortColumn;
    Qt::SortOrder _sortOrder;
};

#endif // PARTSQUERYBUILDER_H
