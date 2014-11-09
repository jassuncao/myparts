#include "customtreeview.h"
#include <qevent.h>

CustomTreeView::CustomTreeView(QWidget *parent) :
    QTreeView(parent)
{
    setDropIndicatorShown(true);
}


void CustomTreeView::dragMoveEvent(QDragMoveEvent *event)
{

    if ( this->dropIndicatorPosition() == QAbstractItemView::AboveItem )
    {
        event->ignore();
        return;
    }

    QTreeView::dragMoveEvent(event);
   //event->accept();
}

