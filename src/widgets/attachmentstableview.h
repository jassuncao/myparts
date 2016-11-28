#ifndef ATTACHMENTSTABLEVIEW_H
#define ATTACHMENTSTABLEVIEW_H

#include <QTableView>

class AttachmentsTableView : public QTableView
{
public:
    explicit AttachmentsTableView(QWidget *parent = 0);
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);
};

#endif // ATTACHMENTSTABLEVIEW_H
