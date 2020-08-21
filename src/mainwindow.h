#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class ModelsRepository;
class PartsManagerView;

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
    void on_actionCapacitor_triggered();
    void on_actionMerge_part_triggered();

private:
    void readSettings();
    void writeSettings();
    ModelsRepository * _modelsProvider;
    Ui::MainWindow *ui;
    PartsManagerView* _partsManagerView;
};

#endif // MAINWINDOW_H
