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
#include "dialogs/addstockdialog.h"
#include "dialogs/removestockdialog.h"
#include "parts/partsdao.h"
#include "models/partsquerybuilder.h"

#include <QHBoxLayout>
#include <QPushButton>
#include <QTableView>
#include <QLabel>
#include <QAbstractItemView>
#include <QHeaderView>
#include <QSqlQuery>
#include <QMenu>
#include <QToolButton>
#include <QIcon>
#include <QDebug>

PartsManagerView::PartsManagerView(QWidget *parent)
    : MiniSplitter(parent)      
{
    _partsQueryBuilder = new PartsQueryBuilder();
    _partsModel = new PartsSqlTableModel(_partsQueryBuilder, this);
    _partsModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    connect(_partsModel,SIGNAL(primeInsert(int,QSqlRecord&)), this, SLOT(slotPartsModelPrimeInsert(int,QSqlRecord&)));
    connect(_partsModel, SIGNAL(beforeSubmit()), this, SLOT(slotBeforeSubmit()));
    connect(_partsModel, SIGNAL(afterSubmit()), this, SLOT(slotAfterSubmit()));
    //connect(_partsModel, SIGNAL(layoutAboutToBeChanged()), this, SLOT(slotLayoutAboutToBeChanged()));
    //connect(_partsModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(slotDataChanged(QModelIndex,QModelIndex)));

    _categoriesTreeModel = new CategoryTreeModel(this);
    _categoriesTreeModel->select();

    _storageTreeModel = new StorageTreeModel(this);
    _storageTreeModel->select();

    _navWidget = new NavigationSubWidget(this);

    _categoryNavigator = new CategoryNavigator(_navWidget);
    _categoryNavigator->setModel(_categoriesTreeModel);
    connect(_categoryNavigator, SIGNAL(selectionChanged(QList<int>)), this, SLOT(slotSelectedCategoryChanged(QList<int>)));
    _navWidget->addNavigator(_categoryNavigator);

    _storageNavigator = new StorageNavigator(_navWidget);
    _storageNavigator->setModel(_storageTreeModel);
    connect(_storageNavigator, SIGNAL(selectionChanged(QList<int>)), this, SLOT(slotSelectedStorageChanged(QList<int>)));
    _navWidget->addNavigator(_storageNavigator);
    _navWidget->setCurrentNavigator(0);

    Manhattan::StyledBar * centerPaneTitleBar = new Manhattan::StyledBar;
    QLabel * centerPaneTitleLabel = new QLabel(tr("Parts List"), centerPaneTitleBar);
    QHBoxLayout *centerPaneTitleLayout = new QHBoxLayout;
    centerPaneTitleLayout->addSpacing(4);
    centerPaneTitleLayout->setMargin(0);
    centerPaneTitleLayout->setSpacing(0);
    centerPaneTitleLayout->addWidget(centerPaneTitleLabel);
    centerPaneTitleBar->setLayout(centerPaneTitleLayout);

    QMenu * duplicateBtnMenu = new QMenu(this);
    duplicateBtnMenu->addAction(tr("Duplicate with all data"));
    duplicateBtnMenu->addAction(tr("Duplicate basic data only"));    

    QPushButton * addPartButton = new QPushButton(QIcon(QString::fromLatin1(":/icons/addStock")), tr("Add part"), this);
    _deletePartButton = new QPushButton(QIcon(QString::fromLatin1(":/icons/removeStock")), tr("Delete part"), this);
    _duplicatePartButton = new QPushButton(QIcon(QString::fromLatin1(":/icons/duplicatePart")), tr("Duplicate"), this);
    _duplicatePartButton->setMenu(duplicateBtnMenu);

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
    _partsFilterWidget->setPartsQueryBuilder(_partsQueryBuilder);

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
    connect(addPartButton, SIGNAL(clicked()), this, SLOT(slotAddPart()));
    connect(_deletePartButton, SIGNAL(clicked()), this, SLOT(slotDeletePart()));
    connect(_duplicatePartButton, SIGNAL(clicked()), this, SLOT(slotDuplicatePart()));
    connect(_partsFilterWidget, SIGNAL(filterChanged()), this, SLOT(slotFilterChanged()));    
    connect(_partDetailsView, SIGNAL(editPartSelected()), this, SLOT(slotEditPart()));      
}

PartsManagerView::~PartsManagerView()
{
    if(_partsQueryBuilder){
        delete _partsQueryBuilder;        
    }
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

void PartsManagerView::slotBeforeSubmit()
{
    QModelIndex curr = _partsTableView->currentIndex();
    if(curr.isValid()){
        _savedPartId = _partsModel->data(_partsModel->index(curr.row(), PartsSqlTableModel::ColumnId), Qt::EditRole);
    }
}

void PartsManagerView::slotAfterSubmit()
{
    qDebug()<<"slotAfterSubmit";
    if(_savedPartId.isValid()){
        QModelIndex newIndex = _partsModel->findIndex(_savedPartId);
        _savedPartId = QVariant();
        _partsTableView->selectionModel()->setCurrentIndex(newIndex, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
    }
}

void PartsManagerView::slotNavModeChanged(int mode)
{
    switch(mode){
        case NavigationSubWidget::Storage:
        break;
    default:
        break;
    }
}

void PartsManagerView::slotFilterChanged()
{
    _partsModel->select();    
}

void PartsManagerView::slotSelectedCategoryChanged(const QList<int> selectedIds)
{
    NodeCriterionValue::Mode mode;
    if(selectedIds.size()>1){
        mode = NodeCriterionValue::IncludeNodeChilds;
    }
    else if(selectedIds.size()==1)
    {
        mode = NodeCriterionValue::SelectedNode;
    }
    else{
        mode = NodeCriterionValue::All;
    }
    NodeCriterionValue value(mode, selectedIds);
    _partsQueryBuilder->setFilter(PartsQueryBuilder::FilterByCategory, QVariant::fromValue(value));
    _partsQueryBuilder->setFilter(PartsQueryBuilder::FilterByStorage, QVariant::fromValue(NodeCriterionValue()));
    slotFilterChanged();
}

void PartsManagerView::slotSelectedStorageChanged(const QList<int> selectedIds)
{
    NodeCriterionValue::Mode mode;
    if(selectedIds.size()>1){
        mode = NodeCriterionValue::IncludeNodeChilds;
    }
    else if(selectedIds.size()==1)
    {
        mode = NodeCriterionValue::SelectedNode;
    }
    else{
        mode = NodeCriterionValue::All;
    }
    NodeCriterionValue value(mode, selectedIds);
    _partsQueryBuilder->setFilter(PartsQueryBuilder::FilterByStorage, QVariant::fromValue(value));
    _partsQueryBuilder->setFilter(PartsQueryBuilder::FilterByCategory, QVariant::fromValue(NodeCriterionValue()));
    slotFilterChanged();
}

void PartsManagerView::slotPartTableCurrentRowChanged(const QModelIndex &current, const QModelIndex &previous)
{
    qDebug()<<"Current is "<<current<<" Previous is "<<previous;   
    _partDetailsView->setCurrentIndex(current);

}

void PartsManagerView::slotAddPart()
{
    PartDialog dlg(_partsModel, _categoriesTreeModel, _storageTreeModel, this);
    dlg.addNewPart();
}

void PartsManagerView::slotEditPart()
{
    QModelIndex index = _partsTableView->currentIndex();
    if(!index.isValid())
        return;
    PartDialog dlg(_partsModel, _categoriesTreeModel, _storageTreeModel, this);
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
    QModelIndex currentCategoryIdx = _categoryNavigator->currentIndex();
    int categoryId;
    if(currentCategoryIdx.isValid()){
        categoryId = _categoriesTreeModel->getItemId(currentCategoryIdx);
    }
    else{
        categoryId = _categoriesTreeModel->rootItemId();
    }
    record.setValue(PartsSqlTableModel::ColumnCategoryId, categoryId);
    record.setGenerated(PartsSqlTableModel::ColumnCategoryId,true);

    //Set the storage field using the storage selected in the tree view
    QModelIndex currentStorageIdx = _storageNavigator->currentIndex();
    int storageId;
    if(currentStorageIdx.isValid()){
        storageId = _storageTreeModel->getItemId(currentStorageIdx);
    }
    else{
        storageId = _storageTreeModel->rootItemId();
    }
    record.setValue(PartsSqlTableModel::ColumnStorageId, storageId);
    record.setGenerated(PartsSqlTableModel::ColumnStorageId,true);

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
