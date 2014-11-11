#include "optionsdialog.h"
#include "widgets/partunitswidget.h"
#include "widgets/partconditionwidget.h"
#include <QtGui>
#include "widgets/optionswidget.h"

OptionsDialog::OptionsDialog(OptionsWidget * optionsWidget, QWidget *parent) :
    QDialog(parent), _dirty(false), _optionsWidget(optionsWidget)
{

    //_partUnitWidget = new PartUnitsWidget(this);
    //_partConditionWidget = new PartConditionWidget(this);

    _buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Apply | QDialogButtonBox::Cancel );

    _applyButton = _buttonBox->button(QDialogButtonBox::Apply);
    _applyButton->setEnabled(false);
    connect(_buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(_buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    connect(_applyButton, SIGNAL(clicked()), this, SLOT(apply()));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(_optionsWidget);
    //mainLayout->addWidget(_partUnitWidget);
    //mainLayout->addWidget(_partConditionWidget);
    mainLayout->addWidget(_buttonBox);
    setLayout(mainLayout);

    connect(_optionsWidget,SIGNAL(dataChanged()), this, SLOT(dataChanged()));
    //connect(_partUnitWidget,SIGNAL(dataChanged()), this, SLOT(dataChanged()));
    //connect(_partConditionWidget,SIGNAL(dataChanged()), this, SLOT(dataChanged()));
    //connect(_partUnitWidget, SIGNAL(dataCommited()), this, SLOT(setDirty(bool)));
}


void OptionsDialog::accept()
{
    apply();
    QDialog::accept();    
}

void OptionsDialog::reject()
{
    setDirty(false);
    QDialog::reject();
}

void OptionsDialog::apply()
{
    if(_optionsWidget->submit()){
        setDirty(false);
    }
    else{
        qDebug("Failed to sub");
    }
}

void OptionsDialog::dataChanged()
{
    setDirty(true);
}

void OptionsDialog::setDirty(bool dirty)
{
    _dirty = dirty;
    _applyButton->setEnabled(_dirty);
}
