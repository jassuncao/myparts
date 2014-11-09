#include "partconditionwidget.h"
#include <QtGui>
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlQuery>
#include "models/partconditionsqltablemodel.h"
#include "widgets/booleanitemdelegate.h"

PartConditionWidget::PartConditionWidget(QWidget *parent) :
    QWidget(parent)
{
    _model = new PartConditionSqlTableModel(this);
    _model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    _model->select();
    _model->setHeaderData(PartConditionSqlTableModel::ColumnName, Qt::Horizontal, tr("Name"));
    _model->setHeaderData(PartConditionSqlTableModel::ColumnDescription, Qt::Horizontal, tr("Description"));
    _model->setHeaderData(PartConditionSqlTableModel::ColumnDefault, Qt::Horizontal, QString());

    _view= new QTableView;
    _view->setModel(_model);
    _view->setSelectionBehavior(QAbstractItemView::SelectRows);
    _view->setSelectionMode(QAbstractItemView::SingleSelection);
    _view->setColumnHidden(PartConditionSqlTableModel::ColumnId,true);
    QStyledItemDelegate * colDelegate = new BooleanItemDelegate(QPixmap(":icons/default"),QPixmap());
    _view->setItemDelegateForColumn(PartConditionSqlTableModel::ColumnDefault, colDelegate);
    _view->verticalHeader()->setVisible(false);
    _view->horizontalHeader()->setResizeMode(PartConditionSqlTableModel::ColumnDefault, QHeaderView::Fixed);
    _view->horizontalHeader()->moveSection(PartConditionSqlTableModel::ColumnDefault,0);
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

    connect(_addButton, SIGNAL(clicked()), this, SLOT(addElement()));
    connect(_removeButton, SIGNAL(clicked()), this, SLOT(deleteElement()));
    connect(_makeDefaultButton, SIGNAL(clicked()), this, SLOT(makeAsDefault()));

    QHBoxLayout * mainLayout = new QHBoxLayout;
    mainLayout->addWidget(_view);
    mainLayout->addWidget(_buttonBox);
    setLayout(mainLayout);

    connect(_model,SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(dataChanged(QModelIndex,QModelIndex)));
    connect(_view->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),this, SLOT(selectionChanged(QItemSelection,QItemSelection)));
}

bool PartConditionWidget::submit()
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

void PartConditionWidget::revert()
{
    _model->revertAll();
    emit dataCommited();
}

void PartConditionWidget::addElement()
{
    int newRow = _model->rowCount();
    _model->insertRow(newRow);
    _view->selectRow(newRow);
    QModelIndex idx = _model->index(newRow,1);
    _view->edit(idx);
}

void PartConditionWidget::deleteElement()
{
    QModelIndex currentIdx = _view->currentIndex();
    if(!currentIdx.isValid())
        return;
    if(_model->removeRows(currentIdx.row(),1)){
        submit();
    }
}

void PartConditionWidget::makeAsDefault()
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
    query.prepare("UPDATE part_condition SET defaultCondition = 1 WHERE id = ?");
    query.bindValue(0,id);
    query.exec();
    query.prepare("UPDATE part_unit SET defaultCondition = 0 WHERE id <> ? AND defaultCondition=1");
    query.bindValue(0,id);
    query.exec();
     _model->database().commit();
    _model->select();
    emit dataCommited();
}

void PartConditionWidget::dataChanged(const QModelIndex, const QModelIndex)
{
    emit dataChanged();
}

void PartConditionWidget::selectionChanged(const QItemSelection &selected, const QItemSelection)
{
    _removeButton->setEnabled(selected.size()>0);
    _makeDefaultButton->setEnabled(selected.size()>0);
}
