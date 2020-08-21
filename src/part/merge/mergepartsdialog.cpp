#include "mergepartsdialog.h"
#include <QDialogButtonBox>
#include <QPushButton>
#include <QGroupBox>
#include <QBoxLayout>
#include <QRadioButton>
#include <QTableWidget>
#include <QHeaderView>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QSettings>
#include <QPainter>

#include "part/merge/mergehelper.h"

enum UserItemType {
    FieldName = QTableWidgetItem::UserType ,
    Exclusive = QTableWidgetItem::UserType + 1
};

enum Column {
    PartAColumn = 1,
    PartBColumn = 2
};

static QTableWidgetItem * createFieldNameItem(const QString & fieldName);
static QTableWidgetItem * createFieldValueItem(const MergeHelper::PartField field, const QString & value, bool selected=false);
static QTableWidgetItem * createFieldValueItem(const MergeHelper::PartField field, const QVariant & value, bool selected=false);
static QTableWidgetItem * createEmptyValueItem(const MergeHelper::PartField field);

MergePartsDialog::MergePartsDialog(QWidget *parent) : QDialog(parent), _mergeHelper(new MergeHelper)
{   

    QPushButton * partA_OnlyBtn = new QPushButton(tr("A only"));
    QPushButton * partB_OnlyBtn = new QPushButton(tr("B only"));

    connect(partA_OnlyBtn, SIGNAL(clicked(bool)), this, SLOT(slotPartAOnlyClicked()));
    connect(partB_OnlyBtn, SIGNAL(clicked(bool)), this, SLOT(slotPartBOnlyClicked()));

    QHBoxLayout * topActionsLayout = new QHBoxLayout;
    topActionsLayout->addWidget(partA_OnlyBtn);
    topActionsLayout->addWidget(partB_OnlyBtn);
    topActionsLayout->addStretch(1);

    _mergeTable = new QTableWidget(12,3);
    _mergeTable->verticalHeader()->setVisible(false);    

    QStringList tableHeader;
    tableHeader << "" << tr("Part A") << tr("Part B");
    _mergeTable->setHorizontalHeaderLabels(tableHeader);
    _mergeTable->setItem(0, 0, createFieldNameItem(tr("Name")));
    _mergeTable->setItem(1, 0, createFieldNameItem(tr("Description")));
    _mergeTable->setItem(2, 0, createFieldNameItem(tr("Minimum stock")));
    _mergeTable->setItem(3, 0, createFieldNameItem(tr("Comment")));
    _mergeTable->setItem(4, 0, createFieldNameItem(tr("Custom part#")));
    _mergeTable->setItem(5, 0, createFieldNameItem(tr("Unit")));
    _mergeTable->setItem(6, 0, createFieldNameItem(tr("Category")));
    _mergeTable->setItem(7, 0, createFieldNameItem(tr("Package")));
    _mergeTable->setItem(8, 0, createFieldNameItem(tr("Parameters")));
    _mergeTable->setItem(9, 0, createFieldNameItem(tr("Distributors")));
    _mergeTable->setItem(10, 0, createFieldNameItem(tr("Manufacturers")));
    _mergeTable->setItem(11, 0, createFieldNameItem(tr("Attachments")));
    _mergeTable->setAlternatingRowColors(true);


    QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    buttonBox->button(QDialogButtonBox::Ok)->setDefault(true);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    QVBoxLayout * mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(topActionsLayout);
    mainLayout->addWidget(_mergeTable, 1);
    mainLayout->addWidget(buttonBox);

    _mergeTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    connect(_mergeTable, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(slotItemChanged(QTableWidgetItem*)));    
    readSettings();
}

MergePartsDialog::~MergePartsDialog()
{
    delete _mergeHelper;
}

void MergePartsDialog::closeEvent(QCloseEvent *)
{
     writeSettings();
}

void MergePartsDialog::writeSettings()
{
    QSettings settings;
    settings.beginGroup("MergePartsDialog");
    settings.setValue("size", this->size());
    settings.endGroup();

}

void MergePartsDialog::readSettings()
{
    QSettings settings;
    settings.beginGroup("MergePartsDialog");
    resize(settings.value("size", QSize(800, 460)).toSize());
    settings.endGroup();
}

void MergePartsDialog::accept()
{
    //Keeps the type of merge to perform for each mergeable field
    QList<QPair<MergeHelper::PartField,MergeHelper::FieldMergeType> > list;

    //Collects the merge decisions for each field into our list
    for(int row = 0; row < _mergeTable->rowCount(); ++row){
       QTableWidgetItem * a = _mergeTable->item(row, PartAColumn);
       QTableWidgetItem * b = _mergeTable->item(row, PartBColumn);
       MergeHelper::FieldMergeType mergeType = MergeHelper::None;
       if(a->checkState() == Qt::Checked && b->checkState() == Qt::Checked){
           mergeType = MergeHelper::Both;
       }
       else if( a->checkState() == Qt::Checked){
           mergeType = MergeHelper::UseA;
       }
       else if( b->checkState() == Qt::Checked){
           mergeType = MergeHelper::UseB;
       }
       QPair<MergeHelper::PartField,MergeHelper::FieldMergeType> pair((MergeHelper::PartField)a->data(FieldName).toInt() , mergeType);
       list.append(pair);
    }

#ifdef QT_DEBUG
    for(int i=0; i< list.size(); ++i){
        const QPair<MergeHelper::PartField,MergeHelper::FieldMergeType> item = list.at(i);
        qDebug()<<item.first << " "<< item.second;
    }
#endif

    bool success = _mergeHelper->mergeParts(_partA, _partB, list);
    if(success){
        qDebug() << "Part "<<_partB << " merged into "<<_partA;
    }
    else{
        qWarning("Merged failed");
    }
    QDialog::accept();    
}

void MergePartsDialog::reject(){
    QDialog::reject();
}

void MergePartsDialog::slotPartAOnlyClicked()
{

    toggleSelectedItems(Column::PartAColumn, Column::PartBColumn);

}

void MergePartsDialog::slotPartBOnlyClicked()
{    
    toggleSelectedItems(Column::PartBColumn, Column::PartAColumn);

}

void MergePartsDialog::toggleSelectedItems(int toCheckCol, int toUncheckCol )
{
    _mergeTable->blockSignals(true);
    for(int row = 0; row < _mergeTable->rowCount(); ++row){
       QTableWidgetItem * check = _mergeTable->item(row, toCheckCol);
       QTableWidgetItem * uncheck = _mergeTable->item(row, toUncheckCol);
       if(check->flags().testFlag(Qt::ItemIsUserCheckable)){
        check->setCheckState(Qt::Checked);
       }
       if(uncheck->flags().testFlag(Qt::ItemIsUserCheckable)){
            uncheck->setCheckState(Qt::Unchecked);
       }
    }
    _mergeTable->blockSignals(false);
}

void MergePartsDialog::slotItemChanged(QTableWidgetItem *item)
{
    // Ensures only one value is selected for an exclusive part field
   qDebug()<<"Item changed "<< item->row()<< " "<< item->checkState();
   if(item->checkState() == Qt::Checked && item->data(Exclusive).toBool()){
       int col = item->column() == PartAColumn ? PartBColumn : PartAColumn;
       QTableWidgetItem * other = _mergeTable->item(item->row(), col);
       other->setCheckState(Qt::Unchecked);
   }
}

void MergePartsDialog::setParts(const QVariant & partA, const QVariant & partB)
{
    _partA = partA;
    _partB = partB;
    loadParts();
}

void MergePartsDialog::selectSmartMerge()
{

}

// Produces a column of items for a part
QList<QTableWidgetItem *> MergePartsDialog::loadPart(QVariant partId)
{    
    QList<QTableWidgetItem *> list;
    // Retrieve the fields values that can be merged.
    QSharedPointer<PartDetails> ptr = _mergeHelper->loadPart(partId);

    if(ptr.isNull()){
        qWarning() << "Load part failed (partId="<<partId<<")";
        return list;
    }

    // Create table items for fixed fields
    QTableWidgetItem * nameItem = createFieldValueItem(MergeHelper::PartName, ptr.data()->name);
    list.append(nameItem);

    QTableWidgetItem * descriptionItem = createFieldValueItem(MergeHelper::PartDescription, ptr.data()->description);
    list.append(descriptionItem);

    QTableWidgetItem * minimumStockItem = createFieldValueItem(MergeHelper::PartMinimumStock, ptr.data()->minimumStock);
    list.append(minimumStockItem);

    QTableWidgetItem * commentItem = createFieldValueItem(MergeHelper::PartComment, ptr.data()->comment);
    list.append(commentItem);

    QTableWidgetItem * customPartNumberItem = createFieldValueItem(MergeHelper::PartCustomPartNumber, ptr.data()->customPartNumber);
    list.append(customPartNumberItem);

    QTableWidgetItem * unitNameItem = createFieldValueItem(MergeHelper::PartUnit, ptr.data()->partUnitName);
    list.append(unitNameItem);

    QTableWidgetItem * catNameItem = createFieldValueItem(MergeHelper::PartCategory, ptr.data()->categoryName);
    list.append(catNameItem);

    QTableWidgetItem * pkgNameItem = createFieldValueItem(MergeHelper::PartPackage, ptr.data()->packageName);
    list.append(pkgNameItem);

    // create table items for variable fields like part parameters or manufacturers

    const int paramCount = ptr.data()->parametersCount;
    if(paramCount>0){
        QTableWidgetItem * paramsItem = createFieldValueItem(MergeHelper::PartParameters, tr("%n parameter(s)", 0, paramCount), true);
        paramsItem->setData(Exclusive, QVariant(false));
        list.append(paramsItem);
    }
    else{
        list.append(createEmptyValueItem(MergeHelper::PartParameters));
    }

    const int distributorCount = ptr.data()->distributorsCount;
    if(distributorCount > 0){
        QTableWidgetItem * distributorItem = createFieldValueItem(MergeHelper::PartDistributors, tr("%n distributor(s)", 0, distributorCount), true);
        distributorItem->setData(Exclusive, QVariant(false));
        list.append(distributorItem);
    }
    else{
        list.append(createEmptyValueItem(MergeHelper::PartDistributors));
    }

    const int manufacturerCount = ptr.data()->manufacturersCount;
    if(manufacturerCount>0){
        QTableWidgetItem * manufacturerItem = createFieldValueItem(MergeHelper::PartManufacturers, tr("%n manufacturer(s)", 0, manufacturerCount), true);
        manufacturerItem->setData(Exclusive, QVariant(false));
        list.append(manufacturerItem);
    }
    else{
        list.append(createEmptyValueItem(MergeHelper::PartManufacturers));
    }

    const int attachmentCount = ptr.data()->attachmentsCount;
    if(attachmentCount > 0){
        QTableWidgetItem * attachmentItem = createFieldValueItem(MergeHelper::PartAttachments, tr("%n attachment(s)", 0, attachmentCount), true);
        attachmentItem->setData(Exclusive, QVariant(false));
        list.append(attachmentItem);
    }
    else{
        list.append(createEmptyValueItem(MergeHelper::PartAttachments));
    }

    return list;
}

void MergePartsDialog::loadParts()
{
    QList<QTableWidgetItem*> partAItems = loadPart(_partA);
    QList<QTableWidgetItem*> partBItems = loadPart(_partB);

    _mergeTable->blockSignals(true);
    //By default select A for the part name
    partAItems.at(0)->setCheckState(Qt::Checked);    
    for(int row = 0; row <partAItems.size(); ++row){

        QTableWidgetItem * a = partAItems.at(row);
        QTableWidgetItem * b = partBItems.at(row);        
        //Use the non empty one
        if(!a->text().isEmpty()){
            a->setCheckState(Qt::Checked);
        }
        else if(!b->text().isEmpty()){
            b->setCheckState(Qt::Checked);
        }

        _mergeTable->setItem(row, PartAColumn, a);
        _mergeTable->setItem(row, PartBColumn, b);        
    }
    _mergeTable->blockSignals(false);
}

static QTableWidgetItem * createFieldNameItem(const QString & fieldName)
{
    QTableWidgetItem * item = new QTableWidgetItem(fieldName);
    item->setFlags(Qt::ItemIsEnabled);
    return item;
}

static QTableWidgetItem * createFieldValueItem(const MergeHelper::PartField field, const QVariant & value, bool selected)
{
    return createFieldValueItem(field, value.toString(), selected);
}

static QTableWidgetItem * createFieldValueItem(const MergeHelper::PartField field, const QString & value, bool selected)
{
    QTableWidgetItem * item = new QTableWidgetItem(value);
    item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
    const Qt::CheckState checkState = selected ? Qt::Checked : Qt::Unchecked;
    item->setCheckState(checkState);
    item->setData(FieldName, QVariant(field));
    item->setData(Exclusive, QVariant(true));
    return item;
}

static QTableWidgetItem * createEmptyValueItem(const MergeHelper::PartField field)
{
    QTableWidgetItem * item = new QTableWidgetItem();
    item->setFlags(Qt::NoItemFlags);
    item->setData(FieldName, QVariant(field));
    item->setData(Exclusive, QVariant(true));
    return item;
}
