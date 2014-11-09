#ifndef OPTIONSDIALOG_H
#define OPTIONSDIALOG_H

#include <QDialog>

class PartUnitsWidget;
class PartConditionWidget;
class QDialogButtonBox;
class QPushButton;

class OptionsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit OptionsDialog(QWidget *parent = 0);    
signals:    
    
public slots:

private slots:
    void accept();
    void reject();
    void apply();
    void dataChanged();
    void setDirty(bool dirty);
private:
    bool _dirty;
    PartUnitsWidget * _partUnitWidget;
    //PartConditionWidget * _partConditionWidget;
    QPushButton * _applyButton;
    QPushButton * _revertButton;
    QPushButton * _quitButton;
    QDialogButtonBox * _buttonBox;
};

#endif // OPTIONSDIALOG_H
