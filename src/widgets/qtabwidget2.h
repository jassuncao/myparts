#ifndef QTABWIDGET2_H
#define QTABWIDGET2_H

#include <QTabWidget>

class QAction;

class QTabWidget2 : public QTabWidget
{
    Q_OBJECT
public:
    explicit QTabWidget2(QWidget *parent = 0);
    ~QTabWidget2();
private slots:
    void slotTabContextMenu(const QPoint &pos);
    void slotCloseCurrentTab();
    void slotCloseOtherTabs();
    void slotCloseAllTabs();
private:
    QAction * _actionCloseTab;
    QAction * _actionCloseOtherTabs;
    QAction * _actionCloseAllTabs;
    int _contextTab;
};

#endif // QTABWIDGET2_H
