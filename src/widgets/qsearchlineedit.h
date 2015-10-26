#ifndef QSEARCHLINEEDIT_H
#define QSEARCHLINEEDIT_H

#include <QLineEdit>
#include <QPixmap>

class QToolButton;
class QLabel;

class QSearchLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    explicit QSearchLineEdit(QWidget *parent = 0);

signals:
    void textClear();
protected:
    void resizeEvent(QResizeEvent *);
    void keyPressEvent(QKeyEvent *event);
private slots:
    void updateClearButton(const QString &text);
    void slotClearText();
private:
    QToolButton *clearButton;
    QLabel *imageLabel;
    QPixmap _searchPixmap;
};

#endif // QSEARCHLINEEDIT_H
