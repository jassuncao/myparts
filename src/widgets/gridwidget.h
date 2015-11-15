#ifndef GRIDWIDGET_H
#define GRIDWIDGET_H

#include <QWidget>

class GridWidget : public QWidget
{
    Q_OBJECT
public:
    explicit GridWidget(QWidget *parent = 0);
    ~GridWidget();

protected:
    void paintEvent(QPaintEvent *);
private:

};

#endif // GRIDWIDGET_H
