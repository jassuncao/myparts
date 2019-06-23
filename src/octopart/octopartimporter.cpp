#include "octopartimporter.h"
#include <QDebug>
#include <QSqlError>
#include <QSqlQuery>

#include "models/customtablemodel.h"
#include "models/modelsrepository.h"
#include "entityselectiondialog.h"

class OctopartImporterQueries {
public:
    explicit OctopartImporterQueries(QSqlDatabase &database):
        _distributorByOctopartIdQuery(database),
        _manufacturerByOctopartIdQuery(database),
        _associateOctopartIdToDistributorQuery(database)
    {
        _distributorByOctopartIdQuery.prepare("SELECT distributor FROM octopart_distributor WHERE octopart_uid = ?");
        _manufacturerByOctopartIdQuery.prepare("SELECT manufacturer from octopart_manufacturer WHERE octopart_uid = ?");
        _associateOctopartIdToDistributorQuery.prepare("INSERT INTO octopart_distributor ('distributor','octopart_uid') VALUES (?,?)");
    }

    QVariant findDistributorByOctopartId(const QString &uid)
    {
        QSqlQuery &query = _distributorByOctopartIdQuery;
        query.bindValue(0, uid);
        if(query.exec()){
            if(query.next()){
                return query.value(0);
            }
            else{
                qDebug() << "No existing distributor found for UID " << uid;
            }
        }
        else{
            qWarning()<<"Query error in findDistributorByOctopartId:" << query.lastError();
        }
        return QVariant();
    }

    QVariant findManufacturerByOctopartId(const QString &uid)
    {
        QSqlQuery &query = _manufacturerByOctopartIdQuery;
        query.bindValue(0, uid);
        if(query.exec()){
            if(query.next()){

                return query.value(0);
            }
            else{
                qDebug() << "No existing manufacturer found for UID " << uid;
            }
        }
        else{
            qWarning()<<"Query error in findManufacturerByOctopartId:" << query.lastError();
        }
        return QVariant();
    }

    bool associateOctopartIdToDistributor(const QVariant &distributor, const QString &uid){
        QSqlQuery &query = _associateOctopartIdToDistributorQuery;
        query.bindValue(0, distributor);
        query.bindValue(1, uid);

        if(query.exec()){
            return true;
        }
        else{
            qWarning()<<"Query error in associateOctopartIdToDistributor:" << query.lastError();
            return false;
        }
    }

private:
   QSqlQuery _distributorByOctopartIdQuery;
   QSqlQuery _manufacturerByOctopartIdQuery;
   QSqlQuery _associateOctopartIdToDistributorQuery;
};

OctopartImporter::OctopartImporter(ModelsRepository * modelsRepository, QWidget *parent, QSqlDatabase &database)
    : _modelsRepository(modelsRepository)
{
    _queries = new OctopartImporterQueries(database);
    _selectionDlg = new EntitySelectionDialog(parent);
}

OctopartImporter::~OctopartImporter()
{
    delete _queries;
}

void OctopartImporter::import(const Octopart::Part &part)
{
    /*
     *  enum ColumnsIndex {
        ColumnDistributor,
        ColumnPartNumber,
        ColumnMinimumOrder,
        ColumnUnitPrice,
        ColumnCurrency,
        ColumnPackaging,
        ColumnDateTime
    };
    */
    //PartDistributorTableModel2 * distributorsModel;
    //part.offers().at(0).seller()

}

void OctopartImporter::importOffer(const Octopart::Offer &offer)
{
    //First check if the Seller was already imported
    //  Look for an entry in octopart_distributor with a matching uuid
    //    If not present prompt the user to select an existing distributor
    //    NOTE: Pre-select one with a similar name
    //    Use the distributors model to create a new entry
    //    Associate the new distributor with a new entry in octopart_distributor
    //
    const Octopart::Seller &seller = offer.seller();
    QVariant distributorId = _queries->findDistributorByOctopartId(seller.uid());
    if(!distributorId.isValid()){
        _selectionDlg->setModel(_modelsRepository->distributorModel());
        _selectionDlg->setTitle(EntitySelectionDialog::tr("Distributor not present"));
        const QString msg = EntitySelectionDialog::tr("The distributor %1 is not registered yet.\n"
                                  "You can choose to create a new entry or use an existing one");
        _selectionDlg->setMessage(msg.arg(seller.name()));
        _selectionDlg->exec();
        if(EntitySelectionDialog::USE_EXISTING == _selectionDlg->selectedOption()){
            distributorId = _selectionDlg->selectedIndex(CompanyTableModel::ColumnId).data(Qt::EditRole);
        }
        else{
            distributorId = importDistributor(seller);
        }
    }
    /*
    QSqlQuery query;
    query.prepare(QString("INSERT INTO %1 (name, description, icon, lft, rgt) VALUES (?,?,?,?,?)").arg(_tableName));
    query.bindValue(0,item->name());
    query.bindValue(1,item->description());
    query.bindValue(2,item->iconId());
    query.bindValue(3,position);
    query.bindValue(4,position+1);
    if(query.exec()){
        int id = query.lastInsertId().toInt();
        item->setId(id);
        return true;
    }
    */
}

QVariant OctopartImporter::importDistributor(const Octopart::Seller &seller)
{
    CompanyTableModel* model = _modelsRepository->distributorModel();
    int row = model->rowCount();
    if(model->insertRow(row)){
        const QModelIndex nameIdx = model->index(row, CompanyTableModel::ColumnName);
        const QModelIndex websiteIdx = model->index(row, CompanyTableModel::ColumnWebsite);
        model->setData(nameIdx, seller.name());
        model->setData(websiteIdx, seller.url());
        if(model->submit()){
            qDebug()<<"Distributor "<<seller.name() << " imported";
            QVariant distributorId =  model->lastInsertedId();
            _queries->associateOctopartIdToDistributor(distributorId, seller.uid());
            return distributorId;
        }
    }
    qWarning()<<"Unable to create new distributor";
    return QVariant();
}


