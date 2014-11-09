#ifndef QSEARCHLINEEDIT_H
#define QSEARCHLINEEDIT_H

#include <QLineEdit>

class QToolButton;
class QLabel;

class QSearchLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    explicit QSearchLineEdit(QWidget *parent = 0);
    
protected:
    void resizeEvent(QResizeEvent *);
private slots:
    void updateClearButton(const QString &text);
private:
    QToolButton *clearButton;
    QLabel *imageLabel;
    
};

#endif // QSEARCHLINEEDIT_H
