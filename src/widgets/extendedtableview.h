#ifndef EXTENDEDTABLEVIEW_H
#define EXTENDEDTABLEVIEW_H

#include <QTableView>
#include <QModelIndexList>

class QPixmap;

class ExtendedTableView: public QTableView
{
public:
    ExtendedTableView(QWidget *parent = 0);
    ~ExtendedTableView();
    void setDragPixmap(const QPixmap & dragPixmap);
protected:
    void closeEditor(QWidget* editor, QAbstractItemDelegate::EndEditHint hint);
    void startDrag(Qt::DropActions supportedActions);
private:
    QModelIndexList selectedDraggableIndexes() const;
    void clearOrRemove();
    QPixmap _dragPixmap;
};

#endif // EXTENDEDTABLEVIEW_H
