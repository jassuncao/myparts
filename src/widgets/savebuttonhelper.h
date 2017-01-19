#ifndef SAVEBUTTONHELPER_H
#define SAVEBUTTONHELPER_H

#include <QObject>
#include <QDialogButtonBox>

class QLineEdit;
class QComboBox;
class QPushButton;
class QDialogButtonBox;
class QSpinBox;

class SaveButtonHelper : public QObject
{
    Q_OBJECT
public:
    explicit SaveButtonHelper(QObject *parent = 0);
    void monitor(QLineEdit * lineEdit);
    void monitor(QComboBox * combo);
    void monitor(QSpinBox * spinBox);
    void setSaveButton(QPushButton * saveButton);
    void setResetButton(QPushButton * resetButton);
    void setButtonBox(QDialogButtonBox * buttonBox, QDialogButtonBox::StandardButton saveRole = QDialogButtonBox::Save);
signals:
    void dataChanged();
public slots:
    void reset();
protected slots:
    void slotDataChanged();
protected:
    void updateButtons();
    QPushButton * _saveButton;
    QPushButton * _resetButton;
    bool _hasChanges;
};

#endif // SAVEBUTTONHELPER_H
