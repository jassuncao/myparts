#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "optionsdialog.h"
#include <QSettings>
#include <QtGui>
#include "widgets/partconditionwidget.h"
#include "widgets/partunitswidget.h"
#include "models/storagetreemodel.h"
#include "footprintmanagerwidget.h"
#include "unitmanagerwidget.h"
#include "distributormanagerwidget.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    readSettings();
    QVector<QVariant> headerData(3);
    headerData.append("Name");
    headerData.append("Description");
    headerData.append("ID");

    StorageTreeModel * storageTreeModel = new StorageTreeModel(headerData, this);
    storageTreeModel->select();
    ui->storageManagerWidget->setModel(storageTreeModel);

    FootprintManagerWidget* footprintManager = new FootprintManagerWidget(this);
    ui->tabWidget->addTab(footprintManager, tr("Footprints"));

    UnitManagerWidget* unitManager = new UnitManagerWidget(this);
    ui->tabWidget->addTab(unitManager, tr("Units"));

    DistributorManagerWidget* distributorManager = new DistributorManagerWidget(this);
    ui->tabWidget->addTab(distributorManager, QIcon(":/icons/distributor"), tr("Distributors"));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    writeSettings();
    event->accept();
}

void MainWindow::on_actionEdit_part_units_triggered()
{
    OptionsDialog dlg(new PartUnitsWidget(), this);
    dlg.exec();
}

void MainWindow::readSettings()
{
     QSettings settings;
     QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
     QSize size = settings.value("size", QSize(400, 400)).toSize();
     resize(size);
     move(pos);
}

void MainWindow::writeSettings()
{
     QSettings settings;
     settings.setValue("pos", pos());
     settings.setValue("size", size());
}

void MainWindow::on_actionPart_conditions_triggered()
{
    OptionsDialog dlg(new PartConditionWidget(), this);
    dlg.exec();
}

void MainWindow::on_actionActionPartParameters_triggered()
{        

}
