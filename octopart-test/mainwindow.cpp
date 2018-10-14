#include "mainwindow.h"

#include <QTableView>
#include <QGridLayout>
#include <QTextBrowser>
#include <QLineEdit>
#include <QPushButton>
#include "octopartpartsearchmodel.h"
#include "qtableviewwithprogress.h"
#include <QProgressIndicator>
#include <QGraphicsBlurEffect>
#include <QGraphicsOpacityEffect>
#include <QGraphicsDropShadowEffect>
#include <QScrollBar>
#include <QStandardItemModel>
#include "qsearchlineedit2.h"

using namespace Octopart;

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
{
    _api = new OctopartAPI("12621337",this);
    model = new OctopartPartSearchModel(_api, this);
    _tableView = new QTableView;
    _tableView->setModel(model);
    model->setHeaderData(OctopartPartSearchModel::ColumnMpn, Qt::Horizontal, "MPN");

    lineEdit = new QSearchLineEdit2(this);
    lineEdit->setPlaceholderText("Search");

    _progress = new QProgressIndicator;
    _logViewer = new QTextBrowser;
    _logViewer->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred));

    QPushButton * search = new QPushButton("Search");

    _datasheetsModel = new QStandardItemModel;
    QStringList datasheetsLabels;
    datasheetsLabels << tr("Source") << tr("Type") << tr("Pages");
    _datasheetsModel->setHorizontalHeaderLabels(datasheetsLabels);
    _datasheets = new QTableView;
    _datasheets->setModel(_datasheetsModel);

    _offersModel = new QStandardItemModel;
    QStringList offersLabels;
    offersLabels << tr("Seller") << tr("SKU") << tr("Packaging") << tr("MOQ") << tr("Price");
    _offersModel->setHorizontalHeaderLabels(offersLabels);
    _offers = new QTableView;
    _offers->setModel(_offersModel);

    _specsModel = new QStandardItemModel;
    QStringList specsLabels;
    specsLabels << tr("Name") << tr("Key") << tr("Datatype") << tr("Value") << tr("Unit");
    _specsModel->setHorizontalHeaderLabels(specsLabels);
    _specs = new QTableView;
    _specs->setModel(_specsModel);

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(lineEdit, 0, 0);
    layout->addWidget(search, 0, 1);
    layout->addWidget(_tableView, 1, 0, 1, 2);
    layout->addWidget(_logViewer, 2, 0, 1, 2);
    layout->addWidget(_progress, 3, 0);
    layout->addWidget(_datasheets, 4, 0, 1, 2);
    layout->addWidget(_offers, 5, 0, 1, 2);
    layout->addWidget(_specs, 6, 0, 1, 2);


    setLayout(layout);
    setWindowTitle(tr("Octopart API Test"));    
    connect(search, SIGNAL(clicked(bool)), this, SLOT(slotSearch()));
    //connect(lineEdit, SIGNAL(textChanged(QString)), model, SLOT(searchByMpn(QString)));
    //connect(lineEdit, SIGNAL(textChanged(QString)), _logViewer, SLOT(clear()));
    connect(model, SIGNAL(busy()), this, SLOT(slotBusy()));
    connect(model, SIGNAL(ready()), this, SLOT(slotReady()));
    connect(_tableView, SIGNAL(activated(QModelIndex)), this, SLOT(slotPartSelected(QModelIndex)));
    //connect(_api, SIGNAL(requestFinished(Octopart::RequestResult)), this, SLOT(slotRequestFinished(Octopart::RequestResult)));
    //connect(_api, SIGNAL(partsGetFinished(int,Octopart::PartFull)), this, SLOT(slotPartsGetFinished(int,Octopart::PartFull)));
    connect(_api, SIGNAL(requestFinished(Octopart::PartFullResponse)),this, SLOT(slotPartsGetFinished(Octopart::PartFullResponse)));
}

MainWindow::~MainWindow()
{
}

void MainWindow::slotSearch()
{
    _logViewer->clear();
    //model->searchByText(lineEdit->text());
    model->searchByMpn(lineEdit->text());
}

void MainWindow::slotBusy()
{        
    _progress->startAnimation();
    _logViewer->append("Busy...");
    lineEdit->setBusy(true);
}

void MainWindow::slotReady()
{  
    _progress->stopAnimation();
    lineEdit->setBusy(false);
    _logViewer->append("Ready.");    
}

void MainWindow::slotPartSelected(const QModelIndex &index)
{
    if(!index.isValid()){
        _datasheetsModel->clear();
    }
    else{
        QString uid = model->index(index.row(), OctopartPartSearchModel::ColumnUid).data().toString();
        _api->partsGet(uid);
    }
}

void MainWindow::slotPartsGetFinished(const Octopart::PartFullResponse& response)
{

    QList<Datasheet> datasheets = response.result.datasheets();
    _datasheetsModel->setRowCount(0);

    for(int i=0; i<datasheets.size(); ++i){
        const Datasheet datasheet = datasheets.at(i);
        QList<QStandardItem*> items;
        items.append(new QStandardItem(datasheet.sourceName()));
        items.append(new QStandardItem(datasheet.mimetype()));
        items.append(new QStandardItem(QString::number(datasheet.numPages())));
        _datasheetsModel->appendRow(items);
    }

    QList<Offer> offers = response.result.offers();
    _offersModel->setRowCount(0);

    for(int i=0; i<offers.size(); ++i){
        const Offer offer = offers.at(i);
        QList<QStandardItem*> items;
        items.append(new QStandardItem(offer.seller().name()));
        items.append(new QStandardItem(offer.sku()));
        items.append(new QStandardItem(offer.packaging()));
        items.append(new QStandardItem(offer.moq().toString()));
        items.append(new QStandardItem(offer.moqPrice().toString()));
        _offersModel->appendRow(items);
    }

    QList<PartSpec> specs = response.result.specs();
    _specsModel->setRowCount(0);

    for(int i=0; i<specs.size(); ++i){
        const PartSpec spec = specs.at(i);
        QList<QStandardItem*> items;
        items.append(new QStandardItem(spec.name()));
        items.append(new QStandardItem(spec.key()));
        items.append(new QStandardItem(spec.datatype()));
        items.append(new QStandardItem(spec.displayValue()));
        items.append(new QStandardItem(spec.unitName()));
        _specsModel->appendRow(items);
    }

}
