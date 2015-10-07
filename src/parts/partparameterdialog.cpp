#include "partparameterdialog.h"
#include "ui_partparameterdialog.h"
#include <QLineEdit>
#include <widgets/parametervaluevalidator.h>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include "models/unittablemodel.h"
#include "models/partparametertablemodel.h"
#include "widgets/parametervaluedelegate.h"

PartParameterDialog::PartParameterDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PartParameterDialog)
{    
    ui->setupUi(this);    
    QVector<QChar> siPrefixes(18);
    QSqlQuery listSiPrefixesQuery("SELECT symbol FROM si_prefix WHERE length(symbol)>0");
    if(listSiPrefixesQuery.exec()){
        while(listSiPrefixesQuery.next()){
           QChar c = listSiPrefixesQuery.value(0).toString().at(0);
           siPrefixes.append(c);
        }
    }
    connect(ui->comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(valueCurrentIndexChanged(int)));
    //ui->tableWidget->setItemDelegateForColumn(1, new ValueDelegate());
    UnitTableModel * unitsModel = new UnitTableModel(this);
    unitsModel->select();
    ui->tableWidget->setItemDelegateForColumn(1, new ParameterValueDelegate());
    ui->tableWidget->setItemDelegateForColumn(2, new ParameterUnitDelegate(unitsModel));
    ui->lineEdit->setValidator(new ParameterValueValidator());
}

PartParameterDialog::~PartParameterDialog()
{
    delete ui;
}

void PartParameterDialog::valueCurrentIndexChanged(int index)
{
    QString text = ui->comboBox->itemText(index);
    ui->comboBox->setEditText("11"+text);

    //ui->comboBox->lineEdit()->setText("11"+text);
}


ValueDelegate::ValueDelegate(QObject * parent) : QStyledItemDelegate(parent)
{
}

void ValueDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QComboBox* view = dynamic_cast<QComboBox*>(editor);
    if(view){
        QVariant data = index.data(Qt::EditRole);
       view->setEditText(data.toString());
       return;
    }
    QStyledItemDelegate::setEditorData(editor, index);
}

void ValueDelegate::setModelData(QWidget *, QAbstractItemModel *, const QModelIndex &) const
{
}

QWidget * ValueDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &, const QModelIndex &) const
{
    QComboBox * combo = new QComboBox(parent);
    combo->setEditable(true);
    combo->addItem("M");
    combo->addItem("k");
    combo->addItem("");
    combo->addItem("m");
    combo->setInsertPolicy(QComboBox::NoInsert);
    connect(combo, SIGNAL(currentIndexChanged(QString)), this, SLOT(valueCurrentIndexChanged(QString)));
    return combo;
}

void ValueDelegate::valueCurrentIndexChanged(QString text)
{
    QComboBox* editor = qobject_cast<QComboBox*>(sender());
    //QString text = editor->itemText(index);
    editor->setEditText("11"+text);
}

ParameterUnitDelegate::ParameterUnitDelegate(QSqlQueryModel *unitsModel, QObject * parent) :
    QStyledItemDelegate(parent),
    _unitsModel(unitsModel)
{
}

void ParameterUnitDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {
    QComboBox* combo = qobject_cast<QComboBox*>(editor);
    QVariant fkey = index.data(Qt::EditRole);

    int comboIdx = -1;
    //QModelIndex fkeyIdx = mainModel->index(index.row(), column);
    //QVariant fkey = fkeyIdx.data(Qt::EditRole);
    if(!fkey.isNull() ){
        QModelIndex start = combo->model()->index(0,UnitTableModel::ColumnId);
        QModelIndexList res = combo->model()->match(start, Qt::EditRole, fkey);
        if(!res.isEmpty()){
            comboIdx = res.first().row();
        }
    }
    combo->setCurrentIndex(comboIdx);
}

void ParameterUnitDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
     QComboBox* combo = qobject_cast<QComboBox*>(editor);
     int currentIndex = combo->currentIndex();
     if(currentIndex>=0){
         QVariant unitKey = combo->model()->index(currentIndex,UnitTableModel::ColumnId).data(Qt::EditRole);
         model->setData(index,unitKey);
     }

}

QWidget * ParameterUnitDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &, const QModelIndex &) const
{
    QComboBox * combo = new QComboBox(parent);
    combo->setModel(_unitsModel);
    combo->setModelColumn(UnitTableModel::ColumnName);
    return combo;
}
