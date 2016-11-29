#ifndef PARTPICKER_H
#define PARTPICKER_H

#include <QComboBox>

class PartPickerView;

class PartPicker : public QComboBox
{
    Q_OBJECT
public:
    explicit PartPicker(QWidget *parent = 0);
    virtual void showPopup();
    virtual void hidePopup();

private:
    bool eventFilter(QObject* object, QEvent* event);
    bool _skipNextHide;
};

class QTreeBox;
class QTableView;

class PartPickerView : public QWidget
{
    Q_OBJECT
public:
    explicit PartPickerView(QWidget *parent = 0);
private:
    QTreeBox * _categoryCombo;
    QTableView * _partsView;
};

#endif // PARTPICKER_H
