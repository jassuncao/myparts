#ifndef OCTOPARTIMPORTDIALOG_H
#define OCTOPARTIMPORTDIALOG_H

#include <QDialog>
#include <QModelIndex>
#include <QCache>
#include "octopartapi.h"

namespace Ui {
class OctopartImportDialog;
}

namespace Octopart {
class OctopartPartSearchModel;
class Part;
}

class QStandardItemModel;
class QProgressIndicator;

class OctopartImportDialog : public QDialog
{
    Q_OBJECT

public:
    explicit OctopartImportDialog(QWidget *parent = 0);
    ~OctopartImportDialog();
public slots:
    void search(const QString &searchTerm);
private slots:
    void on_searchButton_clicked();
    void slotBusy();
    void slotReady();
    void slotError(const QString &error);
    void slotSearchFinished(int hits);
    void slotPartSelected(const QModelIndex &index);
    void slotRequestFinished(const Octopart::PartFullResponse& response);
    void slotToggleOfferCheckboxes(Qt::CheckState state);
    void slotToggleDatasheetCheckboxes(Qt::CheckState state);
    void slotToggleParameterCheckboxes(Qt::CheckState state);
    void slotDoSearch();

private:
    Ui::OctopartImportDialog *_ui;
    Octopart::OctopartAPI * _api;
    Octopart::OctopartPartSearchModel *_searchModel;
    QProgressIndicator *_progress;
    QCache<QString,Octopart::Part> _partsCache;    
    void initOffersTable();
    void initDatasheetsTable();
    void initParametersTable();
    void showPartDetails(Octopart::Part * part);
    void loadOffersTable(const QList<Octopart::Offer> &offers);
    void loadDatasheetTable(const QList<Octopart::Datasheet> &datasheets);
    void loadParametersTable(const QList<Octopart::PartSpec> &specs);
};

#endif // OCTOPARTIMPORTDIALOG_H
