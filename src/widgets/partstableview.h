#ifndef PARTSTABLEVIEW_H
#define PARTSTABLEVIEW_H

#include <QTableView>
#include <QPoint>

class QAbstractItemModel;
class QMenu;

class PartsTableView : public QTableView
{
    Q_OBJECT
public:
    explicit PartsTableView(QWidget *parent = 0);
    virtual void setModel(QAbstractItemModel *model);
signals:
    
public slots:
protected slots:
    void slotToggleTableColumn(bool checked);
    void slotHeaderContextMenu(QPoint point);
protected:
  void startDrag(Qt::DropActions supportedActions);
  void setupHeaderContextMenu();
  QMenu * _tableHeaderContextMenu;
};

#endif // PARTSTABLEVIEW_H
