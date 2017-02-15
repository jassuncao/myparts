#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>

class QTextBrowser;
class QLineEdit;
class QTableViewWithProgress;
class QProgressIndicator;
class QTableView;
class QGraphicsEffect;

namespace Octopart {
    class OctopartPartSearchModel;
}

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
private slots:
    void slotBusy();
    void slotReady();
    void slotSearch();
private:
    QTextBrowser * _logViewer;
    QLineEdit * lineEdit;
    Octopart::OctopartPartSearchModel * model;
    QTableView * _tableView;
    QProgressIndicator * _progress;
    QGraphicsEffect * _effect;
};

#endif // MAINWINDOW_H
