#include <QHeaderView>
#include <QDrag>
#include <QAction>
#include <QMenu>
#include <QTableView>
#include "partstableview.h"
#include "qsortfiltersqlquerymodel.h"
#include "models/partssqltablemodel.h"
#include "datetimedelegate.h"
#include "currencydelegate.h"

PartsTableView::PartsTableView(QWidget *parent) :
    QTableView(parent), _tableHeaderContextMenu(0)
{
    QFont f = font();
    f.setPointSize(8);
    setFont(f);
    setFrameStyle(QFrame::StyledPanel);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::ExtendedSelection);
    setEditTriggers(QTableView::NoEditTriggers);
    verticalHeader()->setVisible(false);
    //horizontalHeader()->setStretchLastSection(true);     
    horizontalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
    horizontalHeader()->setHighlightSections(false);
    connect(horizontalHeader(), SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(slotHeaderContextMenu(QPoint)));    
}

void PartsTableView::setModel(QAbstractItemModel * model)
{
    QTableView::setModel(model);
    sortByColumn(PartsSqlTableModel::ColumnName,Qt::AscendingOrder);
    setDragEnabled(true);
    setDragDropMode(QAbstractItemView::DragOnly);
    setDefaultDropAction(Qt::LinkAction);
    setSortingEnabled(true);
    //setItemDelegateForColumn(PartsSqlTableModel::ColumnCreateDate, new DateDelegate(this));
    setItemDelegateForColumn(PartsSqlTableModel::ColumnAvgPrice, new CurrencyDelegate(this));

    int colCount = model->columnCount();
    for(int section = 0; section<colCount; ++section){
        QVariant aux = model->headerData(section, Qt::Horizontal, PartsSqlTableModel::VISIBILITY_COLUMN_ROLE);
        if(aux.toBool()==false){
            setColumnHidden(section, true);
        }
    }
    setupHeaderContextMenu();    
}

void PartsTableView::setupHeaderContextMenu(){
    if(_tableHeaderContextMenu)
        delete _tableHeaderContextMenu;
    _tableHeaderContextMenu = new QMenu(this);
    _tableHeaderContextMenu->addSeparator();
    QMenu * colsMenu = _tableHeaderContextMenu;
    QAbstractItemModel * tModel = model();
    int colCount = tModel->columnCount();
    for(int section = 0; section<colCount; ++section){
        QVariant aux = tModel->headerData(section, Qt::Horizontal, PartsSqlTableModel::VISIBILITY_COLUMN_ROLE);
        if(aux.isValid() || aux.toBool()){
            QString columnName = tModel->headerData(section,Qt::Horizontal).toString();
            QAction * action = colsMenu->addAction(columnName);
            action->setCheckable(true);
            action->setChecked(!isColumnHidden(section));
            action->setData(section);
            connect(action,SIGNAL(toggled(bool)),this, SLOT(slotToggleTableColumn(bool)));
        }
    }
}

void PartsTableView::slotToggleTableColumn(bool checked)
{
    QAction * action = (QAction*)sender();
    int column = action->data().toInt();
    setColumnHidden(column,!checked);
}

void PartsTableView::slotHeaderContextMenu(QPoint point)
{
    if(_tableHeaderContextMenu)
        _tableHeaderContextMenu->popup(horizontalHeader()->mapToGlobal(point));
}


void PartsTableView::startDrag(Qt::DropActions supportedActions)
{
    QModelIndexList indexes = selectionModel()->selectedRows();
    for(int i = indexes.count() - 1 ; i >= 0; --i) {
        if(!(model()->flags(indexes.at(i)) & Qt::ItemIsDragEnabled))
            indexes.removeAt(i);
   }
    if (indexes.count() > 0) {
       QMimeData *data = model()->mimeData(indexes);
       if (!data)
       return;
       QDrag *drag = new QDrag(this);
       drag->setPixmap(QPixmap(":/icons/images/plugin_add.png"));
       drag->setMimeData(data);
       Qt::DropAction viewDefaultDropAction = defaultDropAction();
       Qt::DropAction myDefaultAction = Qt::IgnoreAction;
       if (viewDefaultDropAction != Qt::IgnoreAction && (supportedActions & viewDefaultDropAction))
                  myDefaultAction = viewDefaultDropAction;
       else if (supportedActions & Qt::CopyAction && dragDropMode() != QAbstractItemView::InternalMove)
           myDefaultAction = Qt::CopyAction;
       //qDebug("Action = %d, supported =%d", myDefaultAction, (int)supportedActions);
       Qt::DropAction action = drag->exec(supportedActions, myDefaultAction);
       //qDebug("Action = %d", action);
       if (action == Qt::MoveAction)
       {
           QSortFilterSqlQueryModel * tableModel = static_cast<QSortFilterSqlQueryModel*>(model());
           tableModel->select();
       }
   }
}
