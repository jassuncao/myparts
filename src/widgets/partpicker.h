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
class PartsTableView;
class PartsFilterWidget;
class ModelsRepository;
class PartsQueryBuilder;
class PartsSqlTableModel;

class PartPickerView : public QWidget
{
    Q_OBJECT
public:
    explicit PartPickerView(ModelsRepository *modelsRepo, QWidget *parent = 0);
    virtual ~PartPickerView();
private slots:
    void slotFilterChanged();
private:
    PartsQueryBuilder * _partsQueryBuilder;
    PartsFilterWidget * _partsFilterWidget;
    PartsTableView * _partsTable;
    PartsSqlTableModel * _partsModels;
};

#endif // PARTPICKER_H
