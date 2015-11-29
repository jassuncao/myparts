#ifndef FILTERBUILDER_H
#define FILTERBUILDER_H

/*
#include <QDateTime>
#include <QVariant>
#include <QVector>

class FilterVisitor;

struct FilterCriterion {
    virtual void accept(FilterVisitor * visitor)=0;
};

struct StockCriterion : public FilterCriterion
{
    enum Mode {
        AnyStockLevel,
        StockLevelZero,
        StockLevelGreaterZero,
        StockLevelBelowMin
    };
    explicit StockCriterion(StockCriterion::Mode mode);
    void accept(FilterVisitor * visitor);
    const StockCriterion::Mode mode;
};

struct DistributorCriterion : public FilterCriterion
{
    explicit DistributorCriterion(QVariant & distributorId);
    void accept(FilterVisitor * visitor);
    const QVariant distributorId;
};

struct ManufacturerCriterion : public FilterCriterion
{
public:
    explicit ManufacturerCriterion(QVariant & manufacturerId);
    void accept(FilterVisitor * visitor);
    const QVariant manufacturerId;
};

struct CreateDateCriterion : public FilterCriterion
{
    enum Mode {
        DateFilterNone,
        DateFilterBefore,
        DateFilterOn,
        DateFilterAfter
    };
    explicit CreateDateCriterion(CreateDateCriterion::Mode mode, const QDate & dateUtc);
    void accept(FilterVisitor * visitor);
    const CreateDateCriterion::Mode mode;
    const QDateTime dateTimeUtc;
};

struct ConditionCriterion : public FilterCriterion
{
    explicit ConditionCriterion(QVariant & conditionId);
    void accept(FilterVisitor * visitor);
    const QVariant conditionId;
};

struct FootprintCriterion : public FilterCriterion
{
    explicit FootprintCriterion(const QVariant & footprintId);
    void accept(FilterVisitor * visitor);
    const QVariant footprintId;
};

struct TextCriterion : public FilterCriterion
{
    explicit TextCriterion(const QString & text);
    void accept(FilterVisitor * visitor);
    const QString text;
};

struct CategoryCriterion : public FilterCriterion
{
    enum Mode {
        Disabled=1,
        SelectedAndChilds=2,
        SelectedOnly=3
    };

    explicit CategoryCriterion(CategoryCriterion::Mode mode, const QVector<QVariant> & selectedCategories);
    void accept(FilterVisitor * visitor);
    const CategoryCriterion::Mode mode;
    const QVector<QVariant> selectedCategories;
};

struct StorageCriterion : public FilterCriterion
{
    enum Mode {
        Disabled=1,
        SelectedAndChilds=2,
        SelectedOnly=3
    };
    explicit StorageCriterion(StorageCriterion::Mode mode, const QVector<QVariant> & selectedStorages);
    void accept(FilterVisitor * visitor);
    const StorageCriterion::Mode mode;
    const QVector<QVariant> selectedStorages;
};

class FilterVisitor
{
public:
    virtual void visit(StockCriterion * criterion)=0;
    virtual void visit(DistributorCriterion * criterion)=0;
    virtual void visit(ManufacturerCriterion * criterion)=0;
    virtual void visit(CreateDateCriterion * criterion)=0;
    virtual void visit(ConditionCriterion * criterion)=0;
    virtual void visit(FootprintCriterion * criterion)=0;
    virtual void visit(TextCriterion * criterion)=0;
    virtual void visit(CategoryCriterion * criterion)=0;
    virtual void visit(StorageCriterion * criterion)=0;
};
*/



#endif // FILTERBUILDER_H
