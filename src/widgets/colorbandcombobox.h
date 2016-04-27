#ifndef COLORBANDCOMBOBOX_H
#define COLORBANDCOMBOBOX_H

#include <QComboBox>

class ColorBandComboBox : public QComboBox
{
    Q_OBJECT
public:

    static const int ValueRole = Qt::UserRole+1;
    static const int ShortcutRole = Qt::UserRole+2;

    explicit ColorBandComboBox(QWidget *parent = 0);    
protected:
    void paintEvent(QPaintEvent * );
    void keyPressEvent(QKeyEvent * event);
};

#endif // COLORBANDCOMBOBOX_H
