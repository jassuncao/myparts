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
#include "models/partssqltablemodel.h"
#include "partdialog.h"
#include "dialogs/addstockdialog.h"
#include "dialogs/removestockdialog.h"
#include "models/partsquerybuilder.h"
#include "models/categorytreemodel.h"
#include "models/treeitem.h"
#include "models/partstableproxymodel.h"

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

PartsManagerView::PartsManagerView(QWidget *parent)
    : MiniSplitter(parent)      
{
    _partsQueryBuilder = new PartsQueryBuilder();
    _partsModel = new PartsSqlTableModel(_partsQueryBuilder, this);
    _partsModel->setEditStrategy(QSqlTableModel::OnManualSubmit);        
    connect(_partsModel, SIGNAL(beforeSubmit()), this, SLOT(slotBeforeSubmit()));
    connect(_partsModel, SIGNAL(afterSubmit()), this, SLOT(slotAfterSubmit()));

    _partsTableProxyModel = new PartsTableProxyModel(this);

    _categoriesTreeModel = new CategoryTreeModel(this);
    _categoriesTreeModel->select();    

    _storageTreeModel = new StorageTreeModel(this);
    _storageTreeModel->select();

    _navWidget = new NavigationSubWidget(this);    

    _categoryNavigator = new CategoryNavigator(_navWidget);
    _categoryNavigator->setObjectName("CategoryNavigator");
    _categoryNavigator->setModel(_categoriesTreeModel);
    connect(_categoryNavigator, SIGNAL(selectionChanged(QList<int>)), this, SLOT(slotSelectedCategoryChanged(QList<int>)));
    _navWidget->addNavigator(_categoryNavigator);

    _storageNavigator = new StorageNavigator(_navWidget);
    _storageNavigator->setObjectName("StorageNavigator");
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
    connect(_partsFilterWidget, SIGNAL(filterChanged()), this, SLOT(slotFilterChanged()));    
    connect(_partDetailsView, SIGNAL(editPartSelected()), this, SLOT(slotEditPart()));
    connect(_categoriesTreeModel, SIGNAL(partsDropped(QVector<int>,TreeItem*)), this, SLOT(slotPartsDroppedInCategory(QVector<int>,TreeItem*)));
    connect(_storageTreeModel, SIGNAL(partsDropped(QVector<int>,TreeItem*)), this, SLOT(slotPartsDroppedInStorage(QVector<int>,TreeItem*)));

/*
    QLayout * layout = _navWidget->layout();
    int c = layout->count();
    for(int i=0; i<c; ++i){
        debugLayout("", layout->itemAt(i));
    }
    */
    _navWidget->setMinimumWidth(_navWidget->minimumSizeHint().width());
    _navWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
}

PartsManagerView::~PartsManagerView()
{
    if(_partsQueryBuilder){
        delete _partsQueryBuilder;        
    }
}

PartsTableView *PartsManagerView::createPartsTableView(QAbstractTableModel * tableModel){
    _partsTableView = new PartsTableView(this);
    _partsTableProxyModel->setSourceModel(tableModel);
    _partsTableView->setModel(_partsTableProxyModel);
    //To enable in the future to allow editing stocks in place
    //_partsTableView->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed);
    connect(_partsTableView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
            this, SLOT(slotPartTableCurrentRowChanged(QModelIndex,QModelIndex)));
    connect(_partsTableView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(slotEditPart()));
    connect(_partsTableView, SIGNAL(deletePressed()), this, SLOT(slotDeletePart()));
    return _partsTableView;
}

void PartsManagerView::slotBeforeSubmit()
{
    QModelIndex curr = _partsTableView->currentIndex();    
    if(curr.isValid()){
        QModelIndex sourceIndex = _partsTableProxyModel->mapToSource(curr);
        _savedPartId = _partsModel->data(_partsModel->index(sourceIndex.row(), PartsSqlTableModel::ColumnId), Qt::EditRole);
    }
}

void PartsManagerView::slotAfterSubmit()
{
    qDebug()<<"slotAfterSubmit";
    if(_savedPartId.isValid()){
        QModelIndex newIndex = _partsModel->findIndex(_savedPartId);
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
    _partDetailsView->setCurrentIndex(_partsTableProxyModel->mapToSource(current));

}

void PartsManagerView::slotAddPart()
{
    PartDialog dlg(_partsModel, _categoriesTreeModel, _storageTreeModel, this);
    dlg.setCurrentCategory(_categoryNavigator->currentCategory());
    dlg.setCurrentStorage(_storageNavigator->currentStorage());
    dlg.addNewPart();
}

void PartsManagerView::slotEditPart()
{
    QModelIndex index = _partsTableView->currentIndex();
    if(!index.isValid())
        return;   
    PartDialog dlg(_partsModel, _categoriesTreeModel, _storageTreeModel, this);    
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
    _partsModel->database().transaction();
    foreach (proxyIndex, selectedRows) {
        QModelIndex sourceIndex = _partsTableProxyModel->mapToSource(proxyIndex);
        _partsModel->removeRow(sourceIndex.row());
    }
    bool res = _partsModel->submitAll();
    if(res){
        _partsModel->database().commit();
    }
    else{
        qWarning()<<"Failed to delete parts. Reason:"<<_partsModel->database().lastError();
        _partsModel->database().rollback();
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
    PartDialog dlg(_partsModel, _categoriesTreeModel, _storageTreeModel, this);
    dlg.duplicatePart(_partsTableProxyModel->mapToSource(index), allData);
}


void PartsManagerView::slotPartsDroppedInCategory(QVector<int> parts, TreeItem* item)
{
    _partsModel->updatePartsCategory(parts, item->id());
    _partsModel->select();
}

void PartsManagerView::slotPartsDroppedInStorage(QVector<int> parts, TreeItem* item)
{
    _partsModel->updatePartsStorage(parts, item->id());
    _partsModel->select();
}
