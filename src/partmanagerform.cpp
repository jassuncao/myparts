#include "partmanagerform.h"
#include <QVector>
#include "ui_partmanagerform.h"
#include "category/categorytreemodel.h"
#include "entities/categoriesdao.h"
#include <QStandardItemModel>
#include <QDebug>
#include <QMenu>
#include <QToolBar>
#include "partcategorydialog.h"
#include "qsortfiltersqlquerymodel.h"
#include <QSignalMapper>
#include <QDataWidgetMapper>
#include <QtGui>
#include <QtSql>
#include <QLayout>

#include <QVariant>
#include "partdialog.h"
#include "addstockdialog.h"
#include "parts/partsdao.h"
#include "removestockdialog.h"
#include "parts/stockhistoryproxymodel.h"
#include "parts/stockhistorymodel.h"
#include "widgets/stockactioncolumndelegate.h"
#include "widgets/datetimedelegate.h"
#include "widgets/currencydelegate.h"
#include "models/storagetreemodel.h"
#include "models/treeitem.h"
#include "entities/storagedao.h"
#include "widgets/flowlayout.h"
#include "widgets/qdualpushbutton.h"
#include "widgets/filteritemwidget.h"

PartManagerForm::PartManagerForm(QWidget *parent) :
    QWidget(parent),    
    ui(new Ui::PartManagerForm)
{
    ui->setupUi(this);   

    QToolBar * toolbar = new QToolBar();
    toolbar->setIconSize(QSize(16,16));
    ui->horizontalLayout_2->addWidget(toolbar);
    toolbar->addAction(QIcon(":/icons/toggle_expand"), "Expand", this, SLOT(on_expandTreeButton_clicked()));
    toolbar->addAction(QIcon(":/icons/toggle_collapse"), "Collapse", this, SLOT(on_collapseTreeButton_clicked()));

    QWidget* empty = new QWidget();
    empty->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);
    toolbar->addWidget(empty);    
    ui->horizontalLayout_2->addWidget(toolbar);


    _savedfilterPanelState = ui->filterSplitter->saveState();
    ui->filterHeader_2->setVisible(false);
    ui->filterSplitter->setCollapsible(0,false);
    ui->splitter_2->setChildrenCollapsible(false);

    ui->mainTreeView->setDragDropMode(QAbstractItemView::DragDrop);
    ui->mainTreeView->setDefaultDropAction(Qt::MoveAction);    

    _categoriesTreeModel = new CategoryTreeModel(this);
    _categoriesTreeModel->setToolTipColumn(Entities::CategoriesDAO::DESCRIPTION_COL);
    _categoriesTreeModel->select();
    ui->mainTreeView->setModel(_categoriesTreeModel);

    connect(ui->mainTreeView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(mainTreeView_currentChanged(QModelIndex,QModelIndex)));
    connect(ui->filterSplitter,SIGNAL(splitterMoved(int,int)), this, SLOT(filterSplitterMoved(int,int)));        
    connect(_categoriesTreeModel,SIGNAL(partDropped()), this, SLOT(refreshPartsModel()));

    _storageTreeModel = new StorageTreeModel(this);
    _storageTreeModel->select();

    _partsModel = new PartsSqlQueryModel2(this);      
    _partsModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    connect(_partsModel,SIGNAL(primeInsert(int,QSqlRecord&)), this, SLOT(slotPartsModelPrimeInsert(int,QSqlRecord&)));

    ui->partDetailsView->setPartsModel(_partsModel);
    ui->filterForm->setFilterBuilder(&_filterBuilder);
    ui->filterForm->setStorageModel(_storageTreeModel);

    ui->partsTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    //To enable in the future to allow editing stocks in place
    //ui->partsTableView->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed);
    ui->partsTableView->setModel(_partsModel);
    ui->partsTableView->sortByColumn(1,Qt::AscendingOrder);    

    _partsModel->setHeaderData(PartsSqlQueryModel2::ColumnName, Qt::Horizontal, tr("Name"));
    _partsModel->setHeaderData(PartsSqlQueryModel2::ColumnDescription, Qt::Horizontal, tr("Description"));
    _partsModel->setHeaderData(PartsSqlQueryModel2::ColumnActualStock, Qt::Horizontal, tr("Stock"));
    _partsModel->setHeaderData(PartsSqlQueryModel2::ColumnMinStock, Qt::Horizontal, tr("Min. Stock"));
    _partsModel->setHeaderData(PartsSqlQueryModel2::ColumnAvgPrice, Qt::Horizontal, tr("Avg. Price"));
    _partsModel->setHeaderData(PartsSqlQueryModel2::ColumnCustomPartNumber, Qt::Horizontal, tr("Custom Part#"));
    _partsModel->setHeaderData(PartsSqlQueryModel2::ColumnComment, Qt::Horizontal, tr("Comment"));
    _partsModel->setHeaderData(PartsSqlQueryModel2::ColumnCreateDate, Qt::Horizontal, tr("Create Date"));
    _partsModel->setHeaderData(PartsSqlQueryModel2::ColumnCategoryName, Qt::Horizontal, tr("Category"));
    _partsModel->setHeaderData(PartsSqlQueryModel2::ColumnStorage, Qt::Horizontal, tr("Storage Location"));
    _partsModel->setHeaderData(PartsSqlQueryModel2::ColumnCondition, Qt::Horizontal, tr("Condition"));
    _partsModel->setHeaderData(PartsSqlQueryModel2::ColumnFootprintName, Qt::Horizontal, tr("Footprint"));

    ui->partsTableView->setDragEnabled(true);
    ui->partsTableView->setDragDropMode(QAbstractItemView::DragOnly);
    ui->partsTableView->setDefaultDropAction(Qt::LinkAction);
    ui->partsTableView->setSortingEnabled(true);    
    ui->partsTableView->setColumnHidden(PartsSqlQueryModel2::ColumnId, true);
    ui->partsTableView->setColumnHidden(PartsSqlQueryModel2::ColumnCustomPartNumber, true);
    ui->partsTableView->setColumnHidden(PartsSqlQueryModel2::ColumnComment, true);
    ui->partsTableView->setColumnHidden(PartsSqlQueryModel2::ColumnCreateDate, true);
    ui->partsTableView->setColumnHidden(PartsSqlQueryModel2::ColumnPartUnit, true);
    ui->partsTableView->setColumnHidden(PartsSqlQueryModel2::ColumnPartUnitId, true);
    ui->partsTableView->setColumnHidden(PartsSqlQueryModel2::ColumnConditionId, true);
    ui->partsTableView->setColumnHidden(PartsSqlQueryModel2::ColumnCategoryId, true);
    ui->partsTableView->setColumnHidden(PartsSqlQueryModel2::ColumnStorageId, true);
    ui->partsTableView->setColumnHidden(PartsSqlQueryModel2::ColumnFootprintId, true);

    ui->partsTableView->setItemDelegateForColumn(PartsSqlQueryModel2::ColumnCreateDate, new DateDelegate(this));
    ui->partsTableView->setItemDelegateForColumn(PartsSqlQueryModel2::ColumnAvgPrice, new CurrencyDelegate(this));

    ui->partsTableView->verticalHeader()->setVisible(false);
    ui->partsTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->partsTableView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->partsTableView->horizontalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->partsTableView->horizontalHeader(), SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(partsTableViewHeaderContextMenu(QPoint)));   

    connect(ui->partsTableView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
            this, SLOT(partsTableView_currentRowChanged(QModelIndex,QModelIndex)));

    connect(ui->partsTableView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(slotEditPart()));

    connect(ui->filterForm, SIGNAL(filterChanged()), this, SLOT(slotFilterChanged()));
    connect(ui->partDetailsView, SIGNAL(addStockSelected()), this, SLOT(slotAddStock()));
    connect(ui->partDetailsView, SIGNAL(removeStockSelected()), this, SLOT(slotRemoveStock()));
    connect(ui->partDetailsView, SIGNAL(editPartSelected()), this, SLOT(slotEditPart()));
    ui->addPartToolButton->setDefaultAction(ui->actionAddPart);
    ui->deletePartToolButton->setDefaultAction(ui->actionDeletePart);
    //ui->duplicatePartButton->setDefaultAction(ui->actionDuplicatePart);
    connect(ui->actionAddPart, SIGNAL(triggered()), this, SLOT(slotAddPart()));
    connect(ui->actionDeletePart, SIGNAL(triggered()), this, SLOT(slotDeletePart()));
    connect(ui->actionDuplicatePart, SIGNAL(triggered()), this, SLOT(slotDuplicatePart()));
    createMenus();
    ui->filterForm->resetFilter();
    /*
    FlowLayout * flow = new FlowLayout();
    flow->addWidget(createFilterItem(ui->filterlabel1, ui->filter1));
    flow->addWidget(createFilterItem(ui->filterlabel2, ui->filter2));
    flow->addWidget(createFilterItem(ui->filterlabel3, ui->filter3));
    flow->addWidget(createFilterItem(ui->filterlabel4, ui->filter4));
    flow->addWidget(createFilterItem(ui->filterlabel5, ui->filter5));
    flow->addWidget(createFilterItem(ui->filterlabel6, ui->filter6));
    /*
    flow->addWidget(ui->filterlabel1);
    flow->addWidget(ui->filter1);
    flow->addWidget(ui->filterlabel2);
    flow->addWidget(ui->filter2);
    flow->addWidget(ui->filterlabel3);
    flow->addWidget(ui->filter3);
    flow->addWidget(ui->filterlabel4);
    flow->addWidget(ui->filter4);
    flow->addWidget(ui->filterlabel5);
    flow->addWidget(ui->filter5);
    flow->addWidget(ui->filterlabel6);
    flow->addWidget(ui->filter6);
    */
    //ui->filterFrame->setLayout(flow);
    QMenu * testMenu =new QMenu(this);

    testMenu->addAction("Action1");
    testMenu->addAction("Action2");
    //ui->actionDuplicatePart->setMenu(testMenu);
    //ui->duplicatePartButton->setMenu(testMenu);
    ui->moreButton->setMenu(testMenu);
    /*
    QDualPushButton * btn = new QDualPushButton;
    //btn->setText("Test: ASAS");
    ui->horizontalLayout_10->addWidget(btn);
*/
    /*
    FilterItemWidget * item1 = new FilterItemWidget(tr("Distributor:"));
    ui->extraItemsLayout->addWidget(item1);
    ui->extraItemsLayout->addWidget(new FilterItemWidget(tr("Manufacturer:")));
    ui->extraItemsLayout->addWidget(new FilterItemWidget(tr("Create before:")));
    */
    ui->extraItemsLayout->addStretch(1);
    //ui->horizontalLayout_10->addWidget(item1);
}

QWidget * PartManagerForm::createFilterItem(QWidget * label, QWidget * item)
{
    QHBoxLayout * layout = new QHBoxLayout();
    layout->setMargin(0);
    layout->setSpacing(4);
    layout->addWidget(label);
    layout->addWidget(item);
    QWidget * wraper = new QWidget();
    wraper->setLayout(layout);
    return wraper;
}

PartManagerForm::~PartManagerForm()
{
    delete ui;
}

void PartManagerForm::navigator_customContextMenuRequested(const QPoint &)
{
    QMenu menu;
    //menu.exec(ui->comboBox->mapToGlobal(pos));
}

void PartManagerForm::refreshPartsModel()
{
    _partsModel->select();
}

void PartManagerForm::partsTableViewHeaderContextMenu(QPoint point)
{
    //int column = ui->partsTableView->horizontalHeader()->logicalIndexAt(point);
    _tableHeaderContextMenu->popup(ui->partsTableView->horizontalHeader()->mapToGlobal(point));
}

void PartManagerForm::slotFilterChanged()
{
    _partsModel->setFilter(_filterBuilder.build());
    if(!_partsModel->query().isActive())
        _partsModel->select();
}

void PartManagerForm::filterSplitterMoved(int, int index)
{
    if(index==1){
        QList<int> sizes = ui->filterSplitter->sizes();
        if(sizes.at(1)==0){
            ui->filterHeader_2->setVisible(true);
        }
        else
            ui->filterHeader_2->setVisible(false);
    }
}

void PartManagerForm::mainTreeView_currentChanged(const QModelIndex &current, const QModelIndex&)
{    
    if(!current.isValid()) return;

    FilterBuilder::CategoryFilterMode catMode = _filterBuilder.categoryFilterMode();
    if(catMode == FilterBuilder::AllCategories)
        return;
    int nodeId = _categoriesTreeModel->getItemId(current);
    qDebug()<<"current category changed "<< nodeId;
    if(catMode == FilterBuilder::SubCategories){
        _filterBuilder.setSelectedCategories(Entities::CategoriesDAO::listChildCategories(nodeId));
    }
    else if(catMode == FilterBuilder::SelectedCategory){
        _filterBuilder.setSelectedCategories(QVector<QVariant>(1,nodeId));
    }
    slotFilterChanged();
}

void PartManagerForm::on_mainTreeView_customContextMenuRequested(const QPoint &pos)
{
    _categoriesCtxMenuIndex = ui->mainTreeView->indexAt(pos);
    bool indexValid = _categoriesCtxMenuIndex.isValid();
    bool canDelete = false;
    if(indexValid){
        canDelete = !_categoriesTreeModel->hasChildren(_categoriesCtxMenuIndex);
    }
    ui->actionEditCategory->setEnabled(indexValid);
    ui->actionDeleteCategory->setEnabled(canDelete);
    _categoriesContextMenu->exec(ui->mainTreeView->mapToGlobal(pos));
}

void PartManagerForm::createMenus()
{
    _categoriesContextMenu = new QMenu(this);
    _categoriesContextMenu->addAction(ui->actionAddCategory);
    _categoriesContextMenu->addAction(ui->actionEditCategory);
    _categoriesContextMenu->addAction(ui->actionDeleteCategory);

    _tableHeaderContextMenu = new QMenu(this);
    QMenu * colsMenu = _tableHeaderContextMenu->addMenu(QIcon(":/icons/table_select_column"),"Columns");
    createShowColumnAction(colsMenu, PartsSqlQueryModel2::ColumnName);
    createShowColumnAction(colsMenu, PartsSqlQueryModel2::ColumnDescription);
    createShowColumnAction(colsMenu, PartsSqlQueryModel2::ColumnActualStock);
    createShowColumnAction(colsMenu, PartsSqlQueryModel2::ColumnMinStock);
    createShowColumnAction(colsMenu, PartsSqlQueryModel2::ColumnAvgPrice);
    createShowColumnAction(colsMenu, PartsSqlQueryModel2::ColumnCustomPartNumber);
    createShowColumnAction(colsMenu, PartsSqlQueryModel2::ColumnComment);
    createShowColumnAction(colsMenu, PartsSqlQueryModel2::ColumnCreateDate);
    createShowColumnAction(colsMenu, PartsSqlQueryModel2::ColumnCategoryName);
    createShowColumnAction(colsMenu, PartsSqlQueryModel2::ColumnStorage);
    createShowColumnAction(colsMenu, PartsSqlQueryModel2::ColumnCondition);
}

QAction * PartManagerForm::createShowColumnAction(QMenu * parentMenu, const int columnNumber)
{
    QString columnName = ui->partsTableView->model()->headerData(columnNumber,Qt::Horizontal).toString();
    QAction * action = parentMenu->addAction(columnName);
    action->setCheckable(true);
    action->setChecked(!ui->partsTableView->isColumnHidden(columnNumber));
    action->setData(columnNumber);
    connect(action,SIGNAL(toggled(bool)),this, SLOT(onToggleTableColumn(bool)));
    return action;
}

void PartManagerForm::onToggleTableColumn(bool checked)
{
    QAction * action = (QAction*)sender();
    int column = action->data().toInt();
    ui->partsTableView->setColumnHidden(column,!checked);
}

void PartManagerForm::partsTableView_currentRowChanged(const QModelIndex &current, const QModelIndex&)
{   ui->partDetailsView->setCurrentIndex(current);
}

void PartManagerForm::on_actionAddCategory_triggered()
{    
    const QModelIndex & parentIndex = _categoriesCtxMenuIndex;
    if(!_categoriesTreeModel->insertItem(parentIndex))
        return;
    int row = _categoriesTreeModel->rowCount(parentIndex) - 1;
    PartCategoryDialog dlg(_categoriesTreeModel, this);
    dlg.setWindowTitle(tr("Add Category"));
    dlg.setRootIndex(parentIndex);
    dlg.setCurrentIndex(row);
    if(dlg.exec()==QDialog::Accepted){
        ui->mainTreeView->expand(parentIndex);
        QModelIndex childIndex = _categoriesTreeModel->index(row, 0, parentIndex);
        ui->mainTreeView->selectionModel()->select(childIndex, QItemSelectionModel::ClearAndSelect);
    }
    else{
        _categoriesTreeModel->revert();
    }

}

void PartManagerForm::on_actionDeleteCategory_triggered()
{
    QModelIndex currentIdx = ui->mainTreeView->selectionModel()->currentIndex();
    if(!currentIdx.isValid())
        return;
    _categoriesTreeModel->removeItem(currentIdx);
}

void PartManagerForm::on_actionEditCategory_triggered()
{
    QModelIndex currentIdx = ui->mainTreeView->selectionModel()->currentIndex();
    if(!currentIdx.isValid())
        return;

    PartCategoryDialog dlg(_categoriesTreeModel, this);
    dlg.setWindowTitle(tr("Edit Category"));
    dlg.setRootIndex(_categoriesTreeModel->parent(currentIdx));
    dlg.setCurrentModelIndex(currentIdx);
    if(dlg.exec()==QDialog::Accepted){
    }
    else{
        _categoriesTreeModel->revert();
    }

}

void PartManagerForm::on_colapseFilterPanelButton_clicked()
{
    _savedfilterPanelState = ui->filterSplitter->saveState();
    QList<int> sizes = ui->filterSplitter->sizes();
    sizes[1]=0;
    ui->filterSplitter->setSizes(sizes);
    ui->filterHeader_2->setVisible(true);
}

void PartManagerForm::on_expandFilterPanelButton_clicked()
{
    ui->filterSplitter->restoreState(_savedfilterPanelState);
    ui->filterHeader_2->setVisible(false);
}

void PartManagerForm::on_lineEdit_returnPressed()
{
}

void PartManagerForm::on_textSearchLineEdit_returnPressed()
{
    _filterBuilder.setTextFilter(ui->textSearchLineEdit->text());
    slotFilterChanged();
}

void PartManagerForm::slotAddStock()
{
    QModelIndex index = ui->partsTableView->currentIndex();
    if(!index.isValid())
        return;        
    QVariant partKey = _partsModel->data(_partsModel->index(index.row(),PartsSqlQueryModel2::ColumnId));
    QVariant partUnit = _partsModel->data(_partsModel->index(index.row(),PartsSqlQueryModel2::ColumnPartUnit));
    AddStockDialog dlg(this);
    dlg.setPartUnit(partUnit.toString());
    if(dlg.exec()!=QDialog::Accepted)
        return;
    if(PartsDAO::addStock(partKey, dlg.getStockChange(), dlg.getPartPrice(), dlg.getComment())){        
        QModelIndex stockIdx = _partsModel->index(index.row(), PartsSqlQueryModel2::ColumnActualStock);
        QModelIndex priceIdx = _partsModel->index(index.row(), PartsSqlQueryModel2::ColumnAvgPrice);
        int currentStock = _partsModel->data(stockIdx,Qt::EditRole).toInt();
        double currentAvgPrice = _partsModel->data(priceIdx,Qt::EditRole).toDouble();
        currentStock+=dlg.getStockChange();
        if(currentAvgPrice==0)
            currentAvgPrice = dlg.getPartPrice();
        else
            currentAvgPrice = (currentAvgPrice + dlg.getPartPrice())/2;
        if(
                _partsModel->setData(stockIdx,currentStock) &&
                _partsModel->setData(priceIdx,currentAvgPrice) &&
                _partsModel->submitAll())
        {
            qWarning()<<"Stock for part "<<partKey<<" changed to "<<currentStock;
        }
        else{
            //TODO: Display some warning
            qWarning()<<"Failed to update cached stock for part "<<partKey;
        }
    }
    else{
        //TODO: Display some warning
        qWarning("Failed to insert stock change (addition) in database");
    }
}

void PartManagerForm::slotRemoveStock()
{
    QModelIndex index = ui->partsTableView->currentIndex();
    if(!index.isValid())
        return;
    QVariant partKey = _partsModel->data(_partsModel->index(index.row(),PartsSqlQueryModel2::ColumnId));
    QVariant partUnit = _partsModel->data(_partsModel->index(index.row(),PartsSqlQueryModel2::ColumnPartUnit));
    QModelIndex actualStockIdx = _partsModel->index(index.row(), PartsSqlQueryModel2::ColumnActualStock);
    int availableStock = _partsModel->data(actualStockIdx,Qt::EditRole).toInt();
    RemoveStockDialog dlg(this);
    dlg.setPartUnit(partUnit.toString());
    dlg.setAvailableStock(availableStock);
    if(dlg.exec()!=QDialog::Accepted)
        return;
    if(PartsDAO::removeStock(partKey, dlg.getStockChange(), dlg.getComment())){
        availableStock+=dlg.getStockChange();
        if(_partsModel->setData(actualStockIdx,availableStock) && _partsModel->submit()){
            qWarning()<<"Stock for part "<<partKey<<" changed to "<<availableStock;
        }
        else{
            //TODO: Display some warning
            qWarning()<<"Failed to update cached stock for part "<<partKey;
        }
    }
    else{
        //TODO: Display some warning
        qWarning("Failed to insert stock change (removal) in database");
    }

}

void PartManagerForm::slotAddPart()
{
    //PartDialog dlg(_partsModel, _storageTreeModel, this);
    //dlg.addNewPart();
}

void PartManagerForm::slotEditPart()
{
    /*
    QModelIndex index = ui->partsTableView->currentIndex();
    if(!index.isValid())
        return;
    PartDialog dlg(_partsModel, _storageTreeModel, this);
    dlg.editPart(index);
    */
}

void PartManagerForm::slotDeletePart()
{
    qDebug("Deleting part");
    if(!ui->partsTableView->selectionModel()->hasSelection())
        return;
    QModelIndexList selectedIndexes = ui->partsTableView->selectionModel()->selectedIndexes();
    _partsModel->database().transaction();
    for(int i=0; i< selectedIndexes.size(); ++i){
        _partsModel->removeRow(selectedIndexes[i].row());
    }
    _partsModel->submitAll();
    _partsModel->database().commit();
}

void PartManagerForm::slotDuplicatePart()
{
    QModelIndex index = ui->partsTableView->currentIndex();
    if(!index.isValid())
        return;
    /*
    PartDialog dlg(_partsModel, _storageTreeModel, this);
    dlg.duplicatePart(index);
    */
}

void PartManagerForm::on_pushButton_2_clicked()
{
}

void PartManagerForm::on_deletePartButton_clicked()
{    
}

void PartManagerForm::slotPartsModelPrimeInsert(int, QSqlRecord &record)
{
    //Set the createDate field
    QDateTime now = QDateTime::currentDateTimeUtc();
    QVariant t = QVariant(now.toTime_t());
    record.setValue(PartsSqlQueryModel2::ColumnCreateDate, t);
    //The generated flag needs to be set or this field will not be included in the insert
    record.setGenerated(PartsSqlQueryModel2::ColumnCreateDate,true);

    //Set the category field using the category selected in the tree view
    QModelIndex currentCategoryIdx = ui->mainTreeView->selectionModel()->currentIndex();
    int categoryId;
    if(currentCategoryIdx.isValid()){
        categoryId = _categoriesTreeModel->getItemId(currentCategoryIdx);
    }
    else{
        categoryId = _categoriesTreeModel->rootItemId();
    }    
    record.setValue(PartsSqlQueryModel2::ColumnCategoryId, categoryId);
    record.setGenerated(PartsSqlQueryModel2::ColumnCategoryId,true);

    //Used to avoid an error in the sql statment generator
    record.setGenerated(PartsSqlQueryModel2::ColumnId,true);

    QSqlQuery q("SELECT id FROM part_unit WHERE defaultUnit=1");
    if(q.exec() && q.next()){
        record.setValue(PartsSqlQueryModel2::ColumnPartUnitId, q.value(0));
    }
    if(q.exec("SELECT id FROM part_condition WHERE defaultCondition=1")){
        if(q.next())
            record.setValue(PartsSqlQueryModel2::ColumnConditionId, q.value(0));
    }
}


void PartManagerForm::on_expandTreeButton_clicked()
{
    ui->mainTreeView->expandAll();
}

void PartManagerForm::on_collapseTreeButton_clicked()
{
    ui->mainTreeView->expandToDepth(0);
}

