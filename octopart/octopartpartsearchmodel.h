#ifndef OCTOPARTPARTSEARCHMODEL_H
#define OCTOPARTPARTSEARCHMODEL_H

#include <QAbstractTableModel>
#include <QVector>
#include <QList>
#include "octopartapi.h"

namespace Octopart {

class OctopartPartSearchModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    enum Columns {
        ColumnMpn,
        ColumnBrand,
        ColumnDescription,
        ColumnUrl,
        ColumnUid
    };

    explicit OctopartPartSearchModel(Octopart::OctopartAPI *api, QObject *parent = 0);
    ~OctopartPartSearchModel();
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role = Qt::EditRole);
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;    
    virtual void fetchMore(const QModelIndex &);
    virtual bool canFetchMore(const QModelIndex &) const;
    int hits() const { return _hits;}
signals:
    void busy();
    void ready();
    void noMatchesFound();
public slots:
    void searchByMpn(const QString & mpn);
private slots:
    void slotPartsMatchResultFinished(int id, Octopart::PartsMatchResult result);
private:
    enum ModelState {
        Initializing,
        Ready,
    };

    Octopart::OctopartAPI * _api;
    int _hits;//The number of parts matching
    int _rows;//The maximum number of parts we can fetch (up to 100)
    QVector<QHash<int, QVariant> > _headers;
    int _activeRequest;
    QList<PartBrief> _parts;
    ModelState _state;
    QString _query;
};

}

#endif // OCTOPARTPARTSEARCHMODEL_H
