#include "partsmanagerview.h"
#include "minisplitter.h"
#include "styledbar.h"
#include "partdetailsview.h"
#include "parts/partssqlquerymodel2.h"
#include "widgets/currencydelegate.h"
#include "widgets/datetimedelegate.h"
#include "widgets/partstableview.h"
#include "category/categorytreemodel.h"
#include "models/storagetreemodel.h"
#include "widgets/partsfilterwidget.h"
#include "widgets/qactionpushbutton.h"

#include <QHBoxLayout>
#include <QPushButton>
#include <QTableView>
#include <QLabel>
#include <QAbstractItemView>
#include <QHeaderView>
#include <QSqlQuery>
#include <QMenu>
#include <QToolButton>

PartsManagerView::PartsManagerView(QWidget *parent)
    : MiniSplitter(parent),
      _filterBuilder(new FilterBuilder())
{

    _partsModel = new PartsSqlQueryModel2(this);
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
    _partsFilterWidget->setFilterBuilder(_filterBuilder);

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

void PartsManagerView::slotPartTableCurrentRowChanged(const QModelIndex &current, const QModelIndex &)
{

}

void PartsManagerView::slotPartTableHeaderContextMenu(QPoint point)
{
}

void PartsManagerView::slotAddPart()
{

}

void PartsManagerView::slotEditPart()
{

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
    record.setValue(PartsSqlQueryModel2::ColumnCreateDate, t);
    //The generated flag needs to be set or this field will not be included in the insert
    record.setGenerated(PartsSqlQueryModel2::ColumnCreateDate,true);

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
