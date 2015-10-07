#include "distributormanagerwidget.h"
#include "distributordetailswidget.h"
#include <qsqltablemodel.h>
#include "widgets/qsearchlineedit.h"
#include <QToolBar>
#include <QVBoxLayout>
#include <QSplitter>
#include <QSqlTableModel>
#include <QListView>
#include <QDebug>


DistributorManagerWidget::DistributorManagerWidget(QWidget *parent) : QWidget(parent)
{
    QToolBar * toolbar = new QToolBar;
    toolbar->setIconSize(QSize(16,16));

    toolbar->addAction(QIcon(":/icons/distributor_add"),tr("Add Distributor"), this, SLOT(slotAddDistributor()));
    toolbar->addAction(QIcon(":/icons/distributor_delete"),tr("Delete Distributor"), this, SLOT(slotDeleteDistributor()));

    _searchLineEdit = new QSearchLineEdit;
    toolbar->addWidget(_searchLineEdit);
    connect(_searchLineEdit, SIGNAL(textClear()), this, SLOT(slotFilterChanged()));

    _distributorsListView = new QListView;
    _distributorsListView->setEditTriggers(QListView::NoEditTriggers);
    _distributorsListView->setSelectionBehavior(QListView::SelectRows);
    _distributorsListView->setSelectionMode(QListView::SingleSelection);

    QVBoxLayout *westBoxLayout = new QVBoxLayout;
    westBoxLayout->addWidget(toolbar);
    westBoxLayout->addWidget(_distributorsListView);
    QWidget * westWidgetWrapper = new QWidget;
    westWidgetWrapper->setLayout(westBoxLayout);

    _detailsWidget = new DistributorDetailsWidget(this);

    QSplitter * hSplitter = new QSplitter(Qt::Horizontal);
    hSplitter->addWidget(westWidgetWrapper);
    hSplitter->addWidget(_detailsWidget);
    hSplitter->setStretchFactor(1,1);

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(hSplitter);
    setLayout(layout);

    _distributorsTableModel = new QSqlTableModel(this);
    _distributorsTableModel->setTable("distributor");
    _distributorsTableModel->setSort(DistributorDetailsWidget::ColumnName, Qt::AscendingOrder);
    _distributorsTableModel->setEditStrategy(QSqlTableModel::OnRowChange);
    _distributorsTableModel->select();
    _distributorsListView->setModel(_distributorsTableModel);
    _distributorsListView->setModelColumn(DistributorDetailsWidget::ColumnName);

    _detailsWidget->setModel(_distributorsTableModel);
    _detailsWidget->setCurrentModelIndex(QModelIndex());

    connect(_searchLineEdit, SIGNAL(returnPressed()), this, SLOT(slotFilterChanged()));
    connect(_distributorsListView, SIGNAL(activated(QModelIndex)), this, SLOT(slotItemActivated(QModelIndex)));
    connect(_detailsWidget, SIGNAL(accepted()), this, SLOT(slotAccept()));
}

DistributorManagerWidget::~DistributorManagerWidget()
{
}

void DistributorManagerWidget::slotAddDistributor()
{
    if(_detailsWidget->isNew()){
        return;
    }
    int row = _distributorsTableModel->rowCount();
    if(_distributorsTableModel->insertRow(row)){
        QModelIndex rootIndex = _distributorsTableModel->index(row, DistributorDetailsWidget::ColumnId);
        _detailsWidget->setCurrentModelIndex(rootIndex, true);
    }
}

void DistributorManagerWidget::slotDeleteDistributor()
{
    if(_distributorsListView->selectionModel()){
        QModelIndex index = _distributorsListView->selectionModel()->currentIndex();
        if(index.isValid()){
            _detailsWidget->setCurrentModelIndex(QModelIndex());
            _distributorsTableModel->removeRow(index.row());
        }
    }
}

void DistributorManagerWidget::slotFilterChanged()
{
    if(!_searchLineEdit->text().isEmpty()){
        _distributorsTableModel->setFilter(QString("name LIKE '\%%1\%'").arg(_searchLineEdit->text()));
    }
    else{
        _distributorsTableModel->setFilter(QString::null);
        _distributorsTableModel->select();
    }
}

void DistributorManagerWidget::slotAccept()
{
    _detailsWidget->setCurrentModelIndex(QModelIndex());
    _distributorsListView->setCurrentIndex(QModelIndex());

}

void DistributorManagerWidget::slotReject()
{

}

void DistributorManagerWidget::slotItemActivated(const QModelIndex &index)
{
    qDebug()<<"slotItemActivated "<<index;
    if(index.isValid()){
        QModelIndex idIndex = _distributorsTableModel->index(index.row(), DistributorDetailsWidget::ColumnId);
        _detailsWidget->setCurrentModelIndex(idIndex);
    }
    else{
        _detailsWidget->setCurrentModelIndex(index);
    }
}

