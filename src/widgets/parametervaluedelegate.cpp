#include "parametervaluedelegate.h"
#include "parametervaluevalidator.h"
#include <QLineEdit>
#include "unitparser.h"
#include <QDebug>
#include <QItemEditorFactory>
#include "valuewithunit.h"
#include "parametervalue.h"
#include <QRegExpValidator>

ParameterValueDelegate::ParameterValueDelegate(QObject * parent) :
    QStyledItemDelegate(parent)
{    
}

ParameterValueDelegate::~ParameterValueDelegate()
{
}

QString ParameterValueDelegate::displayText(const QVariant &value, const QLocale &locale) const
{   
    if(value.canConvert<ParameterValue>()){
        ParameterValue aux =  value.value<ParameterValue>();
        qDebug()<<"Display locale "<<locale.name();
        return UnitParser::formatUnit(aux.value.toReal(), aux.symbol, locale);
    }    
    return QStyledItemDelegate::displayText(value, locale);
}


/*
 * ^-?\d+([kKMGTPEZYmunpfazy\x{03BC}]|\.|\,)?\d*(?!\1)[kKMGTPEZYmunpfazy\x{03BC}]?W?$
 */
QWidget * ParameterValueDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem & option, const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;
    qDebug("Creating ParameterValue editor");
    QVariant v = index.data(Qt::EditRole);
    if(v.canConvert<ParameterValue>()){
        QLocale locale;
        QChar decimalPoint = locale.decimalPoint();
        ParameterValue paramValue = v.value<ParameterValue>();
        QLineEdit *le = new QLineEdit(parent);                
        le->setFrame(le->style()->styleHint(QStyle::SH_ItemView_DrawDelegateFrame, 0, le));
        le->setProperty("unitSymbol", paramValue.symbol);
        /*
        QString exp=QString("^-?\\d+([kKMGTPEZYmunpfazy\\x{03BC}]|\\%1)?\\d*[kKMGTPEZYmunpfazy\\x{03BC}]?").arg(decimalPoint);
        if(paramValue.symbol.isEmpty()){
            exp.append('$');
        }
        else{
            exp.append(paramValue.symbol).append("?$");
        }
        QRegExp regExp(exp, Qt::CaseSensitive, QRegExp::RegExp2);
        //qDebug()<<"Regex: "<<exp<<" is valid? "<<regExp.isValid();;
        QRegExpValidator * validator = new QRegExpValidator(regExp, parent);
        le->setValidator(validator);
        */
        le->setValidator(new ParameterValueValidator(paramValue.symbol, parent));
        return le;
    }
   return QStyledItemDelegate::createEditor(parent, option, index);
}

void ParameterValueDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    if (!index.isValid())
        return;
    QLineEdit* lineEdit = qobject_cast<QLineEdit*>(editor);
    if(lineEdit){
        QVariant unitSymbol = lineEdit->property("unitSymbol");
        if(unitSymbol.isValid()){
            bool ok = false;
            double value = UnitParser::parseUnit(lineEdit->text(), unitSymbol.toString(), &ok);
            if(ok){
                model->setData(index,QVariant(value));
            }
            else{               
                //DO something, for example draw a red background
            }
            return;
        }
    }
    QStyledItemDelegate::setModelData(editor, model, index);
}

void ParameterValueDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QVariant v = index.data(Qt::EditRole);    
    if(v.canConvert<ParameterValue>()){
        QLocale locale;
        ParameterValue paramValue = v.value<ParameterValue>();
        QLineEdit* lineEdit = qobject_cast<QLineEdit*>(editor);        
        lineEdit->setText(UnitParser::formatUnit(paramValue.value.toReal(), paramValue.symbol, locale));
    }
    else{
        QStyledItemDelegate::setEditorData(editor, index);
    }
}
