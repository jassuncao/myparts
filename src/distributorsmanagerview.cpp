#include "distributorsmanagerview.h"
#include "distributordetailswidget.h"
#include "widgets/itemstreeview.h"
#include "widgets/listnavigatorwidget.h"
#include "widgets/qsearchlineedit.h"
#include "styledbar.h"
#include "minisplitter.h"
#include <QHBoxLayout>
#include <QListView>
#include <QToolBar>
#include <QLabel>
#include <QPushButton>
#include <QFrame>
#include <QStringListModel>
#include <QStandardItemModel>
#include <QSqlTableModel>
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QStackedLayout>

DistributorsManagerView::DistributorsManagerView(QWidget *parent)
    : Manhattan::MiniSplitter(parent)
{
    _navigatorWidget = new  ListNavigatorWidget(tr("Distributors"));
    _contentLayout = new QStackedLayout;
    _detailsWidget = new  DistributorDetailsWidget;

    QLabel * notSelectedLabel = new QLabel(tr("Select a distributor"));
    notSelectedLabel->setAlignment(Qt::AlignCenter);
    _contentLayout->addWidget(notSelectedLabel);
    _contentLayout->addWidget(_detailsWidget);

    QVBoxLayout * contentLayout = new QVBoxLayout;
    contentLayout->setMargin(0);
    contentLayout->setSpacing(0);
    contentLayout->addWidget(new Manhattan::StyledBar);
    contentLayout->addLayout(_contentLayout);
    //contentLayout->addWidget(_detailsWidget);

    QWidget * rightSplitWidget = new QWidget;
    rightSplitWidget->setLayout(contentLayout);

    insertWidget(0,  _navigatorWidget);
    insertWidget(1,  rightSplitWidget);
    setStretchFactor(0, 0);
    setStretchFactor(1, 1);

    _distributorsTableModel = new QSqlTableModel(this);
    _distributorsTableModel->setTable("distributor");
    _distributorsTableModel->setSort(DistributorDetailsWidget::ColumnName, Qt::AscendingOrder);
    _distributorsTableModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    _distributorsTableModel->select();

    _navigatorWidget->setModel(_distributorsTableModel);
    _navigatorWidget->setModelColumn(DistributorDetailsWidget::ColumnName);
    _detailsWidget->setModel(_distributorsTableModel);
    _detailsWidget->setCurrentModelIndex(QModelIndex());

    connect(_navigatorWidget, SIGNAL(filterChanged(QString)), this, SLOT(slotFilterChanged(QString)));
    connect(_navigatorWidget, SIGNAL(itemSelected(QModelIndex)), this, SLOT(slotItemSelected(QModelIndex)));
    connect(_navigatorWidget, SIGNAL(addNewItem()), this, SLOT(slotAddDistributor()));
    connect(_navigatorWidget, SIGNAL(deleteItem(QModelIndex)), this, SLOT(slotDeleteDistributor(QModelIndex)));
    connect(_detailsWidget, SIGNAL(accepted()), this, SLOT(slotAccept()));
    connect(_detailsWidget, SIGNAL(deleted(QModelIndex)), this, SLOT(slotDeleteDistributor(QModelIndex)));
    connect(_detailsWidget, SIGNAL(rejected()), this, SLOT(slotReject()));
}

DistributorsManagerView::~DistributorsManagerView()
{
}

void DistributorsManagerView::slotAddDistributor()
{
    if(_detailsWidget->isNew()){
        return;
    }
    int row = _distributorsTableModel->rowCount();
    if(_distributorsTableModel->insertRow(row)){
        QModelIndex rootIndex = _distributorsTableModel->index(row, DistributorDetailsWidget::ColumnId);
        _detailsWidget->setCurrentModelIndex(rootIndex, true);
        _contentLayout->setCurrentIndex(1);
    }
}

void DistributorsManagerView::slotDeleteDistributor(const QModelIndex &index)
{
    qDebug()<<"Deleting distributor "<<index;
    if(index.isValid()){
        int row = index.row();
        _distributorsTableModel->removeRow(row);
        if(_distributorsTableModel->submitAll()){
            _navigatorWidget->setCurrentRow(row);
            //_navigatorWidget->setCurrentIndex(newIndex);
        }
    }
}

void DistributorsManagerView::slotFilterChanged(const QString &filterText)
{
    if(!filterText.isEmpty()){
        _distributorsTableModel->setFilter(QString("name LIKE '\%%1\%'").arg(filterText));
    }
    else{
        _distributorsTableModel->setFilter(QString::null);
        _distributorsTableModel->select();
    }
}

void DistributorsManagerView::slotAccept()
{
    //We keep the id of element being edited
    QVariant id;
    bool failedToSubmit;
    if(_detailsWidget->isNew()){
        failedToSubmit=_distributorsTableModel->submitAll()==false;
        id = _distributorsTableModel->query().lastInsertId();
    }
    else{
        id = _detailsWidget->currentModelIndex().data(Qt::EditRole);
        failedToSubmit=_distributorsTableModel->submitAll()==false;
    }
    if(failedToSubmit){
        qWarning()<<"Failed to submit changes ";
        qWarning("Reason is %s", qPrintable(_distributorsTableModel->lastError().text()));
    }
    /* submitAll resets the model making the indexes invalid.
     * We iterate over the records to find the row matching the current id
     */
    int row = findRowNumber(id);     
    _navigatorWidget->setCurrentRow(row);
    /*
    const QModelIndex & currentIndex = _distributorsTableModel->index(row,DistributorDetailsWidget::ColumnId);
    _navigatorWidget->setCurrentIndex(currentIndex);
    */
}

void DistributorsManagerView::slotReject()
{
    if(_detailsWidget->isNew()){
        int row = _detailsWidget->currentModelIndex().row();
        _distributorsTableModel->removeRow(row);
        _navigatorWidget->setCurrentRow(-1);
        //_navigatorWidget->setCurrentIndex(QModelIndex());
        slotItemSelected(QModelIndex());
    }
}

void DistributorsManagerView::slotItemSelected(const QModelIndex &index)
{
    qDebug()<<"Item selected "<<index;
    if(index.isValid()){        
        QModelIndex idIndex = _distributorsTableModel->index(index.row(), DistributorDetailsWidget::ColumnId);
        _detailsWidget->setCurrentModelIndex(idIndex);
        _contentLayout->setCurrentIndex(1);
    }
    else{
        _detailsWidget->setCurrentModelIndex(index);
        _contentLayout->setCurrentIndex(0);
    }
}

int DistributorsManagerView::findRowNumber(QVariant idValue)
{
    int row = 0;
    bool dataAvailable = true;
    while(dataAvailable){
        for(; row<_distributorsTableModel->rowCount(); ++row){
            const QModelIndex & idx = _distributorsTableModel->index(row, DistributorDetailsWidget::ColumnId);
            QVariant id = _distributorsTableModel->data(idx);
            if(id==idValue)
                return row;
        }
        dataAvailable = _distributorsTableModel->canFetchMore();
        if(dataAvailable){
            _distributorsTableModel->fetchMore();
        }
    }
    return -1;
}

