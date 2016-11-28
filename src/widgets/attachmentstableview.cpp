#include "attachmentstableview.h"
#include <QMimeData>
#include <QDragEnterEvent>
#include <QUrl>
#include <QModelIndex>
#include <QDebug>

AttachmentsTableView::AttachmentsTableView(QWidget *parent)
    : QTableView(parent)
{
    setAcceptDrops(true);
    setDropIndicatorShown(true);
    viewport()->setAcceptDrops(true);
}

void AttachmentsTableView::dragEnterEvent(QDragEnterEvent *event)
{
    const QAbstractItemModel * m = model();
    if(m && ((event->dropAction() & m->supportedDropActions()) != 0) && event->mimeData()->hasUrls()){
        event->acceptProposedAction();
    };
}

void AttachmentsTableView::dragMoveEvent(QDragMoveEvent *event)
{
    const QAbstractItemModel * m = model();
    if(m && ((event->dropAction() & m->supportedDropActions()) != 0) && event->mimeData()->hasUrls()){
        event->acceptProposedAction();
    };
}

void AttachmentsTableView::dropEvent(QDropEvent *event)
{
    const QAbstractItemModel * m = model();
    if(m && ((event->dropAction() & m->supportedDropActions()) != 0) && event->mimeData()->hasUrls()){
        const QMimeData * data = event->mimeData();
        const Qt::DropAction action = event->dropAction();
        QModelIndex index = indexAt(event->pos());
        model()->dropMimeData(data, action, index.row(), index.column(), index.parent());
        event->acceptProposedAction();
    }
}


