#include "partdetailsview.h"
#include "ui_partdetailsview.h"
#include "parts/partssqlquerymodel2.h"
#include <QDebug>
#include "parts/stockhistorymodel.h"
#include "widgets/stockactioncolumndelegate.h"
#include <QPushButton>

PartDetailsView::PartDetailsView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PartDetailsView)
{
    ui->setupUi(this);

    QPushButton * addStockButton = new QPushButton(QIcon(QString::fromUtf8(":/icons/addStock")), tr("Add Stock"),ui->partActionsButtonBox);
    QPushButton * removeStockButton = new QPushButton(QIcon(QString::fromUtf8(":/icons/removeStock")), tr("Remove Stock"),ui->partActionsButtonBox);
    QPushButton * editPartButton = new QPushButton(QIcon(QString::fromUtf8(":/icons/editPart")), tr("Edit Part"),ui->partActionsButtonBox);
    ui->partActionsButtonBox->addButton(addStockButton, QDialogButtonBox::ActionRole);
    ui->partActionsButtonBox->addButton(removeStockButton, QDialogButtonBox::ActionRole);
    ui->partActionsButtonBox->addButton(editPartButton, QDialogButtonBox::ActionRole);
    connect(addStockButton, SIGNAL(clicked()), this, SLOT(onAddStock()));
    connect(removeStockButton, SIGNAL(clicked()), this, SLOT(onRemoveStock()));
    connect(editPartButton, SIGNAL(clicked()), this, SLOT(onEditPart()));
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

void PartDetailsView::setCurrentIndex(const QModelIndex &current)
{
    if(_partsModel==0) return;
    ui->partActionsButtonBox->setEnabled(current.isValid());
    updatePartDetailView(current);
    updateStockView(current);
}

void PartDetailsView::updatePartDetailView(const QModelIndex & current)
{
    ui->partNameLabel->setText(columnDisplayData(_partsModel, current, PartsSqlQueryModel2::ColumnName));
    ui->partDescriptionLabel->setText(columnDisplayData(_partsModel, current, PartsSqlQueryModel2::ColumnDescription));
    ui->partCategoryLabel->setText(columnDisplayData(_partsModel, current, PartsSqlQueryModel2::ColumnCategoryName));
    ui->partStockLevelLabel->setText(columnDisplayData(_partsModel, current, PartsSqlQueryModel2::ColumnActualStock));
    ui->partMinStockLabel->setText(columnDisplayData(_partsModel, current, PartsSqlQueryModel2::ColumnMinStock));
    ui->partCustomNumberLabel->setText(columnDisplayData(_partsModel, current, PartsSqlQueryModel2::ColumnCustomPartNumber));
    ui->partCommentLabel->setText(columnDisplayData(_partsModel, current, PartsSqlQueryModel2::ColumnComment));
    ui->partCreateDateLabel->setText(columnDisplayDate(_partsModel, current, PartsSqlQueryModel2::ColumnCreateDate));
    ui->partStorageLocationLabel->setText(columnDisplayData(_partsModel, current, PartsSqlQueryModel2::ColumnStorage));
    ui->partConditionLabel->setText(columnDisplayData(_partsModel, current, PartsSqlQueryModel2::ColumnCondition));
    ui->partStockLevelUnitLabel->setText(columnDisplayData(_partsModel, current, PartsSqlQueryModel2::ColumnPartUnit));
    ui->partMinStockUnitLabel->setText(columnDisplayData(_partsModel, current, PartsSqlQueryModel2::ColumnPartUnit));
}

void PartDetailsView::updateStockView(const QModelIndex & current)
{
    if(!current.isValid()){
        ui->partStockHistoryTable->setModel(0);
        return;
    }

    QModelIndex primaryKeyIndex = _partsModel->index(current.row(), PartsSqlQueryModel2::ColumnId);
    QVariant keyValue = primaryKeyIndex.data(Qt::EditRole);
    qDebug()<<"Changing part to "<<keyValue;

    StockHistoryModel * model= new StockHistoryModel();
    model->setSelectedPart(keyValue);
    model->setHeaderData(0, Qt::Horizontal, QString());
    model->setHeaderData(1, Qt::Horizontal, tr("Date"));
    model->setHeaderData(2, Qt::Horizontal, tr("Amount"));
    model->setHeaderData(3, Qt::Horizontal, tr("Price"));
    model->setHeaderData(4, Qt::Horizontal, tr("Comment"));
    ui->partStockHistoryTable->setItemDelegateForColumn(StockHistoryModel::ColumnAction, new StockActionColumnDelegate());
    ui->partStockHistoryTable->setModel(model);
    ui->partStockHistoryTable->resizeColumnsToContents();
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    ui->partStockHistoryTable->horizontalHeader()->setSectionResizeMode(StockHistoryModel::ColumnAction, QHeaderView::Fixed);
#else
    ui->partStockHistoryTable->horizontalHeader()->setResizeMode(StockHistoryModel::ColumnAction, QHeaderView::Fixed);
#endif
}

QString PartDetailsView::columnDisplayData(QAbstractItemModel * model, const QModelIndex & current, int column){
    if(!current.isValid())
        return QString();
    QModelIndex colIndex = model->index(current.row(), column);
    return colIndex.isValid()? colIndex.data().toString() : QString();
}

QString PartDetailsView::columnDisplayDate(QAbstractItemModel * model, const QModelIndex & current, int column){
    if(!current.isValid())
        return QString();
    QModelIndex colIndex = model->index(current.row(), column);
    if(!colIndex.isValid())
        return QString();
    return QDateTime::fromTime_t(colIndex.data(Qt::EditRole).toUInt()).toString(Qt::DefaultLocaleShortDate);
}


