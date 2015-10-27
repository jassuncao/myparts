#ifndef COMPANYMANAGERVIEW_H
#define COMPANYMANAGERVIEW_H

#include "minisplitter.h"
#include "editors/abstracteditor.h"
#include <QModelIndex>
#include <QWidget>
#include <QLabel>
#include <QSqlRecord>

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

class EditorManagerHelper : public QObject
{
public:
    virtual QString mainTitle() const = 0;
    virtual QString deleteButtonText() const = 0;
    virtual QString saveNewButtonText() const = 0;
    virtual QString saveChangesButtonText() const = 0;
    virtual AbstractEditor* createEditor() const = 0;
    virtual QWidget* createNoDataWidget() const = 0;
    virtual QString tableName() const = 0;
    virtual int itemLabelColumn() const = 0;
    virtual int itemIDColumn() const = 0;
    virtual QString createFilterExpression(const QString & filterText) const = 0;
};


class DistributorManagerHelper : public EditorManagerHelper
{
public:
    virtual QString mainTitle() const { return tr("Distributors"); }
    virtual QString deleteButtonText() const { return tr("Delete distributor"); }
    virtual QString saveNewButtonText() const { return tr("Save distributor"); }
    virtual QString saveChangesButtonText() const { return tr("Save changes"); }
    virtual AbstractEditor* createEditor() const;
    virtual QWidget* createNoDataWidget() const;
    virtual QString tableName() const { return QLatin1String("distributor"); }
    virtual int itemLabelColumn() const { return 1; }
    virtual int itemIDColumn() const  { return 0; }
    virtual QString createFilterExpression(const QString & filterText) const { return QString("name LIKE '\%%1\%'").arg(filterText);}
};

class ManufacturerManagerHelper : public EditorManagerHelper
{
public:
    virtual QString mainTitle() const { return tr("Manufacturers"); }
    virtual QString deleteButtonText() const { return tr("Delete manufacturer"); }
    virtual QString saveNewButtonText() const { return tr("Save manufacturer"); }
    virtual QString saveChangesButtonText() const { return tr("Save changes"); }
    virtual AbstractEditor* createEditor() const;
    virtual QWidget* createNoDataWidget() const;
    virtual QString tableName() const { return QLatin1String("manufacturer"); }
    virtual int itemLabelColumn() const { return 1; }
    virtual int itemIDColumn() const  { return 0; }
    virtual QString createFilterExpression(const QString & filterText) const { return QString("name LIKE '\%%1\%'").arg(filterText);}
};

class FootprintManagerHelper : public EditorManagerHelper
{
public:
    virtual QString mainTitle() const { return tr("Footprints"); }
    virtual QString deleteButtonText() const { return tr("Delete footprint"); }
    virtual QString saveNewButtonText() const { return tr("Save footprint"); }
    virtual QString saveChangesButtonText() const { return tr("Save changes"); }
    virtual AbstractEditor* createEditor() const;
    virtual QWidget* createNoDataWidget() const;
    virtual QString tableName() const { return QLatin1String("part_footprint"); }
    virtual int itemLabelColumn() const { return 1; }
    virtual int itemIDColumn() const  { return 0; }
    virtual QString createFilterExpression(const QString & filterText) const { return QString("name LIKE '\%%1\%'").arg(filterText);}
};

class EditorManagerView : public Manhattan::MiniSplitter
{
    Q_OBJECT
public:
    explicit EditorManagerView(const EditorManagerHelper * helper, QWidget *parent = 0);
    ~EditorManagerView();
    void setModel(QAbstractTableModel * model);
private slots:
    void slotAddItem();
    void slotDeleteItem(const QModelIndex &index);
    void slotFilterChanged(const QString & filterText);
    void slotAccept();
    void slotReject();
    void slotDelete();
    void slotItemSelected(const QModelIndex &index);
    void slotContentChanged();
    void slotPrimeInsert(int, QSqlRecord &record);
private:
    int findRowNumber(QVariant idValue);
    void restoreCurrentIndex(const QModelIndex & index);
    QVariant commitChanges();
    bool discardChangesConfirmation();

    const EditorManagerHelper * _helper;
    ListNavigatorWidget * _navigatorWidget;
    AbstractEditor * _editorWidget;
    QSqlTableModel * _model;
    QStackedLayout * _stackedLayout;
    QDialogButtonBox * _buttonBox;
    QPushButton * _saveButton;
    QPushButton * _deleteButton;
    QPushButton * _cancelButton;
    bool _dirty;
};

#endif // COMPANYMANAGERVIEW_H
