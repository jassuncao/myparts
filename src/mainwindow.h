#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class ModelsProvider;

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
    void on_actionResistor_triggered();

private:
    void readSettings();
    void writeSettings();
    ModelsProvider * _modelsProvider;
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
