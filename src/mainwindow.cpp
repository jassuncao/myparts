#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "optionsdialog.h"
#include <QSettings>
#include <QtGui>
#include "models/storagetreemodel.h"
#include "partsmanagerview.h"
#include "editormanagerview.h"
#include "dialogs/quickaddresistordialog.h"
#include "models/modelsrepository.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);    
    _modelsProvider = new ModelsRepository(this);
    _modelsProvider->initModels();   
    _partsManagerView = new PartsManagerView(_modelsProvider, this);
    EditorManagerView * distributorsView = new EditorManagerView(new DistributorManagerHelper, _modelsProvider->distributorModel(), this);
    EditorManagerView * manufacturersView = new EditorManagerView(new ManufacturerManagerHelper, _modelsProvider->manufacturerModel(), this);
    EditorManagerView * packageView = new EditorManagerView(new PackageManagerHelper, _modelsProvider->packageModel(), this);

    ui->centralWidget->insertTab(0, _partsManagerView, QIcon(QString::fromUtf8(":/largeIcons/page_parts")), tr("Parts"));
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
    readSettings();
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
     _partsManagerView->readSettings(settings);
     resize(size);
     move(pos);
}

void MainWindow::writeSettings()
{
     QSettings settings;
     settings.setValue("pos", pos());
     settings.setValue("size", size());
     _partsManagerView->writeSettings(settings);
}

void MainWindow::slotEditPreferences()
{
    OptionsDialog dlg(_modelsProvider, this);
    dlg.exec();
}

void MainWindow::on_actionResistor_triggered()
{
    QuickAddResistorDialog * dlg = new QuickAddResistorDialog(_modelsProvider, this);
    dlg->setSelectedCategory(_partsManagerView->selectedCategory());
    dlg->setSelectedStorage(_partsManagerView->selectedStorage());
    dlg->exec();
    dlg->deleteLater();
}
