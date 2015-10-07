#include "relationalcombobox.h"
#include <QDebug>

RelationalComboBox::RelationalComboBox(QWidget *parent)
    : QComboBox(parent),
      _modelKeyColumn(-1)
{    
    connect(this,SIGNAL(currentIndexChanged(int)), this, SIGNAL(currentKeyChanged()));
}

RelationalComboBox::~RelationalComboBox()
{
}

int RelationalComboBox::modelKeyColumn() const
{
    return _modelKeyColumn;
}

void RelationalComboBox::setModelKeyColumn(int keyColumn)
{
    _modelKeyColumn = keyColumn;
}

QVariant RelationalComboBox::currentKey() const
{
    int row = currentIndex();
    if(row<0){
        return QVariant();
    }
    return model()->index(row, _modelKeyColumn).data(Qt::EditRole);
}

void RelationalComboBox::setCurrentKey(const QVariant & key)
{
    int index(-1);
    if(!key.isNull()){
        //Look up for the matching row.
        int rowCount = model()->rowCount();
        for(int i=0; i<rowCount;++i){
            QVariant otherKey = model()->index(i,_modelKeyColumn).data();
            if(key==otherKey){
                index = i;
                break;
            }
        }
    }
    setCurrentIndex(index);
}

