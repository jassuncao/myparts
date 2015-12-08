#ifndef EXTENDEDTABLEVIEW_H
#define EXTENDEDTABLEVIEW_H

#include <QTableView>

class ExtendedTableView: public QTableView
{
public:
    ExtendedTableView(QWidget *parent = 0);
    ~ExtendedTableView();
protected:
    virtual void closeEditor(QWidget* editor, QAbstractItemDelegate::EndEditHint hint);
};

#endif // EXTENDEDTABLEVIEW_H
