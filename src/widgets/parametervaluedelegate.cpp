#include "parametervaluedelegate.h"
#include "parametervaluevalidator.h"
#include <QLineEdit>
#include "unitparser.h"
#include <QDebug>
#include <QItemEditorFactory>
#include "valuewithunit.h"
#include "parametervalue.h"

ParameterValueDelegate::ParameterValueDelegate(QObject * parent) :
    QStyledItemDelegate(parent)
{
    _validator = new ParameterValueValidator(this);
}

ParameterValueDelegate::~ParameterValueDelegate()
{
}

QString ParameterValueDelegate::displayText(const QVariant &value, const QLocale &locale) const
{
    if(value.canConvert<ValueWithUnit>()) {
        ValueWithUnit aux = value.value<ValueWithUnit>();
        return UnitParser::formatUnit(aux.value(), aux.symbol(), locale);
    }
    if(value.canConvert<ParameterValue>()){
        ParameterValue aux =  value.value<ParameterValue>();
        return UnitParser::formatUnit(aux.value, aux.symbol, locale);
    }
    return UnitParser::formatUnit(value.toDouble(), 0, locale);
}

QWidget * ParameterValueDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &, const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;
   QLineEdit *le = new QLineEdit(parent);
   le->setFrame(le->style()->styleHint(QStyle::SH_ItemView_DrawDelegateFrame, 0, le));
   le->setValidator(_validator);
   return le;
}

void ParameterValueDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QLineEdit* lineEdit = qobject_cast<QLineEdit*>(editor);
    bool ok = false;
    double value = UnitParser::parseUnit(lineEdit->text(), &ok);
    if(ok){
        bool res = model->setData(index,QVariant(value));
    }
}

void ParameterValueDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QVariant v = index.data(Qt::EditRole);
    if(v.isValid()){
        QLineEdit* lineEdit = qobject_cast<QLineEdit*>(editor);
        lineEdit->setText(UnitParser::formatUnit(v.toDouble(),0, QLocale::system()));
    }
    else{
        qWarning()<<"setEditorData invalid ";
    }
}
