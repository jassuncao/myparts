#include "checkboxheaderview.h"
#include <QPainter>
#include <QMouseEvent>
#include <QDebug>
#include <QCheckBox>

CheckboxHeaderView::CheckboxHeaderView(Qt::Orientation orientation, QWidget *parent)
    : QHeaderView(orientation, parent), _selected(false)
{
}

QSize CheckboxHeaderView::sectionSizeFromContents(int logicalIndex) const
{    
    if (logicalIndex == 0)
    {        
        const int margin = style()->pixelMetric(QStyle::PM_HeaderMargin);
        QStyleOptionButton option;
        option.initFrom(this);        
        const QRect checkRect = style()->subElementRect(QStyle::SE_CheckBoxIndicator, &option, this);
        int width = checkRect.width() + margin * 2;
        int height = checkRect.height();
        return QSize(width, height);
    }
    else{
        return QHeaderView::sectionSizeFromContents(logicalIndex);
    }
}

void CheckboxHeaderView::paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const
{
    painter->save();
    QHeaderView::paintSection(painter, rect, logicalIndex);
    painter->restore();

    if (logicalIndex == 0)
    {

      QStyleOptionButton option;
      option.initFrom(this);
      option.state |= QStyle::State_Enabled;
      option.state |= _selected ? QStyle::State_On : QStyle::State_Off;
      const int margin = style()->pixelMetric(QStyle::PM_HeaderMargin);
      option.rect.moveLeft(margin);
/*
      QRect checkRect = style()->subElementRect(QStyle::SE_ItemViewItemCheckIndicator, &option, this);
      qDebug()<<"Size "<< spacing;
*/

      this->style()->drawControl(QStyle::CE_CheckBox, &option, painter);           

    }    

}
void CheckboxHeaderView::mousePressEvent(QMouseEvent *event)
{    
    QHeaderView::mousePressEvent(event);    
    int idx = this->logicalIndexAt(event->pos());
    if(idx != 0){
        return;
    }

    QStyleOptionButton option;
    option.initFrom(this);    
    QRect checkRect = style()->subElementRect(QStyle::SE_ItemViewItemCheckIndicator, &option, this);
    const int margin = style()->pixelMetric(QStyle::PM_HeaderMargin);
    checkRect.moveLeft(margin);
    if(!checkRect.contains(event->pos())){
        return;
    }

    if(_selected){
        _selected = false;
        emit stateChanged(Qt::Unchecked);
    }
    else{
        _selected = true;

        emit stateChanged(Qt::Checked);
    }

    this->updateSection(0);
}

void CheckboxHeaderView::setChecked(Qt::CheckState state)
{
    switch(state){
        case Qt::Unchecked:
        _selected = false;
        break;
    case Qt::Checked:
    case Qt::PartiallyChecked:
        _selected = true;
        break;
    }
    this->updateSection(0);
}

void CheckboxHeaderView::reset()
{
    QHeaderView::reset();
    setChecked(Qt::Unchecked);
}
