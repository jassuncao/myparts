#include "gridwidget.h"
#include <QPainter>
#include <QStyle>
#include <QLayout>
#include <QGridLayout>
#include <QFormLayout>

GridWidget::GridWidget(QWidget *parent) : QWidget(parent)
{    
}

GridWidget::~GridWidget()
{

}

void GridWidget::paintEvent(QPaintEvent *)
{
    QColor evenRowColourProperty(Qt::white);
    QColor oddRowColourProperty(QColor(0xF8, 0xF7, 0xF6));
    QPainter painter(this);
    painter.setPen(QColor(0xB3,0xAB,0xA7));

    // Fill the entire background with even colour first, then draw odd
    // rectangles. Probably more efficient this way
    painter.setBrush(evenRowColourProperty);
    painter.drawRect(0, 0, this->width(), this->height());

    QLayout * wLayout = layout();

    int rowCount = 0;
    int columnCount = 0;
    int horizontalSpacing = 0;
    int verticalSpacing = 0;

    QGridLayout * gridLayout = dynamic_cast<QGridLayout*>(wLayout);
    if(gridLayout){
        rowCount = gridLayout->rowCount();
        columnCount = gridLayout->columnCount();
        horizontalSpacing = gridLayout->horizontalSpacing();
        verticalSpacing = gridLayout->verticalSpacing();

    }

    int leftMargin;
    int topMargin;
    int rightMargin;
    int bottomMargin;

    wLayout->getContentsMargins(&leftMargin, &topMargin, &rightMargin, &bottomMargin);

    int y = 0;
    bool alt = false;
    int maxLabelWidth = 0;    
    for(int row = 0; row<rowCount; ++row){
        if (alt) {
            painter.setBrush(oddRowColourProperty);
        }
        else{
            painter.setBrush(evenRowColourProperty);
        }
        QLayoutItem* item = wLayout->itemAt(row*columnCount);
        if(item){
            int labelWidth = item->sizeHint().width();
            maxLabelWidth = qMax(maxLabelWidth, labelWidth);
            int h = item->sizeHint().height() + verticalSpacing;
            painter.drawRect(0, y, this->width()-1, h);
            y += h;
        }
        alt = !alt;
    }
    maxLabelWidth= maxLabelWidth + leftMargin+ (horizontalSpacing/2);
    painter.drawLine(maxLabelWidth,0, maxLabelWidth, this->height()-1);
}

