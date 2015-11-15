#include "partdetailsview.h"
#include "ui_partdetailsview.h"
#include "models/partssqltablemodel.h"
#include "parts/stockhistorymodel.h"
#include "widgets/currencydelegate.h"
#include "widgets/datetimedelegate.h"
#include "models/customtablemodel.h"
#include <QDebug>
#include <QPushButton>

class StockChangeDelegate : public QStyledItemDelegate {
public:
    explicit StockChangeDelegate(QObject *parent);
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};


StockChangeDelegate::StockChangeDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{
}

void StockChangeDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItem optionCopy(option);
    int change = index.data().toInt();
    if(change<0){
        optionCopy.palette.setColor(QPalette::Text, QColor(Qt::red));
    }
    else if(change>0) {
        optionCopy.palette.setColor(QPalette::Text, QColor(Qt::darkGreen));
    }
   QStyledItemDelegate::paint(painter, optionCopy, index);
}

PartDetailsView::PartDetailsView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PartDetailsView)
{
    ui->setupUi(this);

    _partStockModel = PartStockTableModel::createNew(this);
    ui->partStockHistoryTable->setItemDelegateForColumn(PartStockTableModel::ColumnDateTime, new DateDelegate(this));
    ui->partStockHistoryTable->setItemDelegateForColumn(PartStockTableModel::ColumnPrice, new CurrencyDelegate(this));
    ui->partStockHistoryTable->setItemDelegateForColumn(PartStockTableModel::ColumnChange, new StockChangeDelegate(this));
    ui->partStockHistoryTable->setModel(_partStockModel);
    ui->partStockHistoryTable->resizeColumnsToContents();

    QPushButton * addStockButton = new QPushButton(QIcon(QString::fromUtf8(":/icons/addStock")), tr("Add Stock"),ui->partActionsButtonBox);
    QPushButton * removeStockButton = new QPushButton(QIcon(QString::fromUtf8(":/icons/removeStock")), tr("Remove Stock"),ui->partActionsButtonBox);
    QPushButton * editPartButton = new QPushButton(QIcon(QString::fromUtf8(":/icons/editPart")), tr("Edit Part"),ui->partActionsButtonBox);   

    ui->partActionsButtonBox->addButton(editPartButton, QDialogButtonBox::ActionRole);
    ui->stockActionsButtonBox->addButton(addStockButton, QDialogButtonBox::ActionRole);
    ui->stockActionsButtonBox->addButton(removeStockButton, QDialogButtonBox::DestructiveRole);
    connect(addStockButton, SIGNAL(clicked()), this, SIGNAL(addStockSelected()));
    connect(removeStockButton, SIGNAL(clicked()), this, SIGNAL(removeStockSelected()));
    connect(editPartButton, SIGNAL(clicked()), this, SIGNAL(editPartSelected()));
    //connect(addStockButton, SIGNAL(clicked()), this, SLOT(onAddStock()));
    //connect(removeStockButton, SIGNAL(clicked()), this, SLOT(onRemoveStock()));
    //connect(editPartButton, SIGNAL(clicked()), this, SLOT(onEditPart()));
}

PartDetailsView::~PartDetailsView()
{
    delete ui;
}

void PartDetailsView::onAddStock()
{
    emit addStockSelected();
}

void PartDetailsView::onRemoveStock()
{
    emit removeStockSelected();
}

void PartDetailsView::onEditPart()
{
    emit editPartSelected();
}

void PartDetailsView::setPartsModel(QAbstractItemModel * model)
{
    _partsModel = model;
    setCurrentIndex(QModelIndex());
}

void PartDetailsView::setCurrentIndex(const QModelIndex &current)
{
    if(_partsModel==0) return;
    ui->partActionsButtonBox->setEnabled(current.isValid());
    ui->stockActionsButtonBox->setEnabled(current.isValid());
    ui->partStockHistoryTable->setEnabled(current.isValid());
    updatePartDetailView(current);
    updateStockView(current);
}

void PartDetailsView::updatePartDetailView(const QModelIndex & current)
{

    ui->partNameLabel->setText(columnDisplayData(_partsModel, current, PartsSqlTableModel::ColumnName));
    ui->partDescriptionLabel->setText(columnDisplayData(_partsModel, current, PartsSqlTableModel::ColumnDescription));
    ui->partCategoryLabel->setText(columnDisplayData(_partsModel, current, PartsSqlTableModel::ColumnCategoryName));
    ui->partStockLevelLabel->setText(columnDisplayData(_partsModel, current, PartsSqlTableModel::ColumnActualStock, PartsSqlTableModel::ColumnPartUnit));
    ui->partMinStockLabel->setText(columnDisplayData(_partsModel, current, PartsSqlTableModel::ColumnMinStock, PartsSqlTableModel::ColumnPartUnit));
    ui->partCustomNumberLabel->setText(columnDisplayData(_partsModel, current, PartsSqlTableModel::ColumnCustomPartNumber));
    ui->partCommentLabel->setText(columnDisplayData(_partsModel, current, PartsSqlTableModel::ColumnComment));
    ui->partCreateDateLabel->setText(columnDisplayDate(_partsModel, current, PartsSqlTableModel::ColumnCreateDate));
    ui->partStorageLocationLabel->setText(columnDisplayData(_partsModel, current, PartsSqlTableModel::ColumnStorage));
    ui->partConditionLabel->setText(columnDisplayData(_partsModel, current, PartsSqlTableModel::ColumnCondition));
    ui->partFootprintLabel->setText(columnDisplayData(_partsModel, current, PartsSqlTableModel::ColumnFootprintName));
}

void PartDetailsView::updateStockView(const QModelIndex & current)
{   
    QModelIndex primaryKeyIndex = _partsModel->index(current.row(), PartsSqlTableModel::ColumnId);
    QVariant keyValue = primaryKeyIndex.data(Qt::EditRole);
    qDebug()<<"Changing part to "<<keyValue;
    _partStockModel->setCurrentPartId(keyValue);
    _partStockModel->select();
}

QString PartDetailsView::columnDisplayData(QAbstractItemModel * model, const QModelIndex & current, int column){
    if(!current.isValid())
        return QString();
    QModelIndex colIndex = model->index(current.row(), column);
    return colIndex.isValid()? colIndex.data().toString() : QString();
}

QString PartDetailsView::columnDisplayData(QAbstractItemModel * model, const QModelIndex & current, int column1, int column2){
    if(!current.isValid())
        return QString();
    QModelIndex col1Index = model->index(current.row(), column1);
    QModelIndex col2Index = model->index(current.row(), column2);
    if(col1Index.isValid()){
        QString data = col1Index.data().toString();
        QString postfix = col2Index.isValid() ? col2Index.data().toString() : QString();
        return QString("%1 %2").arg(data).arg(postfix);
    }
    return QString();
}

QString PartDetailsView::columnDisplayDate(QAbstractItemModel * model, const QModelIndex & current, int column){
    if(!current.isValid())
        return QString();
    QModelIndex colIndex = model->index(current.row(), column);
    if(!colIndex.isValid())
        return QString();
    return QDateTime::fromTime_t(colIndex.data(Qt::EditRole).toUInt()).toString(Qt::DefaultLocaleShortDate);
}


