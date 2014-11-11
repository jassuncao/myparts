#include "partmanagerform.h"
#include <QVector>
#include "ui_partmanagerform.h"
#include "category/categorytreemodel.h"
#include "entities/categoriesdao.h"
#include <QStandardItemModel>
#include <QDebug>
#include <QMenu>
#include "partcategorydialog.h"
#include "qsortfiltersqlquerymodel.h"
#include <QSignalMapper>
#include <QDataWidgetMapper>
#include <QtGui>
#include <QtSql>

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

PartManagerForm::PartManagerForm(QWidget *parent) :
    QWidget(parent),    
    ui(new Ui::PartManagerForm)
{
    ui->setupUi(this);
    _savedfilterPanelState = ui->filterSplitter->saveState();
    ui->filterHeader_2->setVisible(false);
    ui->filterSplitter->setCollapsible(0,false);
    ui->splitter_2->setChildrenCollapsible(false);

    ui->categoriesTreeView->setDragDropMode(QAbstractItemView::DragDrop);
    ui->categoriesTreeView->setDefaultDropAction(Qt::MoveAction);
    QVector<QVariant> headerData(3);
    headerData.append("Name");
    headerData.append("Description");
    headerData.append("ID");

    _categoriesTreeModel = new CategoryTreeModel(headerData, this);
    _categoriesTreeModel->setToolTipColumn(Entities::CategoriesDAO::DESCRIPTION_COL);
    _categoriesTreeModel->select();

    ui->categoriesTreeView->setModel(_categoriesTreeModel);
    connect(ui->categoriesTreeView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(categoriesTreeView_currentChanged(QModelIndex,QModelIndex)));
    connect(ui->filterSplitter,SIGNAL(splitterMoved(int,int)), this, SLOT(filterSplitterMoved(int,int)));        
    connect(_categoriesTreeModel,SIGNAL(partDropped()), this, SLOT(partCategoryChanged()));      

    _partsModel = new PartsSqlQueryModel2(this);
    _partsModel->setTable("part");    
    _partsModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    connect(_partsModel,SIGNAL(primeInsert(int,QSqlRecord&)), this, SLOT(part_primeInsert(int,QSqlRecord&)));    

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

    ui->partsTableView->setItemDelegateForColumn(PartsSqlQueryModel2::ColumnCreateDate, new DateDelegate());
    ui->partsTableView->setItemDelegateForColumn(PartsSqlQueryModel2::ColumnAvgPrice, new CurrencyDelegate());

    ui->partsTableView->verticalHeader()->setVisible(false);
    ui->partsTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->partsTableView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->partsTableView->horizontalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->partsTableView->horizontalHeader(), SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(partsTableViewHeaderContextMenu(QPoint)));   


    QSqlQueryModel * storageLocationsModel = new QSqlQueryModel(this);
    storageLocationsModel->setQuery("SELECT id, name FROM part_storage");
    ui->filterByStorageCombo->setModel(storageLocationsModel);
    ui->filterByStorageCombo->setModelColumn(1);
    ui->filterByStorageCombo->setCurrentIndex(-1);  

    QSqlQueryModel * conditionModel = new QSqlQueryModel(this);
    conditionModel->setQuery("SELECT id, value FROM part_condition");
    ui->filterByCondtitionCombo->setModel(conditionModel);
    ui->filterByCondtitionCombo->setModelColumn(1);
    ui->filterByCondtitionCombo->setCurrentIndex(-1);

    connect(ui->partsTableView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
            this, SLOT(partsTableView_currentRowChanged(QModelIndex,QModelIndex)));

    QPushButton * addStockButton = new QPushButton(QIcon(QString::fromUtf8(":/icons/addStock")), tr("Add Stock"),ui->partActionsButtonBox);
    QPushButton * removeStockButton = new QPushButton(QIcon(QString::fromUtf8(":/icons/removeStock")), tr("Remove Stock"),ui->partActionsButtonBox);
    QPushButton * editPartButton = new QPushButton(QIcon(QString::fromUtf8(":/icons/editPart")), tr("Edit Part"),ui->partActionsButtonBox);
    ui->partActionsButtonBox->addButton(addStockButton, QDialogButtonBox::ActionRole);
    ui->partActionsButtonBox->addButton(removeStockButton, QDialogButtonBox::ActionRole);
    ui->partActionsButtonBox->addButton(editPartButton, QDialogButtonBox::ActionRole);
    connect(addStockButton, SIGNAL(clicked()), this, SLOT(onAddStock()));
    connect(removeStockButton, SIGNAL(clicked()), this, SLOT(onRemoveStock()));
    connect(editPartButton, SIGNAL(clicked()), this, SLOT(onEditPart()));
    connect(ui->dateFilterOperatorCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(dateFilterOperatorCombo_indexChanged(int)));
    createMenus();
    resetFilter();
}

PartManagerForm::~PartManagerForm()
{
    delete ui;
}

void PartManagerForm::partCategoryChanged()
{
    _partsModel->select();
}

void PartManagerForm::partsTableViewHeaderContextMenu(QPoint point)
{
    //int column = ui->partsTableView->horizontalHeader()->logicalIndexAt(point);
    _tableHeaderContextMenu->popup(ui->partsTableView->horizontalHeader()->mapToGlobal(point));
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

void PartManagerForm::categoriesTreeView_currentChanged(const QModelIndex &current, const QModelIndex&)
{    
    if(current.isValid()){

        FilterBuilder::CategoryFilteringMode catMode = _filterBuilder.categoryFilteringMode();
        qDebug("Catt:%d",catMode);
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
        _partsModel->setFilter(_filterBuilder.build());
    }
}

void PartManagerForm::on_categoriesTreeView_customContextMenuRequested(const QPoint &pos)
{
    _categoriesCtxMenuIndex = ui->categoriesTreeView->indexAt(pos);
    bool indexValid = _categoriesCtxMenuIndex.isValid();
    bool canDelete = false;
    if(indexValid){
        canDelete = !_categoriesTreeModel->hasChildren(_categoriesCtxMenuIndex);
    }
    ui->actionEditCategory->setEnabled(indexValid);
    ui->actionDeleteCategory->setEnabled(canDelete);
    _categoriesContextMenu->exec(ui->categoriesTreeView->mapToGlobal(pos));
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
{   
    ui->partActionsButtonBox->setEnabled(current.isValid());
    updatePartDetailView(current);
    updateStockView(current);
}

QString columnDisplayData(QAbstractItemModel * model, const QModelIndex & current, int column){
    if(!current.isValid())
        return QString();
    QModelIndex colIndex = model->index(current.row(), column);
    return colIndex.isValid()? colIndex.data().toString() : QString();
}

QString columnDisplayDate(QAbstractItemModel * model, const QModelIndex & current, int column){
    if(!current.isValid())
        return QString();
    QModelIndex colIndex = model->index(current.row(), column);
    if(!colIndex.isValid())
        return QString();
    return QDateTime::fromTime_t(colIndex.data(Qt::EditRole).toUInt()).toString(Qt::DefaultLocaleShortDate);
}

void PartManagerForm::updatePartDetailView(const QModelIndex & current)
{
    ui->partNameLabel->setText(columnDisplayData(_partsModel, current, PartsSqlQueryModel2::ColumnName));
    ui->partDescriptionLabel->setText(columnDisplayData(_partsModel, current, PartsSqlQueryModel2::ColumnDescription));
    ui->partCategoryLabel->setText(columnDisplayData(_partsModel, current, PartsSqlQueryModel2::ColumnCategoryName));
    ui->partStockLevelLabel->setText(columnDisplayData(_partsModel, current, PartsSqlQueryModel2::ColumnActualStock));
    ui->partMinStockLabel->setText(columnDisplayData(_partsModel, current, PartsSqlQueryModel2::ColumnMinStock));
    ui->partCustomNumberLabel->setText(columnDisplayData(_partsModel, current, PartsSqlQueryModel2::ColumnCustomPartNumber));
    ui->partCommentLabel->setText(columnDisplayData(_partsModel, current, PartsSqlQueryModel2::ColumnComment));
    ui->partCreateDateLabel->setText(columnDisplayDate(_partsModel, current, PartsSqlQueryModel2::ColumnCreateDate));
    ui->partStorageLocationLabel->setText(columnDisplayData(_partsModel, current, PartsSqlQueryModel2::ColumnStorage));
    ui->partConditionLabel->setText(columnDisplayData(_partsModel, current, PartsSqlQueryModel2::ColumnCondition));
    ui->partStockLevelUnitLabel->setText(columnDisplayData(_partsModel, current, PartsSqlQueryModel2::ColumnPartUnit));
    ui->partMinStockUnitLabel->setText(columnDisplayData(_partsModel, current, PartsSqlQueryModel2::ColumnPartUnit));
}

void PartManagerForm::updateStockView(const QModelIndex & current)
{
    if(!current.isValid()){
        ui->partStockHistoryTable->setModel(0);
        return;
    }

    QModelIndex primaryKeyIndex = _partsModel->index(current.row(), PartsSqlQueryModel2::ColumnId);
    QVariant keyValue = primaryKeyIndex.data(Qt::EditRole);
    qDebug()<<"Changing part to "<<keyValue;

    StockHistoryModel * model= new StockHistoryModel();
    model->setSelectedPart(keyValue);
    model->setHeaderData(0, Qt::Horizontal, QString());
    model->setHeaderData(1, Qt::Horizontal, tr("Date"));
    model->setHeaderData(2, Qt::Horizontal, tr("Amount"));
    model->setHeaderData(3, Qt::Horizontal, tr("Price"));
    model->setHeaderData(4, Qt::Horizontal, tr("Comment"));
    ui->partStockHistoryTable->setItemDelegateForColumn(StockHistoryModel::ColumnAction, new StockActionColumnDelegate());
    ui->partStockHistoryTable->setModel(model);
    ui->partStockHistoryTable->resizeColumnsToContents();
    ui->partStockHistoryTable->horizontalHeader()->setResizeMode(StockHistoryModel::ColumnAction, QHeaderView::Fixed);   
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
        ui->categoriesTreeView->expand(parentIndex);
        QModelIndex childIndex = _categoriesTreeModel->index(row, 0, parentIndex);
        ui->categoriesTreeView->selectionModel()->select(childIndex, QItemSelectionModel::ClearAndSelect);
    }
    else{
        _categoriesTreeModel->revert();
    }

}

void PartManagerForm::on_actionDeleteCategory_triggered()
{
    QModelIndex currentIdx = ui->categoriesTreeView->selectionModel()->currentIndex();
    if(!currentIdx.isValid())
        return;
    _categoriesTreeModel->removeItem(currentIdx);
}

void PartManagerForm::on_actionEditCategory_triggered()
{
    QModelIndex currentIdx = ui->categoriesTreeView->selectionModel()->currentIndex();
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

void PartManagerForm::appyFilter()
{
    //Filter by storage stuff starts
    bool filterByStorage = ui->filterByStorageCheck->isChecked();
    _filterBuilder.setFilterByStorage(filterByStorage);
    if(filterByStorage){
        int storageIdx = ui->filterByStorageCombo->currentIndex();
        QAbstractItemModel * model = ui->filterByStorageCombo->model();
        QVariant key = model->data(model->index(storageIdx,0));
        _filterBuilder.setSelectedStorage(key);
    }

    //Filter by category stuff starts
    QModelIndex currentCategory = ui->categoriesTreeView->currentIndex();

    if(ui->selectedCategoryScopeRadio->isChecked()){
        _filterBuilder.setCategoryFilteringMode(FilterBuilder::SelectedCategory);
        if(currentCategory.isValid()){
            int nodeId = _categoriesTreeModel->getItemId(currentCategory);
            _filterBuilder.setSelectedCategories(QVector<QVariant>(1,nodeId));
        }
        else{
            _filterBuilder.setSelectedCategories(QVector<QVariant>(0));
        }
    }
    else if(ui->allSubCategoriesScopeRadio->isChecked()){
        _filterBuilder.setCategoryFilteringMode(FilterBuilder::SubCategories);
        if(currentCategory.isValid()){
            int nodeId = _categoriesTreeModel->getItemId(currentCategory);
            _filterBuilder.setSelectedCategories( Entities::CategoriesDAO::listChildCategories(nodeId));
        }
        else{
            QVector<QVariant> empty = QVector<QVariant>(0);
            _filterBuilder.setSelectedCategories(empty);
        }
    }
    else{
        _filterBuilder.setCategoryFilteringMode(FilterBuilder::AllCategories);
    }

    //Filter by stock stuff starts
    if(ui->stockZeroRadio->isChecked()){
        _filterBuilder.setStockFilteringMode(FilterBuilder::StockLevelZero);
    }
    else if(ui->stockGreateZeroRadio->isChecked()){
        _filterBuilder.setStockFilteringMode(FilterBuilder::StockLevelGreaterZero);
    }
    else if(ui->stockBelowMinRadio->isChecked()){
        _filterBuilder.setStockFilteringMode(FilterBuilder::StockLevelBelowMin);
    }
    else{
        _filterBuilder.setStockFilteringMode(FilterBuilder::AnyStockLevel);
    }
    _filterBuilder.setTextFilter(ui->textSearchLineEdit->text());

    //Filter by date stuff starts
    int dateFilterComboIdx = ui->dateFilterOperatorCombo->currentIndex();
    FilterBuilder::DateFilteringMode dateFilterMode;
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
    _filterBuilder.setDateFilterMode(dateFilterMode);
    _filterBuilder.setSelectedDate(ui->dateFilterEdit->date());

    //Filter by condition stuff
    bool filterByCondition = ui->filterByConditionCheckbox->isChecked();
    _filterBuilder.setFilterByCondition(filterByCondition);
    if(filterByCondition){
        int conditionIdx = ui->filterByCondtitionCombo->currentIndex();
        QAbstractItemModel * model = ui->filterByCondtitionCombo->model();
        QVariant key = model->data(model->index(conditionIdx,0));
        _filterBuilder.setSelectedCondition(key);
    }

    //Create filter expression and apply it to the model
    _partsModel->setFilter(_filterBuilder.build());

    //Need to reselect if a cursor for a previous is open
    if(!_partsModel->query().isActive())
        _partsModel->select();
    //qDebug()<<"Query is "<<_partsModel->selectStatement();
}

void PartManagerForm::resetFilter()
{    
    ui->textSearchLineEdit->clear();
    ui->filterByStorageCheck->setChecked(false);
    ui->allSubCategoriesScopeRadio->setChecked(true);
    ui->stockAnyRadio->setChecked(true);
    ui->dateFilterOperatorCombo->setCurrentIndex(0);
    ui->dateFilterEdit->setDate(QDate::currentDate());
    ui->dateFilterEdit->setEnabled(false);
    ui->filterByConditionCheckbox->setChecked(false);
    appyFilter();
}

void PartManagerForm::on_filterButtonBox_clicked(QAbstractButton *button)
{
    QDialogButtonBox::StandardButton standardButton = ui->filterButtonBox->standardButton(button);
    switch(standardButton) {
        case QDialogButtonBox::Apply:
            appyFilter();     
        break;
    case QDialogButtonBox::Reset:
            resetFilter();
        break;
    default:
        break;
    }
}

void PartManagerForm::on_filterByStorageCombo_currentIndexChanged(int index)
{
}

void PartManagerForm::on_filterByStorageCheck_toggled(bool checked)
{
}

void PartManagerForm::on_filterByStorageCheck_stateChanged(int state)
{
    if(state==Qt::Unchecked){
        ui->filterByStorageCombo->setCurrentIndex(-1);
    }
}

void PartManagerForm::on_lineEdit_returnPressed()
{
}

void PartManagerForm::on_textSearchLineEdit_returnPressed()
{
    _filterBuilder.setTextFilter(ui->textSearchLineEdit->text());
    _partsModel->setFilter(_filterBuilder.build());
    if(!_partsModel->query().isActive())
        _partsModel->select();
}

void PartManagerForm::onAddStock()
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

void PartManagerForm::onRemoveStock()
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

void PartManagerForm::onEditPart()
{    
    QModelIndex index = ui->partsTableView->currentIndex();
    if(!index.isValid())
        return;
    PartDialog dlg(_partsModel, false, this);
    dlg.setWindowTitle(tr("Edit Part"));
    dlg.setCurrentModelIndex(index);
    dlg.exec();
}

void PartManagerForm::on_addPartButton_clicked()
{    
    int row = _partsModel->rowCount();
    _partsModel->database().transaction();
    if(_partsModel->insertRow(row)){
        QModelIndex rootIndex = _partsModel->index(row, PartsSqlQueryModel2::ColumnId);
        PartDialog dlg(_partsModel, true, this);
        dlg.setWindowTitle(tr("Add Part"));
        dlg.setCurrentModelIndex(rootIndex);
        if(dlg.exec()==QDialog::Accepted){
            _partsModel->submitAll();
            QVariant partId = _partsModel->lastInsertedId();
            qDebug()<<"Part id is "<<partId;
            if(dlg.initialStock()>0 && partId.isValid()){
                PartsDAO::addStock(partId, dlg.initialStock(), dlg.partPrice(), QString());
            }            
            _partsModel->database().commit();            
        }
        else{
            _partsModel->revertAll();
            _partsModel->database().rollback();
        }
    }
}

void PartManagerForm::on_pushButton_2_clicked()
{
}

void PartManagerForm::on_deletePartButton_clicked()
{
    if(!ui->partsTableView->selectionModel()->hasSelection())
        return;
    QModelIndexList selectedIndexes = ui->partsTableView->selectionModel()->selectedIndexes();
    for(int i=0; i< selectedIndexes.size(); ++i){
        _partsModel->removeRow(selectedIndexes[i].row());
    }
}

void PartManagerForm::part_primeInsert(int row, QSqlRecord &record)
{
    //Set the createDate field
    QDateTime now = QDateTime::currentDateTimeUtc();
    QVariant t = QVariant(now.toTime_t());
    record.setValue(PartsSqlQueryModel2::ColumnCreateDate, t);
    //The generated flag needs to be set or this field will not be included in the insert
    record.setGenerated(PartsSqlQueryModel2::ColumnCreateDate,true);

    //Set the category field using the category selected in the tree view
    QModelIndex currentCategoryIdx = ui->categoriesTreeView->selectionModel()->currentIndex();
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

void PartManagerForm::on_duplicateParteButton_clicked()
{
    QModelIndex index = ui->partsTableView->currentIndex();
    if(!index.isValid())
        return;
    QSqlRecord copy = _partsModel->record(index.row());
    copy.setNull(PartsSqlQueryModel2::ColumnId);
    copy.setNull(PartsSqlQueryModel2::ColumnActualStock);

    //Set the createDate field
    QDateTime now = QDateTime::currentDateTimeUtc();
    QVariant t = QVariant(now.toTime_t());
    copy.setValue(PartsSqlQueryModel2::ColumnCreateDate, t);

    _partsModel->database().transaction();
    int newRow = _partsModel->rowCount();
    if(_partsModel->insertRecord(newRow,copy)){
        QModelIndex rootIndex = _partsModel->index(newRow, PartsSqlQueryModel2::ColumnId);
        PartDialog dlg(_partsModel, true, this);
        dlg.setWindowTitle(tr("Add Part"));
        dlg.setCurrentModelIndex(rootIndex);
        if(dlg.exec()==QDialog::Accepted){
            _partsModel->submitAll();
            QVariant partId = _partsModel->lastInsertedId();
            qDebug()<<"Part id is "<<partId;
            if(dlg.initialStock()>0 && partId.isValid()){
                PartsDAO::addStock(partId, dlg.initialStock(), dlg.partPrice(), QString());
            }
            _partsModel->database().commit();
        }
        else{
            _partsModel->revertAll();
            _partsModel->database().rollback();
        }
    }
}

void PartManagerForm::on_expandTreeButton_clicked()
{
    ui->categoriesTreeView->expandAll();
}

void PartManagerForm::on_collapseTreeButton_clicked()
{
    ui->categoriesTreeView->expandToDepth(0);
}

void PartManagerForm::dateFilterOperatorCombo_indexChanged(int index)
{
    ui->dateFilterEdit->setEnabled(index>0);
}

