#include "colorbandcombobox.h"
#include <QKeyEvent>
#include <QStylePainter>

const static int BLACK_COLOR_INDEX = 1;

ColorBandComboBox::ColorBandComboBox(QWidget *parent) : QComboBox(parent)
{

}

void ColorBandComboBox::keyPressEvent(QKeyEvent * event)
{
    int key = event->key();
    for(int index=0; index< model()->rowCount(); ++index){
        QVariant var = itemData(index, ShortcutRole);
        if(var.isValid() && var.toInt()==key){
            setCurrentIndex(index);
            focusNextChild();
            return;
        }
    }
    QComboBox::keyPressEvent(event);
}

void ColorBandComboBox::paintEvent(QPaintEvent * ) {
    QStylePainter painter(this);
    painter.setPen(palette().color(QPalette::Text));

    // draw the combobox frame, focusrect and selected etc.
    QStyleOptionComboBox opt;
    initStyleOption(&opt);
    if (opt.currentIcon.isNull() && currentIndex()>0) {
        QColor c = qvariant_cast<QColor>(itemData(currentIndex(), Qt::DecorationRole));
        if (c.isValid()) {
            int siz = style()->pixelMetric(QStyle::PM_ButtonIconSize, &opt, this);
            QPixmap px(siz, siz);
            px.fill(c);
            opt.currentIcon = px;
        }
    }
    painter.drawComplexControl(QStyle::CC_ComboBox, opt);

    // draw the icon and text
    painter.drawControl(QStyle::CE_ComboBoxLabel, opt);
}
