#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QModelIndex>
#include "octopartapi.h"

class QTextBrowser;
class QLineEdit;
class QTableViewWithProgress;
class QProgressIndicator;
class QTableView;
class QGraphicsEffect;
class QStandardItemModel;

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
    void slotPartSelected(const QModelIndex &index);
    void slotPartsGetFinished(int id, Octopart::PartFull result);
private:
    Octopart::OctopartAPI * _api;
    QTextBrowser * _logViewer;
    QLineEdit * lineEdit;
    Octopart::OctopartPartSearchModel * model;
    QTableView * _tableView;

    QProgressIndicator * _progress;
    QGraphicsEffect * _effect;

    QStandardItemModel * _datasheetsModel;
    QStandardItemModel * _offersModel;
    QStandardItemModel * _specsModel;
    QTableView * _datasheets;
    QTableView * _offers;
    QTableView * _specs;


};

#endif // MAINWINDOW_H