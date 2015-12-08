#include "extendedtableview.h"
#include <QKeyEvent>
#include <QHeaderView>

ExtendedTableView::ExtendedTableView(QWidget *parent) :
    QTableView(parent)
{
    setVerticalScrollMode(QTableView::ScrollPerPixel);
    horizontalHeader()->setHighlightSections(false);
}

ExtendedTableView::~ExtendedTableView()
{
}

void ExtendedTableView::closeEditor(QWidget* editor, QAbstractItemDelegate::EndEditHint hint)
{
    if(hint==QAbstractItemDelegate::EditNextItem){
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

