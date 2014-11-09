#include <QtGui>
#include "partstableview.h"
#include "qsortfiltersqlquerymodel.h"

PartsTableView::PartsTableView(QWidget *parent) :
    QTableView(parent)
{
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
