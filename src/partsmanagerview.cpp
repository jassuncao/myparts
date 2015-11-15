#include "partsmanagerview.h"
#include "minisplitter.h"
#include "styledbar.h"
#include "partdetailsview.h"
#include "widgets/currencydelegate.h"
#include "widgets/datetimedelegate.h"
#include "widgets/partstableview.h"
#include "category/categorytreemodel.h"
#include "models/storagetreemodel.h"
#include "widgets/partsfilterwidget.h"
#include "widgets/qactionpushbutton.h"
#include "models/partssqltablemodel.h"
#include "partdialog.h"
#include "addstockdialog.h"
#include "removestockdialog.h"
#include "parts/partsdao.h"

#include <QHBoxLayout>
#include <QPushButton>
#include <QTableView>
#include <QLabel>
#include <QAbstractItemView>
#include <QHeaderView>
#include <QSqlQuery>
#include <QMenu>
#include <QToolButton>
#include <QDebug>

PartsManagerView::PartsManagerView(QWidget *parent)
    : MiniSplitter(parent)      
{
    _partsModel = new PartsSqlTableModel(this);
    _partsModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    connect(_partsModel,SIGNAL(primeInsert(int,QSqlRecord&)), this, SLOT(slotPartsModelPrimeInsert(int,QSqlRecord&)));

    _categoriesTreeModel = new CategoryTreeModel(this);
    _categoriesTreeModel->select();

    _storageTreeModel = new StorageTreeModel(this);
    _storageTreeModel->select();

    _navWidget = new NavigationSubWidget(this);

    CategoryNavigator * categoryNavigator = new CategoryNavigator(_navWidget);
    categoryNavigator->setModel(_categoriesTreeModel);
    _navWidget->addNavigator(categoryNavigator);

    StorageNavigator * storageNavigator = new StorageNavigator(_navWidget);
    storageNavigator->setModel(_storageTreeModel);
    _navWidget->addNavigator(storageNavigator);
    _navWidget->setCurrentNavigator(0);    

    Manhattan::StyledBar * centerPaneTitleBar = new Manhattan::StyledBar;
    QLabel * centerPaneTitleLabel = new QLabel(tr("Parts List"), centerPaneTitleBar);
    QHBoxLayout *centerPaneTitleLayout = new QHBoxLayout;
    centerPaneTitleLayout->addSpacing(4);
    centerPaneTitleLayout->setMargin(0);
    centerPaneTitleLayout->setSpacing(0);
    centerPaneTitleLayout->addWidget(centerPaneTitleLabel);
    centerPaneTitleBar->setLayout(centerPaneTitleLayout);

    /*
    QMenu * duplicateBtnMenu = new QMenu(this);
    QAction * duplicateAllDataAction = duplicateBtnMenu->addAction(tr("Duplicate with all data"));
    duplicateBtnMenu->addAction(tr("Duplicate basic data only"));
    QAction * duplicateAction = new QAction(tr("Duplicate"), this);
    */

    QPushButton * addPartButton = new QPushButton(tr("Add part"), this);
    _deletePartButton = new QPushButton(tr("Delete part"), this);           
    _duplicatePartButton = new QPushButton(tr("Duplicate"), this);

    QHBoxLayout * partsActionsLayout = new QHBoxLayout;
    partsActionsLayout->setSpacing(6);
    partsActionsLayout->setMargin(6);    
    partsActionsLayout->addWidget(addPartButton);
    partsActionsLayout->addWidget(_deletePartButton);
    partsActionsLayout->addWidget(_duplicatePartButton);
    partsActionsLayout->addStretch();

    QFrame * hLine = new QFrame(this);
    hLine->setFrameShape(QFrame::HLine);
    hLine->setFrameShadow(QFrame::Sunken);

    _partsFilterWidget = new PartsFilterWidget(this);
    _partsFilterWidget->setPartsModel(_partsModel);

    _partsTableView = createPartsTableView(_partsModel);
    QVBoxLayout * centerLayout = new QVBoxLayout;
    centerLayout->setMargin(0);
    centerLayout->setSpacing(0);
    centerLayout->addWidget(centerPaneTitleBar);
    centerLayout->addLayout(partsActionsLayout);
    centerLayout->addWidget(hLine);
    centerLayout->addWidget(_partsFilterWidget);
    centerLayout->addWidget(_partsTableView);

    QWidget * centerPane = new QWidget(this);
    centerPane->setLayout(centerLayout);

    _partDetailsView = new PartDetailsView(this);
    _partDetailsView->setPartsModel(_partsModel);

    QVBoxLayout * rightLayout = new QVBoxLayout;
    rightLayout->setMargin(0);
    rightLayout->setSpacing(0);
    rightLayout->addWidget(new Manhattan::StyledBar);
    rightLayout->addWidget(_partDetailsView);

    QWidget * rightPane = new QWidget(this);
    rightPane->setLayout(rightLayout);

    addWidget(_navWidget);
    addWidget(centerPane);
    addWidget(rightPane);
    connect(_navWidget, SIGNAL(modeChanged(int)), this, SLOT(slotNavModeChanged(int)));
    connect(addPartButton, SIGNAL(clicked()), this, SLOT(slotAddPart()));
    connect(_deletePartButton, SIGNAL(clicked()), this, SLOT(slotDeletePart()));
    connect(_duplicatePartButton, SIGNAL(clicked()), this, SLOT(slotDuplicatePart()));
    connect(_partsFilterWidget, SIGNAL(filterChanged()), this, SLOT(slotFilterChanged()));
    connect(_partDetailsView, SIGNAL(addStockSelected()), this, SLOT(slotAddStock()));
    connect(_partDetailsView, SIGNAL(removeStockSelected()), this, SLOT(slotRemoveStock()));
    connect(_partDetailsView, SIGNAL(editPartSelected()), this, SLOT(slotEditPart()));
}

PartsManagerView::~PartsManagerView()
{
}

PartsTableView *PartsManagerView::createPartsTableView(QAbstractTableModel * tableModel){
    _partsTableView = new PartsTableView(this);
    _partsTableView->setModel(tableModel);
    //To enable in the future to allow editing stocks in place
    //_partsTableView->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed);
    connect(_partsTableView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
            this, SLOT(slotPartTableCurrentRowChanged(QModelIndex,QModelIndex)));
    connect(_partsTableView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(slotEditPart()));
    return _partsTableView;
}

void PartsManagerView::slotNavModeChanged(int mode)
{
    switch(mode){
        case NavigationSubWidget::Storage:
        //_navWidget->setModel(_storageTreeModel);
        break;
    default:
        //_navWidget->setModel(_categoriesTreeModel);
        break;
    }
}

void PartsManagerView::slotFilterChanged()
{

    _partsModel->select();
    //_partsModel->setFilter(_filterBuilder->build());
    /*
    if(!_partsModel->query().isActive())
        _partsModel->select();
        */
}

void PartsManagerView::slotPartTableCurrentRowChanged(const QModelIndex &current, const QModelIndex &)
{
    _partDetailsView->setCurrentIndex(current);
}

void PartsManagerView::slotAddPart()
{
    PartDialog dlg(_partsModel, _storageTreeModel, this);
    dlg.addNewPart();
}

void PartsManagerView::slotEditPart()
{
    QModelIndex index = _partsTableView->currentIndex();
    if(!index.isValid())
        return;
    PartDialog dlg(_partsModel, _storageTreeModel, this);
    dlg.editPart(index);
}

void PartsManagerView::slotDeletePart()
{

}

void PartsManagerView::slotDuplicatePart()
{
}

void PartsManagerView::slotPartsModelPrimeInsert(int, QSqlRecord &record)
{
    //Set the createDate field
    QDateTime now = QDateTime::currentDateTimeUtc();
    QVariant t = QVariant(now.toTime_t());
    record.setValue(PartsSqlTableModel::ColumnCreateDate, t);
    //The generated flag needs to be set or this field will not be included in the insert
    record.setGenerated(PartsSqlTableModel::ColumnCreateDate,true);

    //Set the category field using the category selected in the tree view
    /*
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
    */

    //Used to avoid an error in the sql statment generator
    record.setGenerated(PartsSqlTableModel::ColumnId,true);

    QSqlQuery q("SELECT id FROM part_unit WHERE defaultUnit=1");
    if(q.exec() && q.next()){
        record.setValue(PartsSqlTableModel::ColumnPartUnitId, q.value(0));
    }
    if(q.exec("SELECT id FROM part_condition WHERE defaultCondition=1")){
        if(q.next())
            record.setValue(PartsSqlTableModel::ColumnConditionId, q.value(0));
    }
}

void PartsManagerView::slotAddStock()
{
    QModelIndex index = _partsTableView->currentIndex();
    if(!index.isValid())
        return;
    QVariant partKey = _partsModel->data(_partsModel->index(index.row(),PartsSqlTableModel::ColumnId));
    QVariant partUnit = _partsModel->data(_partsModel->index(index.row(),PartsSqlTableModel::ColumnPartUnit));
    AddStockDialog dlg(this);
    dlg.setPartUnit(partUnit.toString());
    if(dlg.exec()!=QDialog::Accepted)
        return;
    if(PartsDAO::addStock(partKey, dlg.getStockChange(), dlg.getPartPrice(), dlg.getComment())){
        QModelIndex stockIdx = _partsModel->index(index.row(), PartsSqlTableModel::ColumnActualStock);
        QModelIndex priceIdx = _partsModel->index(index.row(), PartsSqlTableModel::ColumnAvgPrice);
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

void PartsManagerView::slotRemoveStock()
{
    QModelIndex index = _partsTableView->currentIndex();
    if(!index.isValid())
        return;
    QVariant partKey = _partsModel->data(_partsModel->index(index.row(),PartsSqlTableModel::ColumnId));
    QVariant partUnit = _partsModel->data(_partsModel->index(index.row(),PartsSqlTableModel::ColumnPartUnit));
    QModelIndex actualStockIdx = _partsModel->index(index.row(), PartsSqlTableModel::ColumnActualStock);
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
