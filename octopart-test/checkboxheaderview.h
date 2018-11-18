#ifndef CHECKBOXHEADERVIEW_H
#define CHECKBOXHEADERVIEW_H

#include <QHeaderView>
#include <QRect>
#include <QSize>

class CheckboxHeaderView : public QHeaderView
{
    Q_OBJECT
public:    
    explicit CheckboxHeaderView(Qt::Orientation orientation, QWidget *parent = 0);
public slots:
    void setChecked(Qt::CheckState state);
    virtual void reset();
signals:
    void stateChanged(Qt::CheckState state);
protected:
  void paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const;
  void mousePressEvent(QMouseEvent *event);
  QSize sectionSizeFromContents(int logicalIndex) const;
private:  
  bool _selected;
};

#endif // CHECKBOXHEADERVIEW_H
