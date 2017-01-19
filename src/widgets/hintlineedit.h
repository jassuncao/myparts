#ifndef HINTLINEEDIT_H
#define HINTLINEEDIT_H

#include <QLineEdit>
#include <QPixmap>

class QToolButton;
class QLabel;

class HintLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    explicit HintLineEdit(QWidget *parent = 0);

signals:
    void textClear();
protected:
    void resizeEvent(QResizeEvent *);
private slots:
    void slotShowHint();
private:
    QToolButton * _hintButton;
};

#endif // HINTLINEEDIT_H
