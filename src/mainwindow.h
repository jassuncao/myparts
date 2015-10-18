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
    void on_actionEdit_part_units_triggered();

    void on_actionPart_conditions_triggered();

    void on_actionActionPartParameters_triggered();
    void slotEditPreferences();
    void slotEditManufacturers();

private:
    void readSettings();
    void writeSettings();

    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
