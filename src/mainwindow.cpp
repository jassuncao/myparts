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
#include "dialogs/manufacturersdialog.h"
#include "partmanagerform.h"
#include "partsmanagerview.h"
#include "distributorsmanagerview.h"
#include "companymanagerview.h"

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

    //DistributorManagerHelper  distributorHelper;
    CompanyManagerView * distributorsView = new CompanyManagerView(new DistributorManagerHelper, this);
    CompanyManagerView * manufacturersView = new CompanyManagerView(new ManufacturerManagerHelper, this);
    CompanyManagerView * footprintView = new CompanyManagerView(new FootprintManagerHelper, this);
    //ui->centralWidget->insertTab(0,new PartManagerForm(this), QIcon(QString::fromUtf8(":/largeIcons/page_parts")), tr("Parts"));
    ui->centralWidget->insertTab(0,new PartsManagerView(this), QIcon(QString::fromUtf8(":/largeIcons/page_parts")), tr("Parts"));
    ui->centralWidget->insertTab(1,new FootprintManagerWidget(this), QIcon(QString::fromUtf8(":/largeIcons/page_footprints")), tr("Footprints"));
    ui->centralWidget->insertTab(2,new PartManagerForm(this), QIcon(QString::fromUtf8(":/largeIcons/page_parts")), tr("Parts"));    
    ui->centralWidget->insertTab(3, distributorsView, QIcon(QString::fromUtf8(":/largeIcons/options_distributors")), tr("Distributors"));
    ui->centralWidget->insertTab(4, manufacturersView, QIcon(QString::fromUtf8(":/largeIcons/options_manufacturers")), tr("Manufacturers"));
    ui->centralWidget->insertTab(5, footprintView, QIcon(QString::fromUtf8(":/largeIcons/page_footprints")), tr("Footprints"));
    ui->centralWidget->setTabEnabled(0, true);
    ui->centralWidget->setTabEnabled(1, true);
    ui->centralWidget->setTabEnabled(2, true);
    ui->centralWidget->setTabEnabled(3, true);
    ui->centralWidget->setTabEnabled(4, true);
    ui->centralWidget->setTabEnabled(5, true);
    //ui->stackedWidget->addWidget(new PartManagerForm(this));
    /*
    StorageTreeModel * storageTreeModel = new StorageTreeModel(headerData, this);
    storageTreeModel->select();
    ui->storageManagerWidget->setModel(storageTreeModel);

    FootprintManagerWidget* footprintManager = new FootprintManagerWidget(this);
    ui->tabWidget->addTab(footprintManager, tr("Footprints"));

    UnitManagerWidget* unitManager = new UnitManagerWidget(this);
    ui->tabWidget->addTab(unitManager, tr("Units"));

    DistributorManagerWidget* distributorManager = new DistributorManagerWidget(this);
    ui->tabWidget->addTab(distributorManager, QIcon(":/icons/distributor"), tr("Distributors"));
*/
    connect(ui->actionPreferences, SIGNAL(triggered()), this, SLOT(slotEditPreferences()));
    connect(ui->actionManufacturers, SIGNAL(triggered()), this, SLOT(slotEditManufacturers()));
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
    OptionsDialog dlg(this);
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
    OptionsDialog dlg(this);
    //OptionsDialog dlg(new PartUnitsWidget(), this);
    dlg.exec();
}

void MainWindow::on_actionActionPartParameters_triggered()
{        

}

void MainWindow::slotEditPreferences()
{
    OptionsDialog dlg(this);
    dlg.exec();
}

void MainWindow::slotEditManufacturers()
{
    ManufacturersDialog dlg(this);
    dlg.exec();
}
