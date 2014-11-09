#ifndef CUSTOMTREEVIEW_H
#define CUSTOMTREEVIEW_H

#include <QTreeView>

class QDragMoveEvent;

class CustomTreeView : public QTreeView
{
    Q_OBJECT
public:
    explicit CustomTreeView(QWidget *parent = 0);
    
signals:
    
public slots:

protected:
   void dragMoveEvent(QDragMoveEvent *event);
    
};

#endif // CUSTOMTREEVIEW_H
