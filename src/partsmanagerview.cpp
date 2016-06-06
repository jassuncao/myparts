#include "partsmanagerview.h"
#include "minisplitter.h"
#include "styledbar.h"
#include "partdetailsview.h"
#include "widgets/currencydelegate.h"
#include "widgets/datetimedelegate.h"
#include "widgets/partstableview.h"
#include "models/storagetreemodel.h"
#include "widgets/partsfilterwidget.h"
#include "widgets/qactionpushbutton.h"
#include "widgets/stockinlinedelegate.h"
#include "partdialog.h"
#include "dialogs/addstockdialog.h"
#include "dialogs/removestockdialog.h"
#include "models/partssqltablemodel.h"
#include "models/partsquerybuilder.h"
#include "models/categorytreemodel.h"
#include "models/treeitem.h"
#include "models/partstableproxymodel.h"
#include "models/modelsprovider.h"

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
#include <QMessageBox>
#include <QSqlError>


void debugLayout(QString tab, QLayoutItem * item){
    if(item->widget()){
        //qDebug()<<tab<<"Widget "<<item->widget()->objectName()<<" policy is "<<item->widget()->sizePolicy().horizontalPolicy();
        qDebug()<<tab<<"Widget "<<item->widget()->objectName()<<" width is "<<item->widget()->minimumSizeHint().width();

        QLayout * layout = item->widget()->layout();
        if(layout){
            int c = layout->count();
            for(int i=0; i<c; ++i){
                debugLayout(tab+"\t", layout->itemAt(i));
            }
        }
    }
    else if(item->layout()){
        QLayout * layout = item->layout();
        int c = layout->count();
        for(int i=0; i<c; ++i){
            debugLayout(tab+"\t", layout->itemAt(i));
        }
    }
}

PartsManagerView::PartsManagerView(ModelsProvider * modelsProvider, QWidget *parent)
    : MiniSplitter(parent),
      _modelsProvider(modelsProvider)
{
    /*
    _partsQueryBuilder = new PartsQueryBuilder();
    _partsModel = new PartsSqlTableModel(_partsQueryBuilder, this);
    _partsModel->setEditStrategy(QSqlTableModel::OnManualSubmit);        
    connect(_partsModel, SIGNAL(beforeSubmit()), this, SLOT(slotBeforeSubmit()));
    connect(_partsModel, SIGNAL(afterSubmit()), this, SLOT(slotAfterSubmit()));

    _categoriesTreeModel = new CategoryTreeModel(this);
    _categoriesTreeModel->select();

    _storageTreeModel = new StorageTreeModel(this);
    _storageTreeModel->select();
*/

    _partsTableProxyModel = new PartsTableProxyModel(this);
    PartsSqlTableModel * partsModel = _modelsProvider->partsModel();
    _partsTableProxyModel->setSourceModel(partsModel);

    _navWidget = new NavigationSubWidget(this);    

    _categoryNavigator = new CategoryNavigator(_navWidget);
    _categoryNavigator->setObjectName("CategoryNavigator");
    _categoryNavigator->setModel(_modelsProvider->partCategoryModel());

    connect(_categoryNavigator, SIGNAL(selectionChanged(QList<int>)), this, SLOT(slotSelectedCategoryChanged(QList<int>)));
    _navWidget->addNavigator(_categoryNavigator);    

    _storageNavigator = new StorageNavigator(_navWidget);
    _storageNavigator->setObjectName("StorageNavigator");
    _storageNavigator->setModel(_modelsProvider->partStorageModel());

    connect(_storageNavigator, SIGNAL(selectionChanged(QList<int>)), this, SLOT(slotSelectedStorageChanged(QList<int>)));
    _navWidget->addNavigator(_storageNavigator);
    _navWidget->setCurrentNavigator(0);

    _showDetailsPaneButton = new QToolButton;
    _showDetailsPaneButton->setIcon(QIcon(QLatin1String(":/icons/splitbutton_vertical")));
    _showDetailsPaneButton->setToolTip("Show details side bar");
    _showDetailsPaneButton->hide();


    Manhattan::StyledBar * centerPaneTitleBar = new Manhattan::StyledBar;
    QLabel * centerPaneTitleLabel = new QLabel(tr("Parts List"), centerPaneTitleBar);
    QHBoxLayout *centerPaneTitleLayout = new QHBoxLayout;
    centerPaneTitleLayout->addSpacing(4);
    centerPaneTitleLayout->setMargin(0);
    centerPaneTitleLayout->setSpacing(0);
    centerPaneTitleLayout->addWidget(centerPaneTitleLabel);
    centerPaneTitleLayout->addStretch();
    centerPaneTitleLayout->addWidget(_showDetailsPaneButton);
    centerPaneTitleBar->setLayout(centerPaneTitleLayout);

    QMenu * duplicateBtnMenu = new QMenu(this);
    duplicateBtnMenu->addAction(tr("Duplicate with all data"), this, SLOT(slotDuplicatePartAllData()));
    duplicateBtnMenu->addAction(tr("Duplicate basic data only"), this, SLOT(slotDuplicatePartBasicData()));

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
    _partsFilterWidget->setPartsQueryBuilder(partsModel->queryBuilder());

    _partsTableView = new PartsTableView(this);

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
    _partDetailsView->setPartsModel(partsModel);

    _hideDetailsPaneButton = new QToolButton;
    _hideDetailsPaneButton->setIcon(QIcon(QLatin1String(":/icons/splitbutton_closeright")));
    _hideDetailsPaneButton->setToolTip("Close details side bar");


    Manhattan::StyledBar * detailsTopBar = new Manhattan::StyledBar;
    QHBoxLayout *detailsTopBarLayout = new QHBoxLayout;
    detailsTopBarLayout->addSpacing(4);
    detailsTopBarLayout->setMargin(0);
    detailsTopBarLayout->setSpacing(0);
    detailsTopBarLayout->addStretch();
    detailsTopBarLayout->addWidget(_hideDetailsPaneButton);
    detailsTopBar->setLayout(detailsTopBarLayout);

    QVBoxLayout * rightLayout = new QVBoxLayout;
    rightLayout->setMargin(0);
    rightLayout->setSpacing(0);
    rightLayout->addWidget(detailsTopBar);
    rightLayout->addWidget(_partDetailsView);

    _detailsPane = new QWidget(this);
    _detailsPane->setLayout(rightLayout);

    addWidget(_navWidget);
    addWidget(centerPane);
    addWidget(_detailsPane);

    connect(addPartButton, SIGNAL(clicked()), this, SLOT(slotAddPart()));
    connect(_deletePartButton, SIGNAL(clicked()), this, SLOT(slotDeletePart()));    
    connect(_partsFilterWidget, SIGNAL(filterChanged()), this, SLOT(slotFilterChanged()));    
    connect(_partDetailsView, SIGNAL(editPartSelected()), this, SLOT(slotEditPart()));    
    connect(_hideDetailsPaneButton, SIGNAL(clicked(bool)), this, SLOT(slotHideDetailsPane()));
    connect(_showDetailsPaneButton, SIGNAL(clicked(bool)), this, SLOT(slotShowDetailsPane()));
/*
    QLayout * layout = _navWidget->layout();
    int c = layout->count();
    for(int i=0; i<c; ++i){
        debugLayout("", layout->itemAt(i));
    }
    */
    _navWidget->setMinimumWidth(_navWidget->minimumSizeHint().width());
    _navWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);

    initModels();
    initPartsTableView();
}

PartsManagerView::~PartsManagerView()
{    
}

void PartsManagerView::initModels()
{
    /*
    _categoryNavigator->setModel(_modelsProvider->partCategoryModel());
    _storageNavigator->setModel(_modelsProvider->storageTreeModel());

    PartsSqlTableModel * partsModel = _modelsProvider->partsModel();
    _partsFilterWidget->setPartsQueryBuilder(partsModel->queryBuilder());
    _partsTableProxyModel->setSourceModel(partsModel);
    _partDetailsView->setPartsModel(partsModel);
    */
}



void PartsManagerView::initPartsTableView(){
    _partsTableView->setModel(_partsTableProxyModel);
    _partsTableView->setItemDelegateForColumn(PartsSqlTableModel::ColumnActualStock, new StockInlineDelegate(this));
    //To enable in the future to allow editing stocks in place
    _partsTableView->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed | QAbstractItemView::AnyKeyPressed);
    connect(_partsTableView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
            this, SLOT(slotPartTableCurrentRowChanged(QModelIndex,QModelIndex)));
    connect(_partsTableView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(slotEditPart()));
    connect(_partsTableView, SIGNAL(deletePressed()), this, SLOT(slotDeletePart()));    
}

void PartsManagerView::slotShowDetailsPane()
{
    _detailsPane->show();
    _showDetailsPaneButton->hide();
}

void PartsManagerView::slotHideDetailsPane()
{
    _detailsPane->hide();
    _showDetailsPaneButton->show();
}

void PartsManagerView::slotBeforeSubmit()
{
    QModelIndex curr = _partsTableView->currentIndex();    
    if(curr.isValid()){
        QModelIndex sourceIndex = _partsTableProxyModel->mapToSource(curr);
        _savedPartId = _partsTableProxyModel->sourceModel()->index(sourceIndex.row(), PartsSqlTableModel::ColumnId).data(Qt::EditRole);
        //_savedPartId = _partsModel->data(_partsModel->index(sourceIndex.row(), PartsSqlTableModel::ColumnId), Qt::EditRole);
    }
}

void PartsManagerView::slotAfterSubmit()
{
    qDebug()<<"slotAfterSubmit";
    if(_savedPartId.isValid()){
        PartsSqlTableModel* partsModel = static_cast<PartsSqlTableModel*>(_partsTableProxyModel->sourceModel());
        QModelIndex newIndex = partsModel->findIndex(_savedPartId);
        QModelIndex proxyIndex = _partsTableProxyModel->mapFromSource(newIndex);
        _savedPartId = QVariant();
        _partsTableView->selectionModel()->setCurrentIndex(proxyIndex, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
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
    _modelsProvider->partsModel()->select();
    //_partsModel->select();
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
    PartsSqlTableModel* partsModel = static_cast<PartsSqlTableModel*>(_partsTableProxyModel->sourceModel());
    PartsQueryBuilder * queryBuilder = partsModel->queryBuilder();
    queryBuilder->setFilter(PartsQueryBuilder::FilterByCategory, QVariant::fromValue(value));
    queryBuilder->setFilter(PartsQueryBuilder::FilterByStorage, QVariant::fromValue(NodeCriterionValue()));
    partsModel->select();
    //slotFilterChanged();
}

QVariant PartsManagerView::selectedCategory() const
{
    return _categoryNavigator->currentCategory();
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
    PartsSqlTableModel* partsModel = static_cast<PartsSqlTableModel*>(_partsTableProxyModel->sourceModel());
    PartsQueryBuilder * queryBuilder = partsModel->queryBuilder();

    queryBuilder->setFilter(PartsQueryBuilder::FilterByStorage, QVariant::fromValue(value));
    queryBuilder->setFilter(PartsQueryBuilder::FilterByCategory, QVariant::fromValue(NodeCriterionValue()));
    partsModel->select();
    //slotFilterChanged();
}

QVariant PartsManagerView::selectedStorage() const
{
    return _storageNavigator->currentStorage();
}

void PartsManagerView::slotPartTableCurrentRowChanged(const QModelIndex &current, const QModelIndex &previous)
{
    qDebug()<<"Current is "<<current<<" Previous is "<<previous;
    _partDetailsView->setCurrentIndex(_partsTableProxyModel->mapToSource(current));

}

void PartsManagerView::slotAddPart()
{
    PartDialog dlg(_modelsProvider, this);
    dlg.setCurrentCategory(selectedCategory());
    dlg.setCurrentStorage(selectedStorage());
    dlg.addNewPart();
}

void PartsManagerView::slotEditPart()
{
    QModelIndex index = _partsTableView->currentIndex();
    if(!index.isValid() || index.column()==PartsSqlTableModel::ColumnActualStock)
        return;   
    PartDialog dlg(_modelsProvider, this);
    dlg.editPart(_partsTableProxyModel->mapToSource(index));
}

void PartsManagerView::slotDeletePart()
{
    QModelIndexList selectedRows = _partsTableView->selectionModel()->selectedRows(PartsSqlTableModel::ColumnId);
    if(selectedRows.size()==0)
        return;
    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Question);
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::Yes);
    if(selectedRows.size()==1)
        msgBox.setText("Do you really wish to delete the selected part?");
    else
        msgBox.setText("Do you really wish to delete the selected parts?");
    if(msgBox.exec()!=QMessageBox::Yes)
        return;

    QModelIndex proxyIndex;
    PartsSqlTableModel* partsModel = static_cast<PartsSqlTableModel*>(_partsTableProxyModel->sourceModel());
    partsModel->database().transaction();
    foreach (proxyIndex, selectedRows) {
        QModelIndex sourceIndex = _partsTableProxyModel->mapToSource(proxyIndex);
        partsModel->removeRow(sourceIndex.row());
    }
    bool res = partsModel->submitAll();
    if(res){
        partsModel->database().commit();
    }
    else{
        qWarning()<<"Failed to delete parts. Reason:"<<partsModel->database().lastError();
        partsModel->database().rollback();
    }

}

void PartsManagerView::slotDuplicatePartAllData()
{
    duplicatePart(true);
}

void PartsManagerView::slotDuplicatePartBasicData()
{
    duplicatePart(false);
}

void PartsManagerView::duplicatePart(bool allData)
{
    QModelIndex index = _partsTableView->currentIndex();
    if(!index.isValid())
        return;
    PartDialog dlg(_modelsProvider, this);
    dlg.duplicatePart(_partsTableProxyModel->mapToSource(index), allData);
}

