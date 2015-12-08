#ifndef PARTDIALOG_H
#define PARTDIALOG_H

#include <QDialog>
#include <QModelIndex>

namespace Ui {
class PartDialog;
}

class QSqlRelationalTableModel;
class QSqlTableModel;
class QSqlQueryModel;
class QModelIndex;
class QDataWidgetMapper;
class TreeItemModel;
class PartsSqlTableModel;
class QCheckBox;
class QAbstractButton;
class PartParameterTableModel;
class PartDistributorTableModel;
class PartManufacturerTableModel;
class AttachmentTableModel;
class AttachmentTableModel3;
class PartParameterTableModel2;
class PartManufacturerTableModel2;
class PartDistributorTableModel2;
class PartParametersTableModel3;

class PartDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit PartDialog(PartsSqlTableModel *model, TreeItemModel *categoryModel, TreeItemModel* storageModel, /* bool addMode,*/ QWidget *parent = 0);
    ~PartDialog();    
    int initialStock() const;
    double partPrice() const;
public slots:
    int addNewPart();
    int editPart(const QModelIndex &index);
    int duplicatePart(const QModelIndex &index);
    void reject();
    void accept();
private slots:
    int exec();
    void slotPackageChanged(int index);
    void slotDeselectPackage();
    void slotUsePackage();
    void slotAddParameter();
    void slotDeleteParameter();
    void slotAddDistributor();
    void slotDeleteDistributor();
    void slotAddManufacturer();
    void slotDeleteManufacturer();
    void slotAddAttachment();
    void slotDeleteAttachment();
    void slotCurrentPartParameterRowChanged(const QModelIndex &current, const QModelIndex &);    
    void slotCurrentPartDistributorRowChanged(const QModelIndex &current, const QModelIndex &);
    void slotCurrentPartManufacturerRowChanged(const QModelIndex &current, const QModelIndex &);
    void slotCurrentPartAttachmentRowChanged(const QModelIndex &current, const QModelIndex &);
    void slotPartStorageChanged(int idx);
    void slotPartCategoryChanged(int idx);
    void slotAttachmentDoubleClicked(const QModelIndex &index);

private:
    void reset();
    void initCombos();
    void commitChanges();
    void discardChanges();
    void internalAddPart(const QModelIndex &index);
    void setCurrentModelIndex(const QModelIndex &index);

    QCheckBox * _nextActionCheckbox;
    Ui::PartDialog *ui;
    PartsSqlTableModel * _partsModel;
    PartParametersTableModel3 * _partParamsModel;
    PartDistributorTableModel2 * _partDistributorModel;
    PartManufacturerTableModel2 * _partManufacturerModel;
    AttachmentTableModel3 * _partAttachmentModel;
    TreeItemModel * _categoryModel;
    TreeItemModel * _storageModel;
    QSqlQueryModel * _partConditionModel;
    QSqlQueryModel * _partUnitsModel;
    QSqlQueryModel * _packagesModel;
    QDataWidgetMapper * _mapper;
    QModelIndex _currentModelIndex;
    int _lastSelectedPackage;
    bool _addMode;
    QVariant _currentPartId;

};

#endif // PARTDIALOG_H
