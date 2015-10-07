#include "partsfilterform.h"
#include "ui_partsfilterform.h"
#include <qsqlquerymodel.h>
#include <QPushButton>
#include "entities/storagedao.h"
#include "models/storagetreemodel.h"
#include "models/treeitem.h"
#include <QDebug>


PartsFilterForm::PartsFilterForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PartsFilterForm),
    _filterBuilder(0)
{
    ui->setupUi(this);

    /*
    QVector<QVariant> headerData(3);
    headerData.append("Name");
    headerData.append("Description");
    headerData.append("ID");

    TreeItem * rootItem = new TreeItem(-1, QVector<QVariant>());
    StorageTreeModel * storageTreeModel = new StorageTreeModel(rootItem, this);
    storageTreeModel->select();
    ui->storageFilterCombo->setModel(storageTreeModel);    
    */

    QSqlQueryModel * conditionModel = new QSqlQueryModel(this);
    conditionModel->setQuery("SELECT id, value FROM part_condition");
    ui->filterByCondtitionCombo->setModel(conditionModel);
    ui->filterByCondtitionCombo->setModelColumn(1);
    ui->filterByCondtitionCombo->setCurrentIndex(-1);

    QButtonGroup * group = new QButtonGroup(this);
    group->addButton(ui->allNodesScopeRadio, FilterBuilder::AllCategories);
    group->addButton(ui->allSubNodesScopeRadio, FilterBuilder::SubCategories);
    group->addButton(ui->selectedNodeScopeRadio, FilterBuilder::SelectedCategory);
    connect(group, SIGNAL(buttonClicked(int)), this, SLOT(slotFormChanged()));

    group = new QButtonGroup(this);
    group->addButton(ui->stockAnyRadio, FilterBuilder::AnyStockLevel);
    group->addButton(ui->stockZeroRadio, FilterBuilder::StockLevelZero);
    group->addButton(ui->stockGreateZeroRadio, FilterBuilder::StockLevelGreaterZero);
    group->addButton(ui->stockBelowMinRadio, FilterBuilder::StockLevelBelowMin);
    connect(group, SIGNAL(buttonClicked(int)), this, SLOT(slotFormChanged()));


    connect(ui->filterButtonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(slotFilterButtonBox_clicked(QAbstractButton*)));
    connect(ui->storageFilterCheck, SIGNAL(toggled(bool)), this, SLOT(slotFormChanged()));
    connect(ui->storageFilterCheck, SIGNAL(toggled(bool)), ui->storageFilterCombo, SLOT(setEnabled(bool)));
    connect(ui->storageFilterCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(slotFormChanged()));

    connect(ui->dateFilterOperatorCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(slotFormChanged()));
    connect(ui->dateFilterOperatorCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(slotDateFilterOperatorCombo_indexChanged(int)));        

    connect(ui->filterByConditionCheckbox, SIGNAL(toggled(bool)), ui->filterByCondtitionCombo, SLOT(setEnabled(bool)));
    connect(ui->filterByConditionCheckbox, SIGNAL(toggled(bool)), this, SLOT(slotFormChanged()));
    connect(ui->filterByCondtitionCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(slotFormChanged()));
}

PartsFilterForm::~PartsFilterForm()
{
    delete ui;
}

void PartsFilterForm::setStorageModel(TreeItemModel * storageModel)
{
    ui->storageFilterCombo->setModel(storageModel);
}

void PartsFilterForm::slotFormChanged()
{
    ui->filterButtonBox->button(QDialogButtonBox::Reset)->setEnabled(true);
}

void PartsFilterForm::resetFilter()
{
    ui->filterButtonBox->button(QDialogButtonBox::Reset)->setEnabled(false);
    ui->storageFilterCheck->setChecked(false);
    ui->allSubNodesScopeRadio->setChecked(true);
    ui->stockAnyRadio->setChecked(true);
    ui->dateFilterOperatorCombo->setCurrentIndex(0);
    ui->dateFilterEdit->setDate(QDate::currentDate());
    ui->dateFilterEdit->setEnabled(false);
    ui->filterByConditionCheckbox->setChecked(false);
    applyFilter();
}

void PartsFilterForm::applyFilter()
{
    if(!_filterBuilder) return;

    //Check if filter by storage location is enabled and set the filter accordingly
    QModelIndex selectedStorageIndex = ui->storageFilterCombo->view()->currentIndex();
    if(ui->storageFilterCheck->isChecked()
            && selectedStorageIndex.isValid())
    {
        TreeItemModel* model = dynamic_cast<TreeItemModel*>(ui->storageFilterCombo->model());
        int itemId = model->getItemId(selectedStorageIndex);
        StorageDAO storageDAO;
        _filterBuilder->setSelectedStorageLocations(storageDAO.listChildStorage(itemId));
        _filterBuilder->setStorageLocationFilterMode(FilterBuilder::StorageLocationFilterSub);
    }
    else {
        _filterBuilder->setStorageLocationFilterMode(FilterBuilder::StorageLocationFilterAll);
    }    

    FilterBuilder::CategoryFilterMode categoryFilterMode = (FilterBuilder::CategoryFilterMode) ui->allNodesScopeRadio->group()->checkedId();
    _filterBuilder->setCategoryFilterMode(categoryFilterMode);

    //Filter by stock stuff starts
    FilterBuilder::StockFilterMode stockFilterMode = (FilterBuilder::StockFilterMode)ui->stockAnyRadio->group()->checkedId();
    _filterBuilder->setStockFilterMode(stockFilterMode);        

    //Filter by date stuff starts
    int dateFilterComboIdx = ui->dateFilterOperatorCombo->currentIndex();
    FilterBuilder::DateFilterMode dateFilterMode;
    switch(dateFilterComboIdx){
        case 1:
            dateFilterMode = FilterBuilder::DateFilterBefore;
            break;
        case 2:
            dateFilterMode = FilterBuilder::DateFilterOn;
            break;
        case 3:
            dateFilterMode = FilterBuilder::DateFilterAfter;
            break;
    default:
        dateFilterMode = FilterBuilder::DateFilterNone;
    }
    _filterBuilder->setDateFilterMode(dateFilterMode);
    _filterBuilder->setSelectedDate(ui->dateFilterEdit->date());

    //Filter by condition stuff
    bool filterByCondition = ui->filterByConditionCheckbox->isChecked();
    _filterBuilder->setFilterByConditionEnabled(filterByCondition);
    if(filterByCondition){
        int conditionIdx = ui->filterByCondtitionCombo->currentIndex();
        QAbstractItemModel * model = ui->filterByCondtitionCombo->model();
        QVariant key = model->data(model->index(conditionIdx,0));
        _filterBuilder->setSelectedCondition(key);
    }
    emit filterChanged();
}

void PartsFilterForm::slotFilterButtonBox_clicked(QAbstractButton *button)
{
    QDialogButtonBox::StandardButton standardButton = ui->filterButtonBox->standardButton(button);
    switch(standardButton) {
        case QDialogButtonBox::Apply:
            applyFilter();
            emit filterChanged();
        break;
    case QDialogButtonBox::Reset:
            resetFilter();
            emit filterChanged();
        break;
    default:
        break;
    }
}

void PartsFilterForm::slotDateFilterOperatorCombo_indexChanged(int index)
{
    ui->dateFilterEdit->setEnabled(index>0);
}

