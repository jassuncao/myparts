#include "extendedtableview.h"
#include <QKeyEvent>
#include <QHeaderView>
#include <QDrag>

ExtendedTableView::ExtendedTableView(QWidget *parent) :
    QTableView(parent)
{
    setVerticalScrollMode(QTableView::ScrollPerPixel);
    horizontalHeader()->setHighlightSections(false);
}

ExtendedTableView::~ExtendedTableView()
{
}

void ExtendedTableView::setDragPixmap(const QPixmap &dragPixmap)
{
    _dragPixmap = dragPixmap;
}

void ExtendedTableView::closeEditor(QWidget* editor, QAbstractItemDelegate::EndEditHint hint)
{
    if(hint == QAbstractItemDelegate::EditNextItem){
        int right = model()->columnCount() - 1;
        while (right >= 0 && isColumnHidden(right))
            --right;
        QModelIndex current = currentIndex();
        if(current.row()==model()->rowCount()-1 && current.column()==right){
            model()->insertRow(model()->rowCount());
        }
    }

    QTableView::closeEditor(editor, hint);
}

bool isIndexDragEnabled(QAbstractItemModel * model, const QModelIndex &index) {
    return (model->flags(index) & Qt::ItemIsDragEnabled);
}

QModelIndexList ExtendedTableView::selectedDraggableIndexes() const
{
    QModelIndexList indexes = selectedIndexes();
    for(int i = indexes.count() - 1 ; i >= 0; --i) {
        if (!isIndexDragEnabled(model(), indexes.at(i)))
            indexes.removeAt(i);
    }
    return indexes;
}

void ExtendedTableView::startDrag(Qt::DropActions supportedActions)
{
    if(_dragPixmap.isNull()){
        QTableView::startDrag(supportedActions);
    }
    else{

    }
    QModelIndexList indexes = selectedDraggableIndexes();
    if (indexes.count() > 0) {
        QMimeData *data = model()->mimeData(indexes);
        if (!data)
            return;
        QRect rect;
        rect.adjust(horizontalOffset(), verticalOffset(), 0, 0);
        QDrag *drag = new QDrag(this);
        drag->setPixmap(_dragPixmap);
        drag->setMimeData(data);
        //drag->setHotSpot(d->pressedPosition - rect.topLeft());
        Qt::DropAction viewDefaultDropAction = defaultDropAction();
        Qt::DropAction defaultDropAction = Qt::IgnoreAction;
        if (viewDefaultDropAction != Qt::IgnoreAction && (supportedActions & viewDefaultDropAction))
            defaultDropAction = viewDefaultDropAction;
        else if (supportedActions & Qt::CopyAction && dragDropMode() != QAbstractItemView::InternalMove)
            defaultDropAction = Qt::CopyAction;
        if (drag->exec(supportedActions, defaultDropAction) == Qt::MoveAction){
            clearOrRemove();
        }
    }
}

void ExtendedTableView::clearOrRemove()
{
    const QItemSelection selection = selectionModel()->selection();
    QList<QItemSelectionRange>::const_iterator it = selection.constBegin();
    QAbstractItemModel * aModel = model();
    bool overwrite = dragDropOverwriteMode();
    if (!overwrite) {
        for (; it != selection.constEnd(); ++it) {
            QModelIndex parent = (*it).parent();
            if ((*it).left() != 0)
                continue;
            if ((*it).right() != (aModel->columnCount(parent) - 1))
                continue;
            int count = (*it).bottom() - (*it).top() + 1;
            aModel->removeRows((*it).top(), count, parent);
        }
    } else {
        // we can't remove the rows so reset the items (i.e. the view is like a table)
        QModelIndexList list = selection.indexes();
        for (int i=0; i < list.size(); ++i) {
            QModelIndex index = list.at(i);
            QMap<int, QVariant> roles = aModel->itemData(index);
            for (QMap<int, QVariant>::Iterator it = roles.begin(); it != roles.end(); ++it)
                it.value() = QVariant();
            aModel->setItemData(index, roles);
        }
    }
}

