#include "partsfilterwidget.h"
#include "widgets/flowlayout.h"
#include "widgets/qsearchlineedit.h"
#include "widgets/filteritemwidget.h"
#include "models/extrarowproxymodel.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QAction>
#include <QMenu>
#include <QStandardItemModel>
#include <QSqlQueryModel>
#include <QComboBox>

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

    _stockFilterItem = new FilterItemWidget(tr("Stock:"), false, this);
    _conditionFilterItem = new FilterItemWidget(tr("Condition:"), false, this);
    _distributorFilterItem = new FilterItemWidget(tr("Distributor:"), false, this);
    _textFilterItem = new QSearchLineEdit(this);
    _textFilterItem->setPlaceholderText("Search");

    _moreMenu = new QMenu(this);
    _moreButton = new QPushButton(tr("More"), this);
    _moreButton->setStyleSheet(QString::fromLatin1(
                                   "QPushButton::menu-indicator {"
                                   " subcontrol-position: right center;"
                                   " subcontrol-origin: padding;"
                                   " left: -4px;"
                                   " }"));
    //_moreButton->setMenu(_moreMenu);
    _fixedItemsLayout->addWidget(_stockFilterItem);
    _fixedItemsLayout->addWidget(_conditionFilterItem);
    _fixedItemsLayout->addWidget(_distributorFilterItem);
    _fixedItemsLayout->addWidget(_textFilterItem);
    _fixedItemsLayout->addWidget(_moreButton);

    setupModels();

    connect(_moreButton, SIGNAL(clicked()), this, SLOT(slotAddFilterItem()));   
}

PartsFilterWidget::~PartsFilterWidget()
{
}

void PartsFilterWidget::setupModels()
{
    QStandardItemModel * stockFilterItemModel = new QStandardItemModel(4,0,this);
    stockFilterItemModel->setItem(0,new QStandardItem(tr("Any")));
    stockFilterItemModel->setItem(1,new QStandardItem(tr("Out of stock")));
    stockFilterItemModel->setItem(2,new QStandardItem(tr("In Stock")));
    stockFilterItemModel->setItem(3,new QStandardItem(tr("Below Minimum")));
    _stockFilterItem->setOptionsModel(stockFilterItemModel, 0);
    _stockFilterItem->comboBox()->insertSeparator(1);

    QSqlQueryModel * conditionModel = new QSqlQueryModel(this);
    conditionModel->setQuery("SELECT id, value FROM part_condition");

    ExtraRowProxyModel * conditionProxyModel = new ExtraRowProxyModel(this);
    conditionProxyModel->setEmptyDisplayText(tr("Any"));
    conditionProxyModel->setSourceModel(conditionModel);

    _conditionFilterItem->setOptionsModel(conditionProxyModel, 1);
    _conditionFilterItem->comboBox()->setCurrentIndex(0);

    QSqlQueryModel * distributorModel = new QSqlQueryModel(this);
    distributorModel->setQuery("SELECT id, name FROM distributor");

    ExtraRowProxyModel * distributorProxyModel = new ExtraRowProxyModel(this);
    distributorProxyModel->setEmptyDisplayText(tr("Any"));
    distributorProxyModel->setSourceModel(distributorModel);

    _distributorFilterItem->setOptionsModel(distributorProxyModel, 1);
    _distributorFilterItem->comboBox()->setCurrentIndex(0);

}

void PartsFilterWidget::slotAddFilterItem()
{
    FilterItemWidget * item = new FilterItemWidget("Item 1:", true, this);
    _dynamicItemsLayout->addWidget(item);
    connect(item, SIGNAL(deleteItem()), this, SLOT(slotDeleteFilterItem()));
}

void PartsFilterWidget::slotDeleteFilterItem()
{
    QWidget * item = (QWidget*)sender();
    _dynamicItemsLayout->removeWidget(item);
    item->deleteLater();
}

