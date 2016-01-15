#include "partdetailsview.h"
#include "ui_partdetailsview.h"
#include "models/partssqltablemodel.h"
#include "widgets/currencydelegate.h"
#include "widgets/datetimedelegate.h"
#include "models/partstocktablemodel.h"
#include "models/stocktableformatproxymodel.h"
#include "dialogs/addstockdialog.h"
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
    _partsModel(0),
    _partStockModel(0)
{
    ui->setupUi(this);
    ui->partStockHistoryTable->setWordWrap(false);
    _widgetMapper = new QDataWidgetMapper(this);
    _widgetMapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
    _widgetMapper->setItemDelegate(new PartDetailsDelegate(this));

    _partStockModel = PartStockTableModel::createNew(this);

    StockTableFormatProxyModel * proxyModel = new StockTableFormatProxyModel(this);
    proxyModel->setSourceModel(_partStockModel);

    ui->partStockHistoryTable->setItemDelegateForColumn(PartStockTableModel::ColumnDateTime, new DateDelegate(this));
    ui->partStockHistoryTable->setItemDelegateForColumn(PartStockTableModel::ColumnPrice, new CurrencyDelegate(this));
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

    _partStockModel->setCurrentPartId(QVariant());
    _partStockModel->select();
}

PartDetailsView::~PartDetailsView()
{
    delete ui;
}

void PartDetailsView::onAddStock()
{    
    QVariant partUnit = _partsModel->index(_currentIndex.row(), PartsSqlTableModel::ColumnPartUnit).data();
    AddStockDialog dlg(this);
    dlg.setPartUnit(partUnit.toString());
    if(dlg.exec()!=QDialog::Accepted)
        return;
    _partsModel->database().transaction();
    _partStockModel->appendRow(dlg.getStockChange(), dlg.getPartPrice(), dlg.getComment());
    _partsModel->updatePartStock(_currentIndex, dlg.getStockChange());
    _partsModel->updatePartAvgPrice(_currentIndex, dlg.getPartPrice());

    if(_partStockModel->submitAll() && _partsModel->submitAll()){
        _partsModel->database().commit();
    }
    else{
        _partsModel->database().rollback();
        //TODO: Display some warning
        qWarning("Failed to insert stock change (addition) in database");
    }   
}

void PartDetailsView::onRemoveStock()
{
    QVariant partUnit = _partsModel->index(_currentIndex.row(), PartsSqlTableModel::ColumnPartUnit).data();
    QVariant availableStock = _partsModel->index(_currentIndex.row(), PartsSqlTableModel::ColumnActualStock).data();
    RemoveStockDialog dlg(this);
    dlg.setPartUnit(partUnit.toString());
    dlg.setAvailableStock(availableStock.toInt());
    if(dlg.exec()!=QDialog::Accepted)
        return;
    _partsModel->database().transaction();
    _partStockModel->appendRow(dlg.getStockChange(), QVariant(), dlg.getComment());
    _partsModel->updatePartStock(_currentIndex, dlg.getStockChange());
    if(_partStockModel->submitAll() && _partsModel->submitAll()){
        _partsModel->database().commit();
    }
    else{
        _partsModel->database().rollback();
        //TODO: Display some warning
        qWarning("Failed to insert stock change (removal) in database");
    }
}

void PartDetailsView::onEditPart()
{
    emit editPartSelected();
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
    _widgetMapper->addMapping(ui->partStorageLocationLabel, PartsSqlTableModel::ColumnStorage);
    _widgetMapper->addMapping(ui->partConditionLabel, PartsSqlTableModel::ColumnCondition);
    _widgetMapper->addMapping(ui->partPackageLabel, PartsSqlTableModel::ColumnPackageName);
    setCurrentIndex(QModelIndex());        
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
    _partStockModel->setCurrentPartId(keyValue);
    _partStockModel->select();    
}


