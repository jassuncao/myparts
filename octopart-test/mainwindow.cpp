#include "mainwindow.h"

#include <QTableView>
#include <QGridLayout>
#include <QTextBrowser>
#include <QLineEdit>
#include <QPushButton>
#include "octopartpartsearchmodel.h"
#include "octopartapi.h"
#include "qtableviewwithprogress.h"
#include <QProgressIndicator>
#include <QGraphicsBlurEffect>
#include <QGraphicsOpacityEffect>
#include <QGraphicsDropShadowEffect>
#include <QScrollBar>


using namespace Octopart;

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
{
    OctopartAPI * api = new OctopartAPI("12621337",this);
    model = new OctopartPartSearchModel(api, this);
    _tableView = new QTableView;
    _tableView->setModel(model);
    model->setHeaderData(OctopartPartSearchModel::ColumnMpn, Qt::Horizontal, "MPN");

    lineEdit = new QLineEdit;

    _progress = new QProgressIndicator;
    _logViewer = new QTextBrowser;
    _logViewer->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred));

    QPushButton * search = new QPushButton("Search");

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(lineEdit, 0, 0);
    layout->addWidget(search, 0, 1);
    layout->addWidget(_tableView, 1, 0, 1, 2);
    layout->addWidget(_logViewer, 2, 0, 1, 2);
    layout->addWidget(_progress, 3, 0);
    setLayout(layout);
    setWindowTitle(tr("Octopart API Test"));    
    connect(search, SIGNAL(clicked(bool)), this, SLOT(slotSearch()));
    //connect(lineEdit, SIGNAL(textChanged(QString)), model, SLOT(searchByMpn(QString)));
    //connect(lineEdit, SIGNAL(textChanged(QString)), _logViewer, SLOT(clear()));
    connect(model, SIGNAL(busy()), this, SLOT(slotBusy()));
    connect(model, SIGNAL(ready()), this, SLOT(slotReady()));
}

MainWindow::~MainWindow()
{
}

void MainWindow::slotSearch()
{
    _logViewer->clear();
    model->searchByMpn(lineEdit->text());
}

void MainWindow::slotBusy()
{        
    _progress->startAnimation();
    _logViewer->append("Busy...");
}

void MainWindow::slotReady()
{  
    _progress->stopAnimation();
    _logViewer->append("Ready.");    
}
