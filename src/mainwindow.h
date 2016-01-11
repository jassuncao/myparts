#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
protected:
    void closeEvent(QCloseEvent *event);
private slots:    
    void slotEditPreferences();
private:
    void readSettings();
    void writeSettings();

    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
