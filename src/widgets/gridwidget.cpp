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

    QFormLayout * formLayout = dynamic_cast<QFormLayout*>(layout());
    int leftMargin;
    int topMargin;
    int rightMargin;
    int bottomMargin;

    formLayout->getContentsMargins(&leftMargin, &topMargin, &rightMargin, &bottomMargin);
    int spacing = formLayout->verticalSpacing();
    int y= 0;
    bool alt = false;
    int maxLabelWidth = 0;
    for (int i = 0; i < formLayout->rowCount(); i++) {

        if (alt) {
            painter.setBrush(oddRowColourProperty);
        }
        else{
            painter.setBrush(evenRowColourProperty);
        }
        QLayoutItem* item = formLayout->itemAt(i, QFormLayout::FieldRole);
        if (item) {
            int h = item->sizeHint().height() + spacing;
            painter.drawRect(0, y, this->width()-1, h);
            y += h;
            alt = !alt;
        }
        else {
            item = formLayout->itemAt(i, QFormLayout::SpanningRole);
            if (item) {
                int h = item->sizeHint().height() + spacing;
                painter.drawRect(0, y, this->width()-1, h);
                y += h;
                alt = !alt;
            }
        }
        int aux = formLayout->itemAt(i, QFormLayout::LabelRole)->sizeHint().width();
        maxLabelWidth = maxLabelWidth<aux ? aux : maxLabelWidth;
    }
    maxLabelWidth= maxLabelWidth + leftMargin+ (formLayout->horizontalSpacing()/2);
    painter.drawLine(maxLabelWidth,0, maxLabelWidth, this->height()-1);
}

