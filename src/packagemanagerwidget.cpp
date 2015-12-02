#include "packagemanagerwidget.h"
#include "widgets/qsearchlineedit.h"
#include "packagedetailswidget.h"
#include "models/packagetablemodel.h"
#include <QVBoxLayout>
#include <QLineEdit>
#include <QToolBar>
#include <QListView>
#include <QSplitter>
#include <QDebug>

PackageManagerWidget::PackageManagerWidget(QWidget *parent) : QWidget(parent)
{

    QToolBar * toolbar = new QToolBar;
    toolbar->setIconSize(QSize(16,16));

    toolbar->addAction(QIcon(":/icons/package_add"),tr("Add Package"), this, SLOT(slotAddPackage()));
    toolbar->addAction(QIcon(":/icons/package_delete"),tr("Delete Package"), this, SLOT(slotDeletePackage()));

    _searchLineEdit = new QSearchLineEdit;
    toolbar->addWidget(_searchLineEdit);
    connect(_searchLineEdit, SIGNAL(textClear()), this, SLOT(slotFilterChanged()));

    _packagesListView = new QListView;
    _packagesListView->setEditTriggers(QListView::NoEditTriggers);
    _packagesListView->setSelectionBehavior(QListView::SelectRows);
    _packagesListView->setSelectionMode(QListView::SingleSelection);

    QVBoxLayout *westBoxLayout = new QVBoxLayout;
    westBoxLayout->addWidget(toolbar);
    westBoxLayout->addWidget(_packagesListView);
    QWidget * westWidgetWrapper = new QWidget;
    westWidgetWrapper->setLayout(westBoxLayout);

    _detailsWidget = new PackageDetailsWidget(this);   

    QSplitter * hSplitter = new QSplitter(Qt::Horizontal);
    hSplitter->addWidget(westWidgetWrapper);
    hSplitter->addWidget(_detailsWidget);
    hSplitter->setStretchFactor(1,1);

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(hSplitter);
    setLayout(layout);    

    _packagesTableModel = new PackageTableModel(this);
    _packagesTableModel->setSort(PackageTableModel::ColumnName, Qt::AscendingOrder);
    _packagesTableModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    _packagesTableModel->select();

    _packagesListView->setModel(_packagesTableModel);
    _packagesListView->setModelColumn(PackageTableModel::ColumnName);

    _detailsWidget->setModel(_packagesTableModel);
    _detailsWidget->setCurrentModelIndex(QModelIndex());

    connect(_searchLineEdit, SIGNAL(returnPressed()), this, SLOT(slotFilterChanged()));
    connect(_packagesListView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)), this, SLOT(slotCurrentRowChanged(QModelIndex,QModelIndex)));
}

PackageManagerWidget::~PackageManagerWidget()
{
    delete _packagesTableModel;
}

void PackageManagerWidget::slotCurrentRowChanged(const QModelIndex &current, const QModelIndex &)
{
    if(!current.isValid()){
        return;
    }
    QModelIndex packageIdIndex =_packagesTableModel->index(current.row(), PackageTableModel::ColumnId);
    _detailsWidget->setCurrentModelIndex(packageIdIndex);
}

void PackageManagerWidget::slotAddPackage()
{
    if(_detailsWidget->isNew()){
        return;
    }
    int row = _packagesTableModel->rowCount();    
    if(_packagesTableModel->insertRow(row)){
        QModelIndex rootIndex = _packagesTableModel->index(row, PackageTableModel::ColumnId);
        _detailsWidget->setCurrentModelIndex(rootIndex, true);
    }
}

void PackageManagerWidget::slotDeletePackage()
{
    if(_packagesListView->selectionModel()){
        QModelIndex index = _packagesListView->selectionModel()->currentIndex();
        if(index.isValid()){
            _packagesTableModel->removeRow(index.row());
            _detailsWidget->setCurrentModelIndex(QModelIndex());
        }
    }
}\

void PackageManagerWidget::slotFilterChanged()
{
    if(!_searchLineEdit->text().isEmpty()){
        _packagesTableModel->setFilter(QString("name LIKE '\%%1\%'").arg(_searchLineEdit->text()));
    }
    else{
        _packagesTableModel->setFilter(QString::null);
        _packagesTableModel->select();
    }

}

void PackageManagerWidget::slotAddImage()
{

}

void PackageManagerWidget::slotDeleteImage()
{

}

void PackageManagerWidget::slotAccept()
{

}

void PackageManagerWidget::slotReject()
{

}


