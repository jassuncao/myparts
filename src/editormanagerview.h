#ifndef COMPANYMANAGERVIEW_H
#define COMPANYMANAGERVIEW_H

#include "models/basicentitytablemodel.h"
#include "minisplitter.h"
#include <QModelIndex>
#include <QWidget>
#include <QLabel>
#include <QSqlRecord>
#include <QCoreApplication>

namespace Manhattan {
class StyledBar;
class MiniSplitter;
}

class ListNavigatorWidget;
class CompanyEditorWidget;
class QSqlTableModel;
class QStackedLayout;
class QDialogButtonBox;
class QPushButton;
class AbstractEditor;
class BasicEntityTableModel;


class EditorManagerHelper
{
public:
    virtual QString mainTitle() const = 0;
    virtual QString deleteButtonText() const = 0;
    virtual QString saveNewButtonText() const = 0;
    virtual QString saveChangesButtonText() const = 0;
    virtual AbstractEditor* createEditor() const = 0;
    virtual QWidget* createNoDataWidget() const = 0;        
};


class DistributorManagerHelper : public EditorManagerHelper
{
public:
    QString mainTitle() const { return QCoreApplication::translate("DistributorManagerHelper", "Distributors"); }
    QString deleteButtonText() const { return QCoreApplication::translate("DistributorManagerHelper", "Delete distributor"); }
    QString saveNewButtonText() const { return QCoreApplication::translate("DistributorManagerHelper", "Save distributor"); }
    QString saveChangesButtonText() const { return QCoreApplication::translate("DistributorManagerHelper", "Save changes"); }
    AbstractEditor* createEditor() const;
    QWidget* createNoDataWidget() const;
};

class ManufacturerManagerHelper : public EditorManagerHelper
{
public:
    QString mainTitle() const { return QCoreApplication::translate("ManufacturerManagerHelper", "Manufacturers"); }
    QString deleteButtonText() const { return QCoreApplication::translate("ManufacturerManagerHelper", "Delete manufacturer"); }
    QString saveNewButtonText() const { return QCoreApplication::translate("ManufacturerManagerHelper", "Save manufacturer"); }
    QString saveChangesButtonText() const { return QCoreApplication::translate("ManufacturerManagerHelper", "Save changes"); }
    AbstractEditor* createEditor() const;
    QWidget* createNoDataWidget() const;
};

class PackageManagerHelper : public EditorManagerHelper
{
public:
    QString mainTitle() const { return QCoreApplication::translate("PackageManagerHelper", "Packages"); }
    QString deleteButtonText() const { return QCoreApplication::translate("PackageManagerHelper", "Delete package"); }
    QString saveNewButtonText() const { return QCoreApplication::translate("PackageManagerHelper", "Save package"); }
    QString saveChangesButtonText() const { return QCoreApplication::translate("PackageManagerHelper", "Save changes"); }
    AbstractEditor* createEditor() const;
    QWidget* createNoDataWidget() const;
//    virtual QString tableName() const { return QLatin1String("package"); }
//    virtual int itemLabelColumn() const { return 1; }
//    virtual int itemIDColumn() const  { return 0; }
//    virtual QString createFilterExpression(const QString & filterText) const { return QString("name LIKE '\%%1\%'").arg(filterText);}
};

class EditorManagerView : public Manhattan::MiniSplitter
{
    Q_OBJECT
public:
    explicit EditorManagerView(const EditorManagerHelper * helper, BasicEntityTableModel *model, QWidget *parent = 0);
    ~EditorManagerView();
private slots:
    void slotAddItem();
    void slotDeleteItem(const QModelIndex &index);
    void slotFilterChanged(const QString & filterText);
    void slotAccept();
    void slotReject();
    void slotDelete();
    void slotItemSelected(const QModelIndex &index);
    void slotContentChanged();
    //void slotPrimeInsert(int, QSqlRecord &record);
private:
    int findRowNumber(QVariant idValue);
    void restoreCurrentIndex(const QModelIndex & index);
    QVariant commitChanges();
    bool discardChangesConfirmation();

    const EditorManagerHelper * _helper;
    ListNavigatorWidget * _navigatorWidget;
    AbstractEditor * _editorWidget;
    BasicEntityTableModel * _model;
    QStackedLayout * _stackedLayout;
    QDialogButtonBox * _buttonBox;
    QPushButton * _saveButton;
    QPushButton * _deleteButton;
    QPushButton * _cancelButton;
    bool _dirty;
};

#endif // COMPANYMANAGERVIEW_H
