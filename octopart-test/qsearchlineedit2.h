#ifndef QSEARCHLINEEDIT2_H
#define QSEARCHLINEEDIT2_H


#include <QLineEdit>
#include <QPixmap>

class QToolButton;
class QLabel;
class QProgressIndicator;

class QSearchLineEdit2 : public QLineEdit
{
    Q_OBJECT
public:
    explicit QSearchLineEdit2(QWidget *parent = 0);
    void setBusy(bool busy);
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
    QProgressIndicator *_progressIndicator;
};

#endif // QSEARCHLINEEDIT2_H
