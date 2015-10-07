#include "unitmanagerwidget.h"
#include "widgets/qsearchlineedit.h"
#include "unitdetailswidget.h"
#include "models/unittablemodel.h"
#include <QToolBar>
#include <QListView>
#include <QVBoxLayout>
#include <QSplitter>
#include <QDebug>

UnitManagerWidget::UnitManagerWidget(QWidget *parent) : QWidget(parent)
{
    QToolBar * toolbar = new QToolBar;
    toolbar->setIconSize(QSize(16,16));

    toolbar->addAction(QIcon(":/icons/footprint_add"),tr("Add Unit"), this, SLOT(slotAddItem()));
    toolbar->addAction(QIcon(":/icons/footprint_delete"),tr("Delete Unit"), this, SLOT(slotDeleteItem()));

    _searchLineEdit = new QSearchLineEdit;
    toolbar->addWidget(_searchLineEdit);
    connect(_searchLineEdit, SIGNAL(textClear()), this, SLOT(slotFilterChanged()));

    _listView = new QListView;
    _listView->setEditTriggers(QListView::NoEditTriggers);
    _listView->setSelectionBehavior(QListView::SelectRows);
    _listView->setSelectionMode(QListView::SingleSelection);

    QVBoxLayout *westBoxLayout = new QVBoxLayout;
    westBoxLayout->addWidget(toolbar);
    westBoxLayout->addWidget(_listView);
    QWidget * westWidgetWrapper = new QWidget;
    westWidgetWrapper->setLayout(westBoxLayout);

    _detailsWidget = new UnitDetailsWidget(this);

    QSplitter * hSplitter = new QSplitter(Qt::Horizontal);
    hSplitter->addWidget(westWidgetWrapper);
    hSplitter->addWidget(_detailsWidget);
    hSplitter->setStretchFactor(1,1);

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(hSplitter);
    setLayout(layout);

    connect(_searchLineEdit, SIGNAL(returnPressed()), this, SLOT(slotFilterChanged()));

    _tableModel = new UnitTableModel();
    _tableModel->setSort(UnitTableModel::ColumnName, Qt::AscendingOrder);
    _tableModel->setEditStrategy(QSqlTableModel::OnRowChange);
    _tableModel->select();

    _listView->setModel(_tableModel);
    _listView->setModelColumn(UnitTableModel::ColumnName);
    connect(_listView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)), this, SLOT(slotCurrentRowChanged(QModelIndex,QModelIndex)));

    _detailsWidget->setModel(_tableModel);
    _detailsWidget->setCurrentModelIndex(QModelIndex());

    connect(_detailsWidget, SIGNAL(accepted()), this, SLOT(slotEditorAccepted()));
    connect(_detailsWidget, SIGNAL(rejected()), this, SLOT(slotEditorRejected()));

}

UnitManagerWidget::~UnitManagerWidget()
{

}

void UnitManagerWidget::slotEditorAccepted()
{
}

void UnitManagerWidget::slotEditorRejected()
{
    if(_detailsWidget->isNew()){
        _tableModel->revertAll();               
        _detailsWidget->setCurrentModelIndex(_listView->currentIndex());
    }
}

void UnitManagerWidget::slotAddItem()
{
    if(_detailsWidget->isNew()){
        return;
    }
    int row = _tableModel->rowCount();
    if(_tableModel->insertRow(row)){        
        QModelIndex rootIndex = _tableModel->index(row, UnitTableModel::ColumnId);
        _detailsWidget->setCurrentModelIndex(rootIndex, true);
    }
}

void UnitManagerWidget::slotDeleteItem()
{
    if(_listView->selectionModel()){
        //XXX: Not the best way to handle this.
        if(_detailsWidget->isDirty()){
            _tableModel->revertAll();
        }
        QModelIndex index = _listView->selectionModel()->currentIndex();
        if(index.isValid()){
            int row = index.row();
            int column = index.column();
            if(_tableModel->removeRow(row)){
                QModelIndex newIndex = _tableModel->index(row-1, column);
                _listView->setCurrentIndex(newIndex);
            }            
        }
    }
}

void UnitManagerWidget::slotFilterChanged()
{
    if(!_searchLineEdit->text().isEmpty()){
        _tableModel->setFilter(QString("unit.name LIKE '\%%1\%'").arg(_searchLineEdit->text()));
    }
    else{
        _tableModel->setFilter(QString::null);
        _tableModel->select();
    }
}

void UnitManagerWidget::slotCurrentRowChanged(const QModelIndex &current, const QModelIndex &previous)
{    
    qDebug()<<"slotCurrentRowChanged(currentModel"<<_detailsWidget->currentModelIndex()
           <<" current "<<current <<" previous "<< previous;
    if(_detailsWidget->isDirty()){
        //XXX: Not the best way to handle this
        _tableModel->revertAll();
    }
    _detailsWidget->setCurrentModelIndex(current);   
}


