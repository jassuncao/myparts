#include "octopartimportdialog.h"
#include "ui_octopartimportdialog.h"
#include "octopartpartsearchmodel.h"
#include <QStandardItemModel>
#include <QProgressIndicator>
#include <QDebug>
#include <QStyledItemDelegate>
#include <QMessageBox>
#include "checkboxheaderview.h"


using namespace Octopart;

static const QString PRICE_FORMAT("%1 (%2)");

OctopartImportDialog::OctopartImportDialog(QWidget *parent) :
    QDialog(parent),
    _ui(new Ui::OctopartImportDialog)
{
    _ui->setupUi(this);
    _ui->lineEdit->setPlaceholderText("Search");        
    _ui->lineEdit->setSearchIcon(QIcon(QString(":/icons/find")));
    _ui->lineEdit->setClearIcon(QIcon(QString(":/icons/edit-clear-location-rtl")));
    connect(_ui->lineEdit, SIGNAL(returnPressed()), this, SLOT(slotDoSearch()));

    _api = new OctopartAPI("12621337",this);
    _searchModel = new OctopartPartSearchModel(_api, this);
    _searchModel->setHeaderData(OctopartPartSearchModel::ColumnMpn, Qt::Horizontal, tr("MPN"));
    _searchModel->setHeaderData(OctopartPartSearchModel::ColumnBrand, Qt::Horizontal, tr("Brand"));
    _searchModel->setHeaderData(OctopartPartSearchModel::ColumnDescription, Qt::Horizontal, tr("Description"));    

    _ui->resultsTableView->setModel(_searchModel);
    _ui->resultsTableView->setSelectionMode(QAbstractItemView::SingleSelection);
    _ui->resultsTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    _ui->resultsTableView->setColumnHidden(OctopartPartSearchModel::ColumnUid,true);
    _ui->resultsTableView->setColumnHidden(OctopartPartSearchModel::ColumnUrl,true);
    _ui->resultsTableView->horizontalHeader()->setStretchLastSection(true);
    _ui->resultsTableView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    _ui->resultsTableView->verticalHeader()->setVisible(false);
    _progress = new QProgressIndicator(this);
    _ui->resultsTableView->setCornerWidget(_progress);
    _ui->resultsTableView->setCornerButtonEnabled(false);
    _progress->setVisible(true);

    initOffersTable();
    initDatasheetsTable();
    initParametersTable();

    connect(_searchModel, SIGNAL(busy()), this, SLOT(slotBusy()));
    connect(_searchModel, SIGNAL(ready()), this, SLOT(slotReady()));
    connect(_searchModel, SIGNAL(error(QString)), this, SLOT(slotError(QString)));
    connect(_searchModel, SIGNAL(searchFinished(int)), this, SLOT(slotSearchFinished(int)));
    connect(_ui->resultsTableView, SIGNAL(activated(QModelIndex)), this, SLOT(slotPartSelected(QModelIndex)));

    _ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Import"));
    _ui->buttonBox->button(QDialogButtonBox::Ok)->setAutoDefault(false);

    connect(_ui->abortButton, SIGNAL(clicked()), _api, SLOT(abortAll()));
    connect(_api, SIGNAL(requestFinished(Octopart::PartFullResponse)), this, SLOT(slotRequestFinished(Octopart::PartFullResponse)));
    setModal(true);
}

OctopartImportDialog::~OctopartImportDialog()
{
    delete _ui;
}

void OctopartImportDialog::initOffersTable()
{
    CheckboxHeaderView *checkboxHeader = new CheckboxHeaderView(Qt::Horizontal, _ui->offersTable);
    _ui->offersTable->setHorizontalHeader(checkboxHeader);
    checkboxHeader->setResizeMode(0, QHeaderView::ResizeToContents);
    connect(checkboxHeader, SIGNAL(stateChanged(Qt::CheckState)), this, SLOT(slotToggleOfferCheckboxes(Qt::CheckState)));
}

void OctopartImportDialog::initDatasheetsTable()
{
    CheckboxHeaderView *checkboxHeader = new CheckboxHeaderView(Qt::Horizontal, _ui->datasheetsTable);
    _ui->datasheetsTable->setHorizontalHeader(checkboxHeader);
    checkboxHeader->setResizeMode(0, QHeaderView::ResizeToContents);
    connect(checkboxHeader, SIGNAL(stateChanged(Qt::CheckState)), this, SLOT(slotToggleDatasheetCheckboxes(Qt::CheckState)));
}

void OctopartImportDialog::initParametersTable()
{
    CheckboxHeaderView *checkboxHeader = new CheckboxHeaderView(Qt::Horizontal, _ui->parametersTable);
    _ui->parametersTable->setHorizontalHeader(checkboxHeader);
    checkboxHeader->setResizeMode(0, QHeaderView::ResizeToContents);
    checkboxHeader->setStretchLastSection(true);
    connect(checkboxHeader, SIGNAL(stateChanged(Qt::CheckState)), this, SLOT(slotToggleParameterCheckboxes(Qt::CheckState)));

}

void OctopartImportDialog::search(const QString &searchTerm)
{
    _ui->lineEdit->setText(searchTerm);
    slotDoSearch();
}

void OctopartImportDialog::on_searchButton_clicked()
{
    slotDoSearch();
}

void OctopartImportDialog::slotDoSearch()
{
    QString text = _ui->lineEdit->text();
    if(text.isEmpty()){
        return;
    }
    _searchModel->searchByMpn(text);
    _ui->resultsTableView->setEnabled(false);
    _ui->datasheetsTable->setEnabled(false);
    _ui->offersTable->setEnabled(false);
    _ui->parametersTable->setEnabled(false);
}

void OctopartImportDialog::slotBusy()
{
    _progress->startAnimation();
    _ui->abortButton->setVisible(true);
}

void OctopartImportDialog::slotReady()
{
    _progress->stopAnimation();
    _ui->resultsTableView->setEnabled(true);
    _ui->datasheetsTable->setEnabled(true);
    _ui->offersTable->setEnabled(true);
    _ui->parametersTable->setEnabled(true);
    _ui->abortButton->setVisible(false);    
}

void OctopartImportDialog::slotError(const QString &error)
{
    const QString errorMsg = QString(tr("Request failed with error:\n'%1'")).arg(error);
    QMessageBox::warning(this, "Error", errorMsg);

}

void OctopartImportDialog::slotSearchFinished(int hits)
{
    _ui->statusLabel->setText(tr("Results found: %1").arg(hits));
}

void OctopartImportDialog::slotPartSelected(const QModelIndex &index)
{
    if(!index.isValid()){
        _ui->datasheetsTable->clear();
    }
    else{
        const int row = index.row();
        const QModelIndex & uidIndex = index.model()->index(row, OctopartPartSearchModel::ColumnUid);
        const QString &uid = uidIndex.data(Qt::DisplayRole).toString();
        Octopart::Part * part = _partsCache.object(uid);
        if(part){
            showPartDetails(part);
        }
        else {
            _api->partsGet(uid);
        }
    }
}

void OctopartImportDialog::slotRequestFinished(const Octopart::PartFullResponse& response)
{
    Octopart::Part * part = new Octopart::Part(response.result);
    _partsCache.insert(part->uid(), part);
    showPartDetails(part);
}

static void toggleTableCheckboxes(QTableWidget * table, Qt::CheckState newState)
{
    const int rowCount = table->rowCount();
    for(int row=0; row<rowCount; ++row){
        table->item(row, 0)->setCheckState(newState);
    }

}

void OctopartImportDialog::slotToggleDatasheetCheckboxes(Qt::CheckState state)
{
    toggleTableCheckboxes(_ui->datasheetsTable, state);
}

void OctopartImportDialog::slotToggleParameterCheckboxes(Qt::CheckState state)
{
    toggleTableCheckboxes(_ui->parametersTable, state);
}

void OctopartImportDialog::slotToggleOfferCheckboxes(Qt::CheckState state)
{
    toggleTableCheckboxes(_ui->offersTable, state);
}

static QString formatOfferPrice(const Offer &offer)
{
    if(offer.moqPrice().isValid()){
        if(!offer.currency().isEmpty()){
            return PRICE_FORMAT.arg(offer.moqPrice().toString()).arg(offer.currency());
        }
        return offer.moqPrice().toString();
    }
    return QString();

}

void OctopartImportDialog::showPartDetails(Part *part)
{
    loadOffersTable(part->offers());
    loadDatasheetTable(part->datasheets());
    loadParametersTable(part->specs());
}

void OctopartImportDialog::loadOffersTable(const QList<Offer> &offers)
{
    _ui->offersTable->clearContents();
    _ui->offersTable->setRowCount(offers.size());
    for(int row=0; row<offers.size(); ++row){
        const Offer offer = offers.at(row);
        QString price = formatOfferPrice(offer);

        QTableWidgetItem *item = new QTableWidgetItem();
        item->data(Qt::CheckStateRole);
        item->setCheckState(Qt::Unchecked);
        _ui->offersTable->setItem(row, 0, item);
        _ui->offersTable->setItem(row, 1, new QTableWidgetItem(offer.seller().name()));
        _ui->offersTable->setItem(row, 2, new QTableWidgetItem(offer.sku()));
        _ui->offersTable->setItem(row, 3, new QTableWidgetItem(offer.packaging()));
        _ui->offersTable->setItem(row, 4, new QTableWidgetItem(offer.moq().toString()));
        _ui->offersTable->setItem(row, 5, new QTableWidgetItem(price));

    }
}

void OctopartImportDialog::loadDatasheetTable(const QList<Datasheet> &datasheets)
{
    _ui->datasheetsTable->clearContents();
    _ui->datasheetsTable->setRowCount(datasheets.size());
    for(int row=0; row<datasheets.size(); ++row){
        const Datasheet datasheet = datasheets.at(row);

        QTableWidgetItem *item = new QTableWidgetItem();
        item->data(Qt::CheckStateRole);
        item->setCheckState(Qt::Unchecked);
        _ui->datasheetsTable->setItem(row, 0, item);
        _ui->datasheetsTable->setItem(row, 1, new QTableWidgetItem(datasheet.sourceName()));
        _ui->datasheetsTable->setItem(row, 2, new QTableWidgetItem(QString::number(datasheet.numPages())));
        _ui->datasheetsTable->setItem(row, 3, new QTableWidgetItem(datasheet.mimetype()));
    }
}

void OctopartImportDialog::loadParametersTable(const QList<PartSpec> &specs)
{
    _ui->parametersTable->clearContents();
    _ui->parametersTable->setRowCount(specs.size());
    for(int row=0; row<specs.size(); ++row){
        const PartSpec spec = specs.at(row);
        QTableWidgetItem *item = new QTableWidgetItem();
        item->data(Qt::CheckStateRole);
        item->setCheckState(Qt::Unchecked);
        _ui->parametersTable->setItem(row, 0, item);
        _ui->parametersTable->setItem(row, 1, new QTableWidgetItem(spec.name()));
        _ui->parametersTable->setItem(row, 2, new QTableWidgetItem(spec.displayValue()));
        /*
        _ui->parametersTable->setItem(row, 3, new QTableWidgetItem(spec.unitName()));
        _ui->parametersTable->setItem(row, 1, new QTableWidgetItem(spec.key()));
        _ui->parametersTable->setItem(row, 2, new QTableWidgetItem(spec.datatype()));
        */
    }
}


