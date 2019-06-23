#ifndef OCTOPARTIMPORTER_H
#define OCTOPARTIMPORTER_H

#include "octopartapi.h"
#include <QSqlDatabase>

class OctopartImporterQueries;
class EntitySelectionDialog;
class ModelsRepository;

class OctopartImporter
{
public:
    explicit OctopartImporter(ModelsRepository * modelsRepository,  QWidget *parent, QSqlDatabase &database);
    ~OctopartImporter();
    void import(const Octopart::Part &part);
private:
    void importOffer(const Octopart::Offer &offer);
    QVariant importDistributor(const Octopart::Seller &seller);
    ModelsRepository * _modelsRepository;
    OctopartImporterQueries* _queries;
    EntitySelectionDialog * _selectionDlg;

};

#endif // OCTOPARTIMPORTER_H
