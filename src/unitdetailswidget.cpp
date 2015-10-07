#include "unitdetailswidget.h"
#include <qstyleditemdelegate.h>
#include "widgets/checkedheader.h"
#include "models/unittablemodel.h"
#include <QDebug>
#include <QTableView>
#include <QStandardItemModel>
#include <QLineEdit>
#include <QFormLayout>
#include <QGroupBox>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QDataWidgetMapper>
#include <QSqlError>
#include <QMessageBox>
#include <QEvent>
#include <QKeyEvent>

UnitDetailsDelegate::UnitDetailsDelegate(QObject * parent)
    : QStyledItemDelegate(parent)
{
}

void UnitDetailsDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
   QTableView* view = dynamic_cast<QTableView*>(editor);
   if(view){
       QVariantList selectedItems;
       QVariant data = index.data(Qt::EditRole);
       if(data.isValid() && data.canConvert(QVariant::List)){
           selectedItems = data.toList();
       }
#ifdef QT_DEBUG
       qDebug()<<"selectedItems contains "<<selectedItems.size();
#endif
       QStandardItemModel * itemModel = static_cast<QStandardItemModel*>(view->model());
       int rowCount = itemModel->rowCount();
       for(int row=0; row<rowCount; ++row){
           QStandardItem * item = itemModel->item(row, 0);
           Qt::CheckState itemState = selectedItems.contains(item->data()) ? Qt::Checked : Qt::Unchecked ;
           item->setCheckState(itemState);
       }
       return;
   }
   QStyledItemDelegate::setEditorData(editor, index);
}

void UnitDetailsDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QTableView* view = dynamic_cast<QTableView*>(editor);
    if(view){
        QVariantList selectedItems;
        QStandardItemModel * itemModel = static_cast<QStandardItemModel*>(view->model());
        int rowCount = itemModel->rowCount();
        for(int row=0; row<rowCount; ++row){
            QStandardItem * item = itemModel->item(row, 0);
            if(item->checkState()==Qt::Checked){
                selectedItems.append(item->data());
            }
        }
        model->setData(index, selectedItems);
        return;
    }
    QStyledItemDelegate::setModelData(editor, model, index);
}

UnitDetailsWidget::UnitDetailsWidget(QWidget *parent) : QWidget(parent),
     _dirty(false), _newRecord(false)
{
    //Info groupbox START
    _nameLineEdit = new QLineEdit;
    _symbolLineEdit = new QLineEdit;
    QFormLayout * layout1 = new QFormLayout;
    layout1->addRow(tr("Name:"), _nameLineEdit);
    layout1->addRow(tr("Symbol:"), _symbolLineEdit);
    //Info groupbox END

    //Prefixes groupbox START
    _allowedPrefixesTable = new QTableView;        
    _allowedPrefixesTable->setCornerButtonEnabled(false);
    _allowedPrefixesTable->setSelectionMode(QAbstractItemView::SingleSelection);
    _allowedPrefixesTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    _allowedPrefixesTable->verticalHeader()->setVisible(false);
    CheckedHeader * horizontalHeader = new CheckedHeader(Qt::Horizontal);
    _allowedPrefixesTable->setHorizontalHeader(horizontalHeader);
    connect(horizontalHeader,SIGNAL(toggled(bool)), this, SLOT(slotToggleCheckboxes(bool)));

    QHBoxLayout * hboxLayout2 = new QHBoxLayout;
    hboxLayout2->addWidget(_allowedPrefixesTable);

    QGroupBox * groupBox3= new QGroupBox(tr("Allowed prefixes"));
    groupBox3->setLayout(hboxLayout2);

    //Prefixes groupbox END

    _buttonBox = new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Discard);
    connect(_buttonBox, SIGNAL(accepted()), this, SLOT(slotAccept()));
    //connect(_buttonBox, SIGNAL(rejected()), this, SLOT(slotReject()));
    QPushButton* btnNoKey = _buttonBox->button(QDialogButtonBox::Discard);
    connect(btnNoKey, SIGNAL(clicked()), SLOT(slotReject()));

    QVBoxLayout * mainLayout = new QVBoxLayout;
    mainLayout->addLayout(layout1);
    mainLayout->addWidget(groupBox3,1);
    mainLayout->addWidget(_buttonBox);
    setLayout(mainLayout);

    _mapper = new QDataWidgetMapper(this);
    _mapper->setItemDelegate(new UnitDetailsDelegate(this));
    connect(_nameLineEdit, SIGNAL(textChanged(QString)), this, SLOT(slotSetDirty()));
    connect(_symbolLineEdit, SIGNAL(textChanged(QString)), this, SLOT(slotSetDirty()));        

    initPrefixesTableModel();

    _allowedPrefixesTable->resizeColumnsToContents();
    //this needs to be done after the model is set
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    _allowedPrefixesTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
#else
    _allowedPrefixesTable->horizontalHeader()->setResizeMode(0,QHeaderView::Fixed);
#endif
    connect(_allowedPrefixesTableModel,SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(slotSetDirty()));    
    _allowedPrefixesTable->installEventFilter(this);
}

UnitDetailsWidget::~UnitDetailsWidget()
{
}

void UnitDetailsWidget::initPrefixesTableModel(){
    _allowedPrefixesTableModel = new QStandardItemModel(0,4,this);
    QStringList labels;
    labels<<""<<tr("Prefix")<<tr("Symbol")<<tr("Power");
    _allowedPrefixesTableModel->setHorizontalHeaderLabels(labels);

    QSqlQuery sqlQuery("SELECT id, prefix, symbol, power FROM si_prefix");

    if(sqlQuery.exec()){
        while(sqlQuery.next()){
            QList<QStandardItem*> items;
            QStandardItem * checkItem = new QStandardItem(false);
            checkItem->setCheckable(true);
            checkItem->setCheckState(Qt::Unchecked);
            checkItem->setData(sqlQuery.value(0));
            checkItem->setEditable(false);
            items.append(checkItem);

            QStandardItem * prefixItem = new QStandardItem(sqlQuery.value(1).toString());
            prefixItem->setEditable(false);
            items.append(prefixItem);

            QStandardItem * symbolItem = new QStandardItem(sqlQuery.value(2).toString());
            symbolItem->setEditable(false);
            items.append(symbolItem);

            int power = sqlQuery.value(3).toInt();
            QStandardItem * powerItem = new QStandardItem(tr("10^%0").arg(power));
            powerItem->setEditable(false);
            items.append(powerItem);
            _allowedPrefixesTableModel->appendRow(items);
        }
    }
    else{
        //TODO:Show error message
        qDebug()<<"Failed to execute query. Error : "<<sqlQuery.lastError();
    }
    _allowedPrefixesTable->setModel(_allowedPrefixesTableModel);
}

QModelIndex UnitDetailsWidget::currentModelIndex() const
{
    return _currentIndex;
}

void UnitDetailsWidget::setCurrentModelIndex(const QModelIndex & modelIndex, bool newRecord)
{
    qDebug()<<"Current index is "<<modelIndex<<" and is "<<modelIndex.isValid();
    _currentIndex = modelIndex;    
    _newRecord = newRecord;
    _mapper->setCurrentModelIndex(modelIndex);
    setEnabled(_currentIndex.isValid());    
    if(newRecord){
        _nameLineEdit->setFocus();
        slotSetDirty();
    }
    else{
        slotResetDirty();
    }
}

void UnitDetailsWidget::setModel(QAbstractItemModel *model)
{
    if(_model==model)
        return;
    _model = model;
    _mapper->setModel(_model);
    _mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
    _mapper->addMapping(_nameLineEdit, UnitTableModel::ColumnName);
    _mapper->addMapping(_symbolLineEdit, UnitTableModel::ColumnSymbol);
    _mapper->addMapping(_allowedPrefixesTable, UnitTableModel::ColumnPrefixes);
}

void UnitDetailsWidget::slotAccept()
{    
    if(_nameLineEdit->text().isEmpty()){
        QMessageBox::warning(this, tr("Required field"), tr("The unit must have a name"), QMessageBox::Close);
        return;
    }
    if(_symbolLineEdit->text().isEmpty()){
        QMessageBox::warning(this, tr("Required field"), tr("The unit must have a symbol"), QMessageBox::Close);
        return;
    }

    qDebug()<<"Saving changes to unit with model index" << _currentIndex ;    
    if(!_mapper->submit()){
        qWarning()<<"Failed to submit changes ";
        //TODO:SHow error message
    }
    slotResetDirty();
    emit accepted();
}

void UnitDetailsWidget::slotReject()
{    
    qDebug()<<"Reject changes to unit with model index" << _currentIndex ;    
    if(!isNew()){
        _mapper->revert();
    }
    slotResetDirty();
    emit rejected();

}

void UnitDetailsWidget::slotSetDirty()
{
   _dirty = true;
   _buttonBox->setEnabled(true);
}

void UnitDetailsWidget::slotResetDirty()
{
   _dirty = false;
   _buttonBox->setEnabled(false);
}

void UnitDetailsWidget::slotToggleCheckboxes(bool checked)
{
    int rowCount = _allowedPrefixesTableModel->rowCount();
    Qt::CheckState checkState = checked ? Qt::Checked : Qt::Unchecked;
    for(int row=0; row<rowCount;++row){
        _allowedPrefixesTableModel->item(row)->setCheckState(checkState);
    }
    slotSetDirty();
}

bool UnitDetailsWidget::eventFilter(QObject *obj, QEvent *event)
{
     if (event->type() == QEvent::KeyPress) {
         QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
         if(keyEvent->key()==Qt::Key_Space){
             togglePrefix();
             return true;
         }
     }
     return QObject::eventFilter(obj, event);
}

void UnitDetailsWidget::togglePrefix()
{
    QModelIndex current = _allowedPrefixesTable->currentIndex();
    if(!current.isValid()) return;
    QStandardItem * item = _allowedPrefixesTableModel->item(current.row(), 0);
    if(item){
        Qt::CheckState state = item->checkState();
        state = state==Qt::Checked ? Qt::Unchecked :Qt::Checked;
        item->setCheckState(state);
    }
}




