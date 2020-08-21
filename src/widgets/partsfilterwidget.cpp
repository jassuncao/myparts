#include "partsfilterwidget.h"
#include "widgets/flowlayout.h"
#include <qsearchlineedit.h>
#include "widgets/filteritemwidget.h"
#include "util/proxymodelnoneentry.h"
#include "part/model/partssqltablemodel.h"
#include "part/model/partsquerybuilder.h"
#include "models/modelsrepository.h"
#include "part/model/partconditionmodel.h"
#include "core/sql/basicentitytablemodel.h"

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

PartsFilterWidget::PartsFilterWidget(ModelsRepository *modelsRepository, QWidget *parent) : QWidget(parent),
    _modelsRepository(modelsRepository)
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
    _textFilterItem->setSearchIcon(QIcon(":/icons/find"));
    _textFilterItem->setClearIcon(QIcon(":/icons/edit-clear-location-rtl"));

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

void PartsFilterWidget::setPartsQueryBuilder(PartsQueryBuilder * partsQueryBuilder)
{
    _partsQueryBuilder = partsQueryBuilder;
}

QMenu * PartsFilterWidget::createFilterMoreMenu()
{
    QMenu * menu = new QMenu(this);
    createFilterItemAction(menu, tr("Distributor"), PartsQueryBuilder::FilterByDistributor);
    createFilterItemAction(menu, tr("Manufacturer"), PartsQueryBuilder::FilterByManufacturer);
    createFilterItemAction(menu, tr("Package"), PartsQueryBuilder::FilterByPackage);
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

void PartsFilterWidget::insertFixedFilterItemWidget(FilterItemWidget * filterItem)
{
    _fixedItemsLayout->insertWidget(0, filterItem);
    registerFilterItemWidget(filterItem);
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
    anyStockItem->setData(PartsQueryBuilder::AnyStockLevel);

    QStandardItem * zeroStockItem = new QStandardItem(tr("Out of stock"));
    zeroStockItem->setData(PartsQueryBuilder::StockLevelZero);

    QStandardItem * inStockItem = new QStandardItem(tr("In Stock"));
    inStockItem->setData(PartsQueryBuilder::StockLevelGreaterZero);

    QStandardItem * belowMinStockItem = new QStandardItem(tr("Below Minimum"));
    belowMinStockItem->setData(PartsQueryBuilder::StockLevelBelowMin);

    stockFilterItemModel->setItem(0,anyStockItem);
    stockFilterItemModel->setItem(1,zeroStockItem);
    stockFilterItemModel->setItem(2,inStockItem);
    stockFilterItemModel->setItem(3,belowMinStockItem);

    ComboFilterItemWidget * item  = new ComboFilterItemWidget(tr("Stock:"), PartsQueryBuilder::FilterByStock, false, this);
    item->setOptionsModel(stockFilterItemModel);
    item->setDisplayColumn(0);
    item->setValueColumn(0,  Qt::UserRole + 1);
    return item;
}

FilterItemWidget * PartsFilterWidget::createPartConditionFilterItem()
{  
    QAbstractItemModel * source = _modelsRepository->partConditionModel();

    ProxyModelNoneEntry * proxyModel = new ProxyModelNoneEntry(tr("Any"), QVariant(), this);
    proxyModel->setSourceModel(source);

    ComboFilterItemWidget * item  = new ComboFilterItemWidget(tr("Condition:"), PartsQueryBuilder::FilterByCondition, false, this);
    item->setOptionsModel(proxyModel);
    item->setDisplayColumn(PartConditionModel::ColumnValue);
    item->setValueColumn(PartConditionModel::ColumnId, Qt::EditRole);
    return item;
}

FilterItemWidget * PartsFilterWidget::createPartDistributorFilterItem()
{
    QAbstractTableModel * sourceModel = _modelsRepository->distributorModel();

    ProxyModelNoneEntry * proxyModel = new ProxyModelNoneEntry(tr("Any"), QVariant(), this);
    proxyModel->setSourceModel(sourceModel);

    ComboFilterItemWidget * item  = new ComboFilterItemWidget(tr("Distributor:"), PartsQueryBuilder::FilterByDistributor, true, this);
    item->setOptionsModel(proxyModel);
    item->setDisplayColumn(CompanyTableModel::ColumnName);
    item->setValueColumn(CompanyTableModel::ColumnId, Qt::EditRole);
    return item;
}

FilterItemWidget * PartsFilterWidget::createPartManufacturerFilterItem()
{
    QAbstractTableModel * sourceModel = _modelsRepository->manufacturerModel();

    ProxyModelNoneEntry * proxyModel = new ProxyModelNoneEntry(tr("Any"), QVariant(), this);
    proxyModel->setSourceModel(sourceModel);

    ComboFilterItemWidget * item  = new ComboFilterItemWidget(tr("Manufacturer:"), PartsQueryBuilder::FilterByManufacturer, true, this);
    item->setOptionsModel(proxyModel);
    item->setDisplayColumn(CompanyTableModel::ColumnName);
    item->setValueColumn(CompanyTableModel::ColumnId, Qt::EditRole);
    return item;
}

FilterItemWidget * PartsFilterWidget::createPartPackageFilterItem()
{
    QAbstractTableModel * sourceModel = _modelsRepository->packageModel();

    ProxyModelNoneEntry * proxyModel = new ProxyModelNoneEntry(tr("Any"), QVariant(), this);
    proxyModel->setSourceModel(sourceModel);

    ComboFilterItemWidget * item  = new ComboFilterItemWidget(tr("Package:"), PartsQueryBuilder::FilterByPackage, true, this);
    item->setOptionsModel(proxyModel);
    item->setDisplayColumn(PackageTableModel::ColumnName);
    item->setValueColumn(PackageTableModel::ColumnId, Qt::EditRole);
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
        case PartsQueryBuilder::FilterByDistributor:
        item = createPartDistributorFilterItem();
        break;
    case PartsQueryBuilder::FilterByManufacturer:
        item = createPartManufacturerFilterItem();
        break;
    case PartsQueryBuilder::FilterByPackage:
        item = createPartPackageFilterItem();
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
    /*
    if(_tableModel){
        _tableModel->setFilter((PartsSqlTableModel::SuportedFilters)filterTag, value);
    }
    */
    if(_partsQueryBuilder)
        _partsQueryBuilder->setFilter((PartsQueryBuilder::SupportedFilters)filterTag, value);
    emit filterChanged();
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
    slotFilterItemValueChange(PartsQueryBuilder::FilterByText, text);
}
