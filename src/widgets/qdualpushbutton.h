#ifndef QDUALPUSHBUTTON_H
#define QDUALPUSHBUTTON_H

#include <QPushButton>
#include <QComboBox>

class QToolButton;

class QDualPushButton : public QComboBox
{
    Q_OBJECT
public:
    explicit QDualPushButton(QWidget *parent=0);
    ~QDualPushButton();
protected:
    void resizeEvent(QResizeEvent *);
protected slots:
    void slotClearText();
private:
    QToolButton *clearButton;    
};

#endif // QDUALPUSHBUTTON_H
