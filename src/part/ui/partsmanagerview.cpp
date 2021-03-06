#include "partsmanagerview.h"
#include "styledbar.h"
#include "partdetailsview.h"
#include "widgets/currencydelegate.h"
#include "widgets/datetimedelegate.h"
#include "widgets/partstableview.h"
#include "part/model/storagetreemodel.h"
#include "widgets/partsfilterwidget.h"
#include "widgets/qactionpushbutton.h"
#include "widgets/stockinlinedelegate.h"
#include "widgets/navigationsubwidget.h"
#include "partdialog.h"
#include "stock/addstockdialog.h"
#include "dialogs/removestockdialog.h"
#include "part/model/partssqltablemodel.h"
#include "part/model/partsquerybuilder.h"
#include "part/model/categorytreemodel.h"
#include "core/treemodel/treeitem.h"
#include "part/model/partstableproxymodel.h"
#include "models/modelsrepository.h"
#include "utils.h"
#include "dialogs/tableexportdialog.h"
#include "core/csv/csvexporter.h"
#include "part/merge/mergepartsdialog.h"

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
#include <QSettings>
#include <QFileDialog>
#include <QApplication>

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

PartsManagerView::PartsManagerView(ModelsRepository * modelsProvider, QWidget *parent)
    : MiniSplitter(parent),
      _modelsRepository(modelsProvider)
{
    _partsTableProxyModel = new PartsTableProxyModel(this);    
    _partsTableProxyModel->setSourceModel(_modelsRepository->partsModel());
    _navWidget = new NavigationSubWidget(this);    

    _categoryNavigator = new CategoryNavigator(_modelsRepository, _navWidget);
    _categoryNavigator->setObjectName("CategoryNavigator");
    _categoryNavigator->setModel(_modelsRepository->partCategoryModel());

    connect(_categoryNavigator, SIGNAL(selectionChanged(QList<int>)), this, SLOT(slotSelectedCategoryChanged(QList<int>)));
    _navWidget->addNavigator(_categoryNavigator);    

    _storageNavigator = new StorageNavigator(_modelsRepository, _navWidget);
    _storageNavigator->setObjectName("StorageNavigator");
    _storageNavigator->setModel(_modelsRepository->partStorageModel());

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

    QPushButton * exportButton = new QPushButton(QIcon(QString::fromLatin1(":/icons/table_export")), tr("Export"), this);

    QHBoxLayout * partsActionsLayout = new QHBoxLayout;
    partsActionsLayout->setSpacing(6);
    partsActionsLayout->setMargin(6);    
    partsActionsLayout->addWidget(addPartButton);
    partsActionsLayout->addWidget(_deletePartButton);
    partsActionsLayout->addWidget(_duplicatePartButton);
    partsActionsLayout->addStretch();
    partsActionsLayout->addWidget(exportButton);

    QFrame * hLine = new QFrame(this);
    hLine->setFrameShape(QFrame::HLine);
    hLine->setFrameShadow(QFrame::Sunken);

    _partsFilterWidget = new PartsFilterWidget(_modelsRepository, this);
    _partsFilterWidget->setPartsQueryBuilder(_modelsRepository->partsModel()->queryBuilder());

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
    _partDetailsView->setModelsRepository(_modelsRepository);

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
    connect(exportButton, SIGNAL(clicked()), this, SLOT(slotExportTable()));
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

    initPartsTableView();
    initContextMenuActions();
    connect(_modelsRepository->partStorageModel(), SIGNAL(partsDropped(QVector<int>,TreeItem*)), this, SLOT(slotPartsDroppedInStorage(QVector<int>,TreeItem*)));

}

PartsManagerView::~PartsManagerView()
{    
}

void PartsManagerView::initPartsTableView(){
    _partsTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    //_partsTableView->setModel(_modelsProvider->partsModel());    
    _partsTableView->setModel(_partsTableProxyModel);
    //Disable the stock inline feature...
    //_partsTableView->setItemDelegateForColumn(PartsSqlTableModel::ColumnActualStock, new StockInlineDelegate(this));
    _partsTableView->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed | QAbstractItemView::AnyKeyPressed);
    connect(_partsTableView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
            this, SLOT(slotPartTableCurrentRowChanged(QModelIndex,QModelIndex)));
    connect(_partsTableView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(slotEditPart()));
    connect(_partsTableView, SIGNAL(deletePressed()), this, SLOT(slotDeletePart()));    
    connect(_partsTableView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(slotSelectionChanged(QItemSelection,QItemSelection)));
    _partsTableView->setContextMenuPolicy(Qt::ActionsContextMenu);

}

void PartsManagerView::slotSelectionChanged(const QItemSelection &, const QItemSelection &)
{

    int selectedCount = _partsTableView->selectionModel()->selectedRows().size();
    _actionMergeParts->setEnabled(selectedCount == 2);
}

void PartsManagerView::initContextMenuActions()
{

    QIcon mergePartsIcon(QString::fromLatin1(":/icons/join"));
    _actionMergeParts = new QAction(mergePartsIcon, tr("Merge parts"), this);
    connect(_actionMergeParts, SIGNAL(triggered(bool)), this, SLOT(slotMergeParts()));
    _partsTableView->addAction(_actionMergeParts);
    _actionMergeParts->setEnabled(false);
}

void PartsManagerView::writeSettings(QSettings& settings) const
{
    settings.setValue("parts/splitterSizes", saveState());
    bool showDetails = _detailsPane->isVisible();
    settings.setValue("parts/showDetails", QVariant(showDetails));
}

void PartsManagerView::readSettings(QSettings& settings)
{
    QVariant var = settings.value("parts/splitterSizes");
    if(var.isValid()){
        restoreState(var.toByteArray());
    }
    var = settings.value("parts/showDetails");
    bool b = true;
    if(var.isValid()){
        b = var.toBool();
    }
    if(b){
        slotShowDetailsPane();
    }
    else{
        slotHideDetailsPane();
    }
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
    _modelsRepository->partsModel()->select();
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
}

QVariant PartsManagerView::selectedStorage() const
{
    return _storageNavigator->currentStorage();
}

void PartsManagerView::slotPartTableCurrentRowChanged(const QModelIndex &current, const QModelIndex &)
{    
    _partDetailsView->setCurrentIndex(_partsTableProxyModel->mapToSource(current));

}

void PartsManagerView::slotAddPart()
{
    PartDialog dlg(_modelsRepository, this);
    dlg.setCurrentCategory(selectedCategory());
    dlg.setCurrentStorage(selectedStorage());
    dlg.addNewPart();
}

void PartsManagerView::slotEditPart()
{
    QModelIndex index = _partsTableView->currentIndex();
    if(!index.isValid() || index.column()==PartsSqlTableModel::ColumnTotalStock)
        return;   
    PartDialog dlg(_modelsRepository, this);
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
    PartDialog dlg(_modelsRepository, this);
    dlg.duplicatePart(_partsTableProxyModel->mapToSource(index), allData);
}

void PartsManagerView::slotPartTableContextMenuRequested(const QPoint &pos)
{
    const QModelIndex & index = _partsTableView->indexAt(pos);
    if(!index.isValid()){
        return;
    }
    const QPoint & globalPos = _partsTableView->mapToGlobal(pos);
    _partsTableView->selectionModel()->selectedRows();
    _contextActionsMenu->exec(globalPos);
}

void PartsManagerView::slotMergeParts()
{
    QModelIndexList indexList = _partsTableView->selectionModel()->selectedRows(PartsSqlTableModel::ColumnId);
    if(indexList.size()<2){
        return;
    }
    QVariant partA = indexList.at(0).data(Qt::EditRole);
    QVariant partB = indexList.at(1).data(Qt::EditRole);

    MergePartsDialog dlg(this);
    dlg.setParts(partA, partB);
    int res = dlg.exec();
    if(res){
        _modelsRepository->partsModel()->select();
    }
}

void PartsManagerView::slotExportTable()
{
    TableExportDialog dlg(this);
    int res = dlg.exec();
    if(QDialog::Accepted == res){       
        CsvExporter csvExporter(dlg.includeHeader(), dlg.fieldDelimiter(), dlg.charsetName());
        csvExporter.setTable(_partsTableView);
        csvExporter.toCSV(dlg.filename());
        QMessageBox::information(this, tr("File saved"), tr("CSV file saved"));
    }
}

void PartsManagerView::slotPartsDroppedInStorage(QVector<int> parts, TreeItem* item)
{
    int res = QMessageBox::information(this, tr("Confirmation"), tr("This action will move all the stock to the same stock location.\n Are you sure ?"), QMessageBox::Yes, QMessageBox::No);
    if(res & (QMessageBox::Ok | QMessageBox::Yes)){
        _modelsRepository->partsDroppedInStorage(parts, item);
    }
}

