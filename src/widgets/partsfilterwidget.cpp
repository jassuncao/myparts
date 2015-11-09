#include "partsfilterwidget.h"
#include "widgets/flowlayout.h"
#include "widgets/qsearchlineedit.h"
#include "widgets/filteritemwidget.h"
#include "models/extrarowproxymodel.h"
#include "parts/partssqlquerymodel2.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QAction>
#include <QMenu>
#include <QStandardItemModel>
#include <QSqlQueryModel>
#include <QComboBox>
#include <QDebug>
#include <QAbstractItemModel>


QFilterItemAction::QFilterItemAction(const QString &text, const int filterTag, QObject* parent) :
    QAction(text, parent), _filterTag(filterTag)
{
    setCheckable(true);
    connect(this, SIGNAL(toggled(bool)), this, SLOT(slotToggled(bool)));
}

void QFilterItemAction::slotToggled(bool b)
{
    if(b)
        emit checked(_filterTag);
    else
        emit unchecked(_filterTag);
}

PartsFilterWidget::PartsFilterWidget(QWidget *parent) : QWidget(parent)
{
    _fixedItemsLayout = new QHBoxLayout;
    _fixedItemsLayout->setMargin(0);
    _dynamicItemsLayout = new FlowLayout;
    _dynamicItemsLayout->setMargin(0);

    QVBoxLayout * mainLayout = new QVBoxLayout;
    mainLayout->setSpacing(6);
    mainLayout->setMargin(6);

    mainLayout->addLayout(_fixedItemsLayout);
    mainLayout->addLayout(_dynamicItemsLayout);
    setLayout(mainLayout);

    FilterItemWidget * stockFilterItem = createStockFilterItem();
    FilterItemWidget * conditionFilterItem = createPartConditionFilterItem();

    _textFilterItem = new QSearchLineEdit(this);
    _textFilterItem->setPlaceholderText("Search");

    _moreButton = new QPushButton(tr("More"), this);
    _moreButton->setStyleSheet(QString::fromLatin1(
                                   "QPushButton::menu-indicator {"
                                   " subcontrol-position: right center;"
                                   " subcontrol-origin: padding;"
                                   " left: -4px;"
                                   " }"));        
    _fixedItemsLayout->addWidget(stockFilterItem);
    _fixedItemsLayout->addWidget(conditionFilterItem);
    _fixedItemsLayout->addWidget(_textFilterItem);
    _fixedItemsLayout->addWidget(_moreButton);

    _moreButton->setMenu(createFilterMoreMenu());

    registerFilterItemWidget(stockFilterItem);
    registerFilterItemWidget(conditionFilterItem);

    connect(_textFilterItem, SIGNAL(textClear()), this, SLOT(slotTextFilterItemChanged()));
    connect(_textFilterItem, SIGNAL(returnPressed()), this, SLOT(slotTextFilterItemChanged()));
}

PartsFilterWidget::~PartsFilterWidget()
{
}

void PartsFilterWidget::setFilterBuilder(FilterBuilder * filterBuilder)
{
    _filterBuilder = filterBuilder;
}

QMenu * PartsFilterWidget::createFilterMoreMenu()
{
    QMenu * menu = new QMenu(this);
    createFilterItemAction(menu, tr("Distributor"), FilterBuilder::FilterByDistributor);
    createFilterItemAction(menu, tr("Manufacturer"), FilterBuilder::FilterByManufacturer);
    createFilterItemAction(menu, tr("Footprint"), FilterBuilder::FilterByFootprint);
    return menu;
}

QAction * PartsFilterWidget::createFilterItemAction(QMenu * menu, const QString & title, const int filterTag)
{
    QFilterItemAction * action = new QFilterItemAction(title, filterTag, menu);
    menu->addAction(action);
    connect(action, SIGNAL(unchecked(int)), this, SLOT(slotRemoveFilterItem(int)));
    connect(action, SIGNAL(checked(int)), this, SLOT(slotAddFilterItem(int)));
    return action;
}

void PartsFilterWidget::registerFilterItemWidget(FilterItemWidget * filterItem)
{
    _activeFilterItems[filterItem->filterTag()]=filterItem;
    connect(filterItem, SIGNAL(valueChanged(int,QVariant)), this, SLOT(slotFilterItemValueChange(int,QVariant)));
    if(filterItem->removable()){
        connect(filterItem, SIGNAL(deleteItem(int)), this,SLOT(slotDeleteFilterItem(int)));
    }
}

FilterItemWidget * PartsFilterWidget::createStockFilterItem()
{
    QStandardItemModel * stockFilterItemModel = new QStandardItemModel(4,0,this);
    QStandardItem * anyStockItem = new QStandardItem(tr("Any"));
    anyStockItem->setData(FilterBuilder::AnyStockLevel);

    QStandardItem * zeroStockItem = new QStandardItem(tr("Out of stock"));
    zeroStockItem->setData(FilterBuilder::StockLevelZero);

    QStandardItem * inStockItem = new QStandardItem(tr("In Stock"));
    inStockItem->setData(FilterBuilder::StockLevelGreaterZero);

    QStandardItem * belowMinStockItem = new QStandardItem(tr("Below Minimum"));
    belowMinStockItem->setData(FilterBuilder::StockLevelBelowMin);

    stockFilterItemModel->setItem(0,anyStockItem);
    stockFilterItemModel->setItem(1,zeroStockItem);
    stockFilterItemModel->setItem(2,inStockItem);
    stockFilterItemModel->setItem(3,belowMinStockItem);

    FilterItemWidget * item  = new FilterItemWidget(tr("Stock:"), FilterBuilder::FilterByStock, false, this);
    item->setOptionsModel(stockFilterItemModel);
    item->setDisplayColumn(0);
    item->setValueColumn(0,  Qt::UserRole + 1);
    return item;
}

FilterItemWidget * PartsFilterWidget::createPartConditionFilterItem()
{
    QSqlQueryModel * conditionModel = new QSqlQueryModel(this);
    conditionModel->setQuery("SELECT id, value FROM part_condition");

    ExtraRowProxyModel * conditionProxyModel = new ExtraRowProxyModel(this);
    conditionProxyModel->setEmptyDisplayText(tr("Any"));
    conditionProxyModel->setSourceModel(conditionModel);

    FilterItemWidget * item  = new FilterItemWidget(tr("Condition:"), FilterBuilder::FilterByCondition, false, this);
    item->setOptionsModel(conditionProxyModel);
    item->setDisplayColumn(1);
    item->setValueColumn(0, Qt::EditRole);
    return item;
}

FilterItemWidget * PartsFilterWidget::createPartDistributorFilterItem()
{
    QSqlQueryModel * sourceModel = new QSqlQueryModel(this);
    sourceModel->setQuery("SELECT id, name FROM distributor");

    ExtraRowProxyModel * proxyModel = new ExtraRowProxyModel(this);
    proxyModel->setEmptyDisplayText(tr("Any"));
    proxyModel->setSourceModel(sourceModel);

    FilterItemWidget * item  = new FilterItemWidget(tr("Distributor:"), FilterBuilder::FilterByDistributor, true, this);
    item->setOptionsModel(proxyModel);
    item->setDisplayColumn(1);
    item->setValueColumn(0, Qt::EditRole);
    return item;
}

FilterItemWidget * PartsFilterWidget::createPartManufacturerFilterItem()
{
    QSqlQueryModel * sourceModel = new QSqlQueryModel(this);
    sourceModel->setQuery("SELECT id, name FROM manufacturer");

    ExtraRowProxyModel * proxyModel = new ExtraRowProxyModel(this);
    proxyModel->setEmptyDisplayText(tr("Any"));
    proxyModel->setSourceModel(sourceModel);

    FilterItemWidget * item  = new FilterItemWidget(tr("Manufacturer:"), FilterBuilder::FilterByManufacturer, true, this);
    item->setOptionsModel(proxyModel);
    item->setDisplayColumn(1);
    item->setValueColumn(0, Qt::EditRole);
    return item;
}

FilterItemWidget * PartsFilterWidget::createPartFootprintFilterItem()
{
    QSqlQueryModel * sourceModel = new QSqlQueryModel(this);
    sourceModel->setQuery("SELECT id, name FROM part_footprint");

    ExtraRowProxyModel * proxyModel = new ExtraRowProxyModel(this);
    proxyModel->setEmptyDisplayText(tr("Any"));
    proxyModel->setSourceModel(sourceModel);

    FilterItemWidget * item  = new FilterItemWidget(tr("Footprint:"), FilterBuilder::FilterByFootprint, true, this);
    item->setOptionsModel(proxyModel);
    item->setDisplayColumn(1);
    item->setValueColumn(0, Qt::EditRole);
    return item;
}

void PartsFilterWidget::slotAddFilterItem(const int filterTag)
{
    if(_activeFilterItems.contains(filterTag)){
        qWarning()<<"Attempt to add a duplicate filter item "<<filterTag;
        return;
    }
    FilterItemWidget * item = 0;
    switch(filterTag){
        case FilterBuilder::FilterByDistributor:
        item = createPartDistributorFilterItem();
        break;
    case FilterBuilder::FilterByManufacturer:
        item = createPartManufacturerFilterItem();
        break;
    case FilterBuilder::FilterByFootprint:
        item = createPartFootprintFilterItem();
        break;
    default:
        qDebug()<<"Missing filter creator for "<<filterTag;
        break;
    }

    if(item){
        registerFilterItemWidget(item);
        _dynamicItemsLayout->addWidget(item);
    }
}


void PartsFilterWidget::slotRemoveFilterItem(const int filterTag)
{
    FilterItemWidget * item = _activeFilterItems.value(filterTag, 0);
    if(item && item->removable()){
        _activeFilterItems.remove(filterTag);
        _dynamicItemsLayout->removeWidget(item);
        item->deleteLater();
        slotFilterItemValueChange(filterTag, QVariant());
    }
    else{
        qWarning()<<"Filter item not found for "<<filterTag;
    }
}

void PartsFilterWidget::slotFilterItemValueChange(const int filterTag, const QVariant & value)
{
    qDebug()<<"Filter criterion for "<<filterTag<<" is "<<value;
    if(_filterBuilder)
        _filterBuilder->setFilter((FilterBuilder::SuportedFilters)filterTag, value);
}

void PartsFilterWidget::slotDeleteFilterItem(const int filterTag)
{    
    QList<QAction*> actions = _moreButton->menu()->actions();
    QAction* action;
    foreach (action, actions) {
        QFilterItemAction * filterItemAction = dynamic_cast<QFilterItemAction*>(action);
        if(filterItemAction && filterItemAction->filterTag()==filterTag){
            action->setChecked(false);
            return;
        }
    }
}

void PartsFilterWidget::slotTextFilterItemChanged()
{
    const QString & text = _textFilterItem->text();
    slotFilterItemValueChange(FilterBuilder::FilterByText, text);
}



