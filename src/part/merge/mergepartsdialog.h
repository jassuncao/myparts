#ifndef MERGEPARTSDIALOG_H
#define MERGEPARTSDIALOG_H

#include <QDialog>
#include <QVariant>
#include <QList>

class QTableWidget;
class QTableWidgetItem;
class MergeHelper;

class MergePartsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit MergePartsDialog(QWidget *parent = 0);
    virtual ~MergePartsDialog();
    void setParts(const QVariant & partA, const QVariant & partB);
public slots:
    void accept();
    void reject();
protected:
    void closeEvent(QCloseEvent *);
private slots:
    void slotPartAOnlyClicked();
    void slotPartBOnlyClicked();
    void slotItemChanged(QTableWidgetItem *item);
private:
    void writeSettings();
    void readSettings();
    void selectSmartMerge();
    QList<QTableWidgetItem*> loadPart(QVariant partId);    
    void loadParts();
    void toggleSelectedItems(int toCheckCol, int toUncheckCol);
    QVariant _partA;
    QVariant _partB;
    QTableWidget * _mergeTable;
    MergeHelper * _mergeHelper;
};

#endif // MERGEPARTSDIALOG_H
