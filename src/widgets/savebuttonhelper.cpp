#include "savebuttonhelper.h"

#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QSpinBox>

SaveButtonHelper::SaveButtonHelper(QObject *parent) :
    QObject(parent),
    _saveButton(0),
    _resetButton(0),
    _hasChanges(false)
{

}

void SaveButtonHelper::monitor(QLineEdit * lineEdit)
{
    connect(lineEdit, SIGNAL(textChanged(QString)), this, SLOT(slotDataChanged()));
}

void SaveButtonHelper::monitor(QComboBox * combo)
{
    connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(slotDataChanged()));
}

void SaveButtonHelper::monitor(QSpinBox * spinBox)
{
    connect(spinBox, SIGNAL(valueChanged(int)), this, SLOT(slotDataChanged()));
}

void SaveButtonHelper::reset()
{
    _hasChanges = false;
    updateButtons();
}

void SaveButtonHelper::setSaveButton(QPushButton * saveButton)
{
    _saveButton = saveButton;
    if(_saveButton){
        _saveButton->setEnabled(_hasChanges);
    }
}

void SaveButtonHelper::setResetButton(QPushButton * resetButton)
{
    _resetButton = resetButton;
    if(_resetButton){
        _resetButton->setEnabled(_hasChanges);
    }
}

void SaveButtonHelper::setButtonBox(QDialogButtonBox * buttonBox, QDialogButtonBox::StandardButton saveRole)
{
    setSaveButton(buttonBox->button(saveRole));
    setResetButton(buttonBox->button(QDialogButtonBox::Reset));
}

void SaveButtonHelper::slotDataChanged()
{
    bool oldValue = _hasChanges;
    _hasChanges = true;
    if(oldValue == false){
        updateButtons();
        emit dataChanged();
    }
}

void SaveButtonHelper::updateButtons()
{
    if(_saveButton){
        _saveButton->setEnabled(_hasChanges);
    }
    if(_resetButton){
        _resetButton->setEnabled(_hasChanges);
    }
}
