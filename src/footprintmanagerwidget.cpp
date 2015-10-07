#include "footprintmanagerwidget.h"
#include "widgets/qsearchlineedit.h"
#include "footprintdetailswidget.h"
#include "models/footprinttablemodel.h"
#include <QVBoxLayout>
#include <QLineEdit>
#include <QToolBar>
#include <QListView>
#include <QSplitter>
#include <QDebug>

FootprintManagerWidget::FootprintManagerWidget(QWidget *parent) : QWidget(parent)
{

    QToolBar * toolbar = new QToolBar;
    toolbar->setIconSize(QSize(16,16));

    toolbar->addAction(QIcon(":/icons/footprint_add"),tr("Add Footprint"), this, SLOT(slotAddFootprint()));
    toolbar->addAction(QIcon(":/icons/footprint_delete"),tr("Delete Footprint"), this, SLOT(slotDeleteFootprint()));

    _searchLineEdit = new QSearchLineEdit;
    toolbar->addWidget(_searchLineEdit);
    connect(_searchLineEdit, SIGNAL(textClear()), this, SLOT(slotFilterChanged()));

    _footprintsListView = new QListView;
    _footprintsListView->setEditTriggers(QListView::NoEditTriggers);
    _footprintsListView->setSelectionBehavior(QListView::SelectRows);
    _footprintsListView->setSelectionMode(QListView::SingleSelection);

    QVBoxLayout *westBoxLayout = new QVBoxLayout;
    westBoxLayout->addWidget(toolbar);
    westBoxLayout->addWidget(_footprintsListView);
    QWidget * westWidgetWrapper = new QWidget;
    westWidgetWrapper->setLayout(westBoxLayout);

    _detailsWidget = new FootprintDetailsWidget(this);   

    QSplitter * hSplitter = new QSplitter(Qt::Horizontal);
    hSplitter->addWidget(westWidgetWrapper);
    hSplitter->addWidget(_detailsWidget);
    hSplitter->setStretchFactor(1,1);

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(hSplitter);
    setLayout(layout);    

    _footprintsTableModel = new FootprintTableModel(this);
    _footprintsTableModel->setSort(FootprintTableModel::ColumnName, Qt::AscendingOrder);
    _footprintsTableModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    _footprintsTableModel->select();

    _footprintsListView->setModel(_footprintsTableModel);
    _footprintsListView->setModelColumn(FootprintTableModel::ColumnName);

    _detailsWidget->setModel(_footprintsTableModel);
    _detailsWidget->setCurrentModelIndex(QModelIndex());

    connect(_searchLineEdit, SIGNAL(returnPressed()), this, SLOT(slotFilterChanged()));
    connect(_footprintsListView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)), this, SLOT(slotCurrentRowChanged(QModelIndex,QModelIndex)));
}

FootprintManagerWidget::~FootprintManagerWidget()
{
    delete _footprintsTableModel;
}

void FootprintManagerWidget::slotCurrentRowChanged(const QModelIndex &current, const QModelIndex &)
{
    if(!current.isValid()){
        return;
    }
    QModelIndex footprintIdIndex =_footprintsTableModel->index(current.row(), FootprintTableModel::ColumnId);
    _detailsWidget->setCurrentModelIndex(footprintIdIndex);
}

void FootprintManagerWidget::slotAddFootprint()
{
    if(_detailsWidget->isNew()){
        return;
    }
    int row = _footprintsTableModel->rowCount();    
    if(_footprintsTableModel->insertRow(row)){
        QModelIndex rootIndex = _footprintsTableModel->index(row, FootprintTableModel::ColumnId);
        _detailsWidget->setCurrentModelIndex(rootIndex, true);
    }
}

void FootprintManagerWidget::slotDeleteFootprint()
{
    if(_footprintsListView->selectionModel()){
        QModelIndex index = _footprintsListView->selectionModel()->currentIndex();
        if(index.isValid()){
            _footprintsTableModel->removeRow(index.row());
            _detailsWidget->setCurrentModelIndex(QModelIndex());
        }
    }
}\

void FootprintManagerWidget::slotFilterChanged()
{
    if(!_searchLineEdit->text().isEmpty()){
        _footprintsTableModel->setFilter(QString("name LIKE '\%%1\%'").arg(_searchLineEdit->text()));
    }
    else{
        _footprintsTableModel->setFilter(QString::null);
        _footprintsTableModel->select();
    }

}

void FootprintManagerWidget::slotAddImage()
{

}

void FootprintManagerWidget::slotDeleteImage()
{

}

void FootprintManagerWidget::slotAccept()
{

}

void FootprintManagerWidget::slotReject()
{

}


