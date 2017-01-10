#ifndef PARTPICKER_H
#define PARTPICKER_H

#include <QDialog>
#include <QVariant>

class QTreeBox;
class QTableView;
class PartsTableView;
class PartsFilterWidget;
class ModelsRepository;
class PartsQueryBuilder;
class PartsSqlTableModel;
class CategoryTreeModel;
class TreeComboFilterItemWidget;

class PartPickerDialog : public QDialog
{
    Q_OBJECT
public:
    explicit PartPickerDialog(ModelsRepository *modelsRepo, QWidget *parent = 0);
    virtual ~PartPickerDialog();
    void accept();
    inline QVariant selectedPart() {return _selectedPart;}
private:
    PartsQueryBuilder * const _partsQueryBuilder;
    PartsFilterWidget * _partsFilterWidget;
    PartsTableView * _partsTable;
    PartsSqlTableModel * _partsModels;
    QVariant _selectedPart;
};

#endif // PARTPICKER_H
