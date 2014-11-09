#ifndef PARTSTABLEVIEW_H
#define PARTSTABLEVIEW_H

#include <QTableView>

class PartsTableView : public QTableView
{
    Q_OBJECT
public:
    explicit PartsTableView(QWidget *parent = 0);
    
signals:
    
public slots:

protected:
  void startDrag(Qt::DropActions supportedActions);
    
};

#endif // PARTSTABLEVIEW_H
