#include "partstoragewidget.h"
#include <QtGui>
#include <QSqlTableModel>
#include <QSqlError>
#include <QTableView>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QMessageBox>

PartStorageWidget::PartStorageWidget(QWidget *parent) :
    QWidget(parent)
{
    _model = new QSqlTableModel(this);
    _model->setTable("part_storage");
    _model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    _model->select();
    _model->setHeaderData(1, Qt::Horizontal, tr("Name"));

    _view= new QTableView;
    _view->setModel(_model);
    _view->setSelectionBehavior(QAbstractItemView::SelectRows);
    _view->setSelectionMode(QAbstractItemView::SingleSelection);
    _view->setColumnHidden(0,true);
    _view->verticalHeader()->setVisible(false);
    _view->resizeColumnsToContents();

    _buttonBox = new QDialogButtonBox(Qt::Vertical);
    _addButton = new QPushButton(tr("Add"));
    _removeButton = new QPushButton(tr("Remove"));
    _buttonBox->addButton(_addButton,QDialogButtonBox::ActionRole);
    _buttonBox->addButton(_removeButton,QDialogButtonBox::ActionRole);
    _removeButton->setEnabled(false);

    connect(_addButton, SIGNAL(clicked()), this, SLOT(addUnit()));
    connect(_removeButton, SIGNAL(clicked()), this, SLOT(deleteUnit()));

    QHBoxLayout * mainLayout = new QHBoxLayout;
    mainLayout->addWidget(_view);
    mainLayout->addWidget(_buttonBox);
    setLayout(mainLayout);

    connect(_model,SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(dataChanged(QModelIndex,QModelIndex)));
    connect(_view->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),this, SLOT(selectionChanged(QItemSelection,QItemSelection)));
}

bool PartStorageWidget::submit()
{
    _model->database().transaction();
    if(_model->submitAll()){
        if(_model->database().commit()){
            emit dataCommited();
            return true;
        }
    } else {
        _model->database().rollback();
        QMessageBox::warning(this,
                             tr("Error saving changes"),
                             tr("The database reported an error: %1").arg(_model->lastError().text()));
    }
    return false;
}

void PartStorageWidget::revert()
{
    _model->revertAll();
    emit dataCommited();
}

void PartStorageWidget::addStorage()
{
    int newRow = _model->rowCount();
    _model->insertRow(newRow);
    _view->selectRow(newRow);
    QModelIndex idx = _model->index(newRow,1);
    _view->edit(idx);
}

void PartStorageWidget::deleteStorage()
{
    QModelIndex currentIdx = _view->currentIndex();
    if(!currentIdx.isValid())
        return;
    if(_model->removeRows(currentIdx.row(),1)){
        submit();
    }
}

/*
void PartStorageWidget::dataChanged(const QModelIndex, const QModelIndex)
{
    emit dataChanged();
}
*/
