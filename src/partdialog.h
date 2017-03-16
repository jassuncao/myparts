#ifndef PARTDIALOG_H
#define PARTDIALOG_H

#include <QDialog>
#include <QModelIndex>
#include <QSqlRecord>

namespace Ui {
class PartDialog;
}

class QSqlQueryModel;
class QModelIndex;
class QDataWidgetMapper;
class PartsSqlTableModel;
class QCheckBox;
class AttachmentTableModel3;
class PartManufacturerTableModel2;
class PartDistributorTableModel2;
class PartStockLogTableModel;
class ModelsRepository;
class PartParameterTableModel;
class PartStockTableModel2;

class PartDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit PartDialog(ModelsRepository *modelsProvider, /* bool addMode,*/ QWidget *parent = 0);
    ~PartDialog();    
    int initialStock() const;
    double partPrice() const;
    void setCurrentCategory(const QVariant & category);
    void setCurrentStorage(const QVariant & storage);
public slots:
    int addNewPart();
    int editPart(const QModelIndex &index);
    int duplicatePart(const QModelIndex &index, bool allData);
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
    void slotCurrentStockRowChanged(const QModelIndex &current, const QModelIndex &);
    //void slotPartStorageChanged(int idx);
    void slotPartCategoryChanged(int idx);
    void slotAttachmentDoubleClicked(const QModelIndex &index);
    void slotAddStockEntry();
    void slotRemoveStockEntry();
    void slotMoveStock();
private:
    void initCombos();
    void commitChanges();
    void discardChanges();
    void internalAddPart(const QModelIndex &index, bool copyAll);
    void setCurrentModelIndex(const QModelIndex &index);
    QSqlRecord copySomeData(const QModelIndex &index);
    QSqlRecord copyAllData(const QModelIndex &index);
    QModelIndex insertNewPart(QSqlRecord initialData);

    QCheckBox * _nextActionCheckbox;
    Ui::PartDialog *ui;    
    ModelsRepository * _modelsProvider;
    PartsSqlTableModel * _partsModel;
    PartParameterTableModel * _partParamsModel;    
    PartDistributorTableModel2 * _partDistributorModel;
    PartManufacturerTableModel2 * _partManufacturerModel;
    AttachmentTableModel3 * _partAttachmentModel;
    PartStockLogTableModel * _partStockLogModel;
    PartStockTableModel2 * _partStockModel2;    
    QSqlQueryModel * _partUnitsModel;
    QSqlQueryModel * _packagesModel;    

    QDataWidgetMapper * _mapper;
    QModelIndex _currentModelIndex;
    int _lastSelectedPackage;
    bool _addMode;
    QVariant _currentPartId;
    QVariant _defaultCondition;
    QVariant _defaultUnit;
    QVariant _currentCategory;
    QVariant _currentStorage;

};

#endif // PARTDIALOG_H
