#include "partunitswidget.h"
#include <QtGui>
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlQuery>
#include "partunits/partunitssqltablemodel.h"
#include "widgets/booleanitemdelegate.h"

PartUnitsWidget::PartUnitsWidget(QWidget *parent) :
    OptionsWidget(parent)
{
    _model = new PartUnitsSqlTableModel(this);    
    _model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    _model->select();
    _model->setHeaderData(PartUnitsSqlTableModel::ColumnName, Qt::Horizontal, tr("Name"));
    _model->setHeaderData(PartUnitsSqlTableModel::ColumnAbbreviation, Qt::Horizontal, tr("Abbreviation"));
    _model->setHeaderData(PartUnitsSqlTableModel::ColumnDefault, Qt::Horizontal, QString());

    _view= new QTableView;
    _view->setModel(_model);
    _view->setSelectionBehavior(QAbstractItemView::SelectRows);
    _view->setSelectionMode(QAbstractItemView::SingleSelection);    
    _view->setColumnHidden(PartUnitsSqlTableModel::ColumnId,true);
    QStyledItemDelegate * colDelegate = new BooleanItemDelegate(QPixmap(":icons/default"),QPixmap());
    _view->setItemDelegateForColumn(PartUnitsSqlTableModel::ColumnDefault, colDelegate);
    _view->verticalHeader()->setVisible(false);
    _view->horizontalHeader()->setResizeMode(PartUnitsSqlTableModel::ColumnDefault, QHeaderView::Fixed);
    _view->horizontalHeader()->moveSection(PartUnitsSqlTableModel::ColumnDefault,0);
    _view->horizontalHeader()->setStretchLastSection(true);
    _view->resizeColumnsToContents();

    _buttonBox = new QDialogButtonBox(Qt::Vertical);
    _addButton = new QPushButton(tr("Add"));
    _removeButton = new QPushButton(tr("Remove"));
    _makeDefaultButton = new QPushButton(tr("Make default"));
    _buttonBox->addButton(_addButton,QDialogButtonBox::ActionRole);
    _buttonBox->addButton(_removeButton,QDialogButtonBox::ActionRole);
    _buttonBox->addButton(_makeDefaultButton,QDialogButtonBox::ActionRole);
    _removeButton->setEnabled(false);
    _makeDefaultButton->setEnabled(false);

    connect(_addButton, SIGNAL(clicked()), this, SLOT(addUnit()));
    connect(_removeButton, SIGNAL(clicked()), this, SLOT(deleteUnit()));
    connect(_makeDefaultButton, SIGNAL(clicked()), this, SLOT(makeDefault()));

    QHBoxLayout * mainLayout = new QHBoxLayout;
    mainLayout->addWidget(_view);
    mainLayout->addWidget(_buttonBox);
    setLayout(mainLayout);

    connect(_model,SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(dataChanged(QModelIndex,QModelIndex)));    
    connect(_view->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),this, SLOT(selectionChanged(QItemSelection,QItemSelection)));
}

bool PartUnitsWidget::submit()
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

void PartUnitsWidget::revert()
{
    _model->revertAll();
    emit dataCommited();
}

void PartUnitsWidget::addUnit()
{
    int newRow = _model->rowCount();
    _model->insertRow(newRow);
    _view->selectRow(newRow);
    QModelIndex idx = _model->index(newRow,1);
    _view->edit(idx);    
}

void PartUnitsWidget::deleteUnit()
{               
    QModelIndex currentIdx = _view->currentIndex();    
    if(!currentIdx.isValid())
        return;
    if(_model->removeRows(currentIdx.row(),1)){
        submit();
    }
}

void PartUnitsWidget::makeDefault()
{
    QModelIndex currentIdx = _view->currentIndex();
    if(!currentIdx.isValid())
        return;
    submit();

    QSqlRecord record = _model->record(currentIdx.row());
    if(record.isEmpty())
        return;
    QVariant id = record.value(0);

    _model->database().transaction();
    QSqlQuery query;
    query.prepare("UPDATE part_unit SET defaultUnit = 1 WHERE id = ?");
    query.bindValue(0,id);
    query.exec();
    query.prepare("UPDATE part_unit SET defaultUnit = 0 WHERE id <> ? AND defaultUnit=1");
    query.bindValue(0,id);
    query.exec();
     _model->database().commit();
    _model->select();
    emit dataCommited();
}

void PartUnitsWidget::dataChanged(const QModelIndex, const QModelIndex)
{
    emit OptionsWidget::dataChanged();
    //emit dataChanged();
}

void PartUnitsWidget::selectionChanged(const QItemSelection &selected, const QItemSelection)
{
    _removeButton->setEnabled(selected.size()>0);
    _makeDefaultButton->setEnabled(selected.size()>0);
}
