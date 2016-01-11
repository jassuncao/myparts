#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "optionsdialog.h"
#include <QSettings>
#include <QtGui>
#include "models/storagetreemodel.h"
#include "packagemanagerwidget.h"
#include "distributormanagerwidget.h"
#include "partsmanagerview.h"
#include "distributorsmanagerview.h"
#include "editormanagerview.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    readSettings();
    /*
    QVector<QVariant> headerData(3);
    headerData.append("Name");
    headerData.append("Description");
    headerData.append("ID");
    */

    EditorManagerView * distributorsView = new EditorManagerView(new DistributorManagerHelper, this);
    EditorManagerView * manufacturersView = new EditorManagerView(new ManufacturerManagerHelper, this);
    EditorManagerView * packageView = new EditorManagerView(new PackageManagerHelper, this);    

    ui->centralWidget->insertTab(0,new PartsManagerView(this), QIcon(QString::fromUtf8(":/largeIcons/page_parts")), tr("Parts"));
    ui->centralWidget->setTabEnabled(0, true);

    ui->centralWidget->insertTab(1, distributorsView, QIcon(QString::fromUtf8(":/largeIcons/options_distributors")), tr("Distributors"));
    ui->centralWidget->setTabEnabled(1, true);

    ui->centralWidget->insertTab(2, manufacturersView, QIcon(QString::fromUtf8(":/largeIcons/options_manufacturers")), tr("Manufacturers"));
    ui->centralWidget->setTabEnabled(2, true);

    ui->centralWidget->insertTab(3, packageView, QIcon(QString::fromUtf8(":/largeIcons/page_packages")), tr("Packages"));
    ui->centralWidget->setTabEnabled(3, true);


    //qDebug("MIN WIDTH %d", ui->centralWidget->minimumSizeHint().width());
    ui->centralWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    ui->centralWidget->setMinimumWidth(ui->centralWidget->minimumSizeHint().width());


    //ui->centralWidget->insertTab(2,new PartManagerForm(this), QIcon(QString::fromUtf8(":/largeIcons/page_parts")), tr("Parts"));
    //ui->centralWidget->insertTab(1,new PackageManagerWidget(this), QIcon(QString::fromUtf8(":/largeIcons/page_packages")), tr("Packages"));
    //ui->stackedWidget->addWidget(new PartManagerForm(this));
    /*
    StorageTreeModel * storageTreeModel = new StorageTreeModel(headerData, this);
    storageTreeModel->select();
    ui->storageManagerWidget->setModel(storageTreeModel);

    PackageManagerWidget* packageManager = new PackageManagerWidget(this);
    ui->tabWidget->addTab(packageManager, tr("Packages"));

    UnitManagerWidget* unitManager = new UnitManagerWidget(this);
    ui->tabWidget->addTab(unitManager, tr("Units"));

    DistributorManagerWidget* distributorManager = new DistributorManagerWidget(this);
    ui->tabWidget->addTab(distributorManager, QIcon(":/icons/distributor"), tr("Distributors"));
*/
    connect(ui->actionPreferences, SIGNAL(triggered()), this, SLOT(slotEditPreferences()));
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

void MainWindow::slotEditPreferences()
{
    OptionsDialog dlg(this);
    dlg.exec();
}
