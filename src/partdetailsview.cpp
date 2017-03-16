#include "partdetailsview.h"
#include "ui_partdetailsview.h"
#include "models/partssqltablemodel.h"
#include "widgets/priceitemdelegate.h"
#include "widgets/datetimedelegate.h"
#include "models/partstocklogtablemodel.h"
#include "models/stocktableformatproxymodel.h"
#include "models/partstocktablemodel2.h"
#include "models/modelsrepository.h"
#include "dialogs/addstockdialog.h"
#include "dialogs/addstockentrydialog.h"
#include "dialogs/removestockdialog.h"
#include <QDebug>
#include <QPushButton>
#include <QDataWidgetMapper>
#include <QIdentityProxyModel>
#include <QAbstractItemModel>
#include <QDateTime>
#include <QItemSelectionModel>

class PartDetailsDelegate : public QStyledItemDelegate {
public:
    explicit PartDetailsDelegate(QObject *parent);
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
};

PartDetailsDelegate::PartDetailsDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{
}

void PartDetailsDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QLabel * label = dynamic_cast<QLabel*>(editor);
    if(label && index.isValid()){
        int column = index.column();
        QString text;
        if(column==PartsSqlTableModel::ColumnCreateDate){
            QDateTime date = index.data(Qt::EditRole).toDateTime();
            QLocale locale;
            text = locale.toString(date, QLocale::ShortFormat);
        }
        else if(column==PartsSqlTableModel::ColumnActualStock || column==PartsSqlTableModel::ColumnMinStock){
            QVariant stockVar = index.data(Qt::EditRole);
            if(stockVar.isValid()){
                QVariant partUnitVar = index.model()->index(index.row(), PartsSqlTableModel::ColumnPartUnit).data();
                QString posfix = partUnitVar.isValid() ? partUnitVar.toString() : QString();
                text = QString("%1 %2").arg(stockVar.toInt()).arg(posfix);
            }            
        }
        else{
            text = index.data(Qt::DisplayRole).toString();
        }
        label->setText(text);
    }
    else {
        setEditorData(editor, index);
    }
}

PartDetailsView::PartDetailsView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PartDetailsView),
    _modelsRepository(0),
    _partsModel(0),
    _partStockLogModel(0),
    _partStockModel(0)
{
    ui->setupUi(this);
    //By default the Manhattan style adjusts the size of the first tab. This is causing a visual artifact
    //If the "noTabBarShapeAdjustment" property is set, the adjustment won't be applied
    ui->tabWidget->setProperty("noTabBarShapeAdjustment","xx");

    ui->partStockHistoryTable->setWordWrap(false);
    _widgetMapper = new QDataWidgetMapper(this);
    _widgetMapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
    _widgetMapper->setItemDelegate(new PartDetailsDelegate(this));

    _partStockModel = PartStockTableModel2::createNew(this);
    ui->partStockOverviewTable->setModel(_partStockModel);
    ui->partStockOverviewTable->horizontalHeader()->swapSections(PartStockTableModel2::ColumnStorage, PartStockTableModel2::ColumnCondition);
    ui->partStockOverviewTable->horizontalHeader()->moveSection(PartStockTableModel2::ColumnQuantity, 0);
    ui->partStockOverviewTable->hideColumn(PartStockTableModel2::ColumnLastUpdate);
    ui->partStockOverviewTable->resizeColumnsToContents();

    _partStockLogModel = PartStockLogTableModel::createNew(this);

    StockTableFormatProxyModel * proxyModel = new StockTableFormatProxyModel(this);
    proxyModel->setSourceModel(_partStockLogModel);

    ui->partStockHistoryTable->setItemDelegateForColumn(PartStockLogTableModel::ColumnLastUpdate, new DateDelegate(this));
    ui->partStockHistoryTable->setItemDelegateForColumn(PartStockLogTableModel::ColumnPrice, new PriceItemDelegate(false, this));
    //ui->partStockHistoryTable->setItemDelegateForColumn(PartStockTableModel::ColumnChange, new StockChangeDelegate(this));
    ui->partStockHistoryTable->setModel(proxyModel);
    ui->partStockHistoryTable->resizeColumnsToContents();


    QPushButton * addStockButton = new QPushButton(QIcon(QString::fromUtf8(":/icons/addStock")), tr("Add Stock"),ui->partActionsButtonBox);
    QPushButton * removeStockButton = new QPushButton(QIcon(QString::fromUtf8(":/icons/removeStock")), tr("Remove Stock"),ui->partActionsButtonBox);
    QPushButton * editPartButton = new QPushButton(QIcon(QString::fromUtf8(":/icons/editPart")), tr("Edit Part"),ui->partActionsButtonBox);   

    ui->partActionsButtonBox->addButton(editPartButton, QDialogButtonBox::ActionRole);
    ui->stockActionsButtonBox->addButton(addStockButton, QDialogButtonBox::ActionRole);
    ui->stockActionsButtonBox->addButton(removeStockButton, QDialogButtonBox::DestructiveRole);   
    connect(editPartButton, SIGNAL(clicked()), this, SIGNAL(editPartSelected()));
    connect(addStockButton, SIGNAL(clicked()), this, SLOT(onAddStock()));
    connect(removeStockButton, SIGNAL(clicked()), this, SLOT(onRemoveStock()));

    _partStockLogModel->setCurrentPartId(QVariant());
    _partStockLogModel->select();

    _partStockModel->setCurrentPartId(QVariant());
    _partStockModel->select();
}

PartDetailsView::~PartDetailsView()
{
    delete ui;
}


void PartDetailsView::setModelsRepository(ModelsRepository * repository)
{
    _modelsRepository = repository;
    setPartsModel(repository->partsModel());
}


void PartDetailsView::setPartsModel(PartsSqlTableModel * model)
{
    if(_partsModel==model)
        return;
    _partsModel = model;
    _widgetMapper->setModel(model);
    _widgetMapper->addMapping(ui->partNameLabel, PartsSqlTableModel::ColumnName);
    _widgetMapper->addMapping(ui->partDescriptionLabel, PartsSqlTableModel::ColumnDescription);
    _widgetMapper->addMapping(ui->partCategoryLabel, PartsSqlTableModel::ColumnCategoryName);
    _widgetMapper->addMapping(ui->partStockLevelLabel, PartsSqlTableModel::ColumnActualStock);
    _widgetMapper->addMapping(ui->partMinStockLabel, PartsSqlTableModel::ColumnMinStock);
    _widgetMapper->addMapping(ui->partCustomNumberLabel, PartsSqlTableModel::ColumnCustomPartNumber);
    _widgetMapper->addMapping(ui->partCommentLabel, PartsSqlTableModel::ColumnComment);
    _widgetMapper->addMapping(ui->partCreateDateLabel, PartsSqlTableModel::ColumnCreateDate);
    _widgetMapper->addMapping(ui->partPackageLabel, PartsSqlTableModel::ColumnPackageName);
    setCurrentIndex(QModelIndex());
}

void PartDetailsView::onAddStock()
{    
    /*
    AddStockDialog dlg(this);
    dlg.setPartUnit(partUnit.toString());
    if(dlg.exec()!=QDialog::Accepted)
        return;
    _partsModel->database().transaction();
    _partStockLogModel->appendRow(dlg.getStockChange(), dlg.getPartPrice(), dlg.getComment());
    _partsModel->updatePartStock(_currentIndex, dlg.getStockChange());
    _partsModel->updatePartAvgPrice(_currentIndex, dlg.getPartPrice());

    if(_partStockLogModel->submitAll() && _partsModel->submitAll()){
        _partsModel->database().commit();
    }
    else{
        _partsModel->database().rollback();
        //TODO: Display some warning
        qWarning("Failed to insert stock change (addition) in database");
    }
*/

    AddStockEntryDialog dlg(_modelsRepository, this);

    QVariant partUnit = _partsModel->index(_currentIndex.row(), PartsSqlTableModel::ColumnPartUnit).data();
    dlg.setPartUnit(partUnit.toString());
    QModelIndex index = ui->partStockOverviewTable->currentIndex();
    if(index.isValid()){
        QVariant conditionId = _partStockModel->index(index.row(), PartStockTableModel2::ColumnCondition).data(Qt::EditRole);
        dlg.setSelectedCondition(conditionId);
        QVariant storageId = _partStockModel->index(index.row(), PartStockTableModel2::ColumnStorage).data(Qt::EditRole);
        dlg.setSelectedStorage(storageId);
    }
    int res = dlg.exec();
    if(res == AddStockEntryDialog::Accepted){
        QVariant condition = dlg.selectedCondition();
        QVariant storage = dlg.selectedStorage();
        int quantity = dlg.stockChange();
        double partPrice = dlg.partPrice();
        QString comment = dlg.comment();

        _partsModel->database().transaction();

        _partStockModel->insertOrUpdateRow(condition, storage, quantity);
        _partStockLogModel->appendRow(quantity, partPrice, comment);


        QVariant totalStock = _partStockModel->computeCurrentStock();
        QModelIndex actualStockIndex = _partsModel->index(_currentIndex.row(), PartsSqlTableModel::ColumnActualStock);
        _partsModel->setData(actualStockIndex, totalStock);

        if(partPrice > 0){
            QVariant avgPrice = _partStockLogModel->computeAveragePrice();
            QModelIndex avgPriceIndex = _partsModel->index(_currentIndex.row(), PartsSqlTableModel::ColumnAvgPrice);
            _partsModel->setData(avgPriceIndex, avgPrice);
        }

        if(_partStockModel->submitAll() && _partStockLogModel->submitAll() && _partsModel->submitAll()){
            _partsModel->database().commit();
        }
        else{
            _partsModel->database().rollback();
            //TODO: Display some warning
            qWarning("Failed to insert stock change (removal) in database");
        }
    }

}

void PartDetailsView::onRemoveStock()
{
    QModelIndex stockIndex = ui->partStockOverviewTable->currentIndex();
    if(!stockIndex.isValid()){
        return;
    }
    QModelIndex partUnitIndex = _partsModel->index(_currentIndex.row(), PartsSqlTableModel::ColumnPartUnit);
    QString partUnit = partUnitIndex.data(Qt::EditRole).toString();

    QModelIndex availableStockColIndex = _partStockModel->index(stockIndex.row(), PartStockTableModel2::ColumnQuantity);
    double availableStock = availableStockColIndex.data(Qt::EditRole).toDouble();

    RemoveStockDialog dlg(this);
    dlg.setPartUnit(partUnit);
    dlg.setAvailableStock(availableStock);
    if(dlg.exec()==QDialog::Accepted && dlg.getStockChange() > 0) {
        double stockChange = dlg.getStockChange() * -1;
        double remaining = availableStock + stockChange;

        _partsModel->database().transaction();
        if(remaining > 0){
            _partStockModel->setData(availableStockColIndex, remaining);
        }
        else {
            _partStockModel->removeRow(stockIndex.row());
        }

        _partStockLogModel->appendRow(stockChange, QVariant(), dlg.getComment());

        QVariant totalStock = _partStockModel->computeCurrentStock();
        QModelIndex actualStockIndex = _partsModel->index(_currentIndex.row(), PartsSqlTableModel::ColumnActualStock);
        _partsModel->setData(actualStockIndex, totalStock);

        if(_partStockModel->submitAll() && _partStockLogModel->submitAll() && _partsModel->submitAll()){
            _partsModel->database().commit();
        }
        else{
            _partsModel->database().rollback();
            //TODO: Display some warning
            qWarning("Failed to insert stock change (removal) in database");
        }
    }
}

void PartDetailsView::onEditPart()
{
    emit editPartSelected();
}

void PartDetailsView::setCurrentIndex(const QModelIndex &current)
{
    _currentIndex = current;
    _widgetMapper->setCurrentModelIndex(current);
    this->setEnabled(current.isValid());
    updateStockView(current);
}

void PartDetailsView::updateStockView(const QModelIndex & current)
{   
    QModelIndex primaryKeyIndex = _partsModel->index(current.row(), PartsSqlTableModel::ColumnId);
    QVariant keyValue = primaryKeyIndex.data(Qt::EditRole);
    qDebug()<<"Changing part to "<<keyValue;
    _partStockLogModel->setCurrentPartId(keyValue);
    _partStockLogModel->select();
    _partStockModel->setCurrentPartId(keyValue);
    _partStockModel->select();
}


