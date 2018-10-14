#include "octopartpartsearchmodel.h"

namespace Octopart {

OctopartPartSearchModel::OctopartPartSearchModel(OctopartAPI * api, QObject *parent) :
    QAbstractTableModel(parent),
    _api(api),
    _hits(0),
    _rows(0),
    _activeRequest(-1),
    _state(Initializing)
{
    connect(_api, SIGNAL(requestFinished(Octopart::PartsQueryResponse)), this, SLOT(slotRequestFinished(Octopart::PartsQueryResponse)));
    connect(_api, SIGNAL(requestError(Octopart::ErrorResponse)), this, SLOT(slotRequestError(Octopart::ErrorResponse)));
}

OctopartPartSearchModel::~OctopartPartSearchModel()
{
}

QVariant OctopartPartSearchModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal) {
        QVariant val = _headers.value(section).value(role);
        if (role == Qt::DisplayRole && !val.isValid())
            val = _headers.value(section).value(Qt::EditRole);
        if (val.isValid())
            return val;
    }
    return QAbstractItemModel::headerData(section, orientation, role);
}

bool OctopartPartSearchModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
    if (orientation != Qt::Horizontal || section < 0 || columnCount() <= section)
        return false;

    if (_headers.size() <= section)
        _headers.resize(qMax(section + 1, 16));
    _headers[section][role] = value;
    emit headerDataChanged(orientation, section, section);
    return true;
}

int OctopartPartSearchModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : _rows;
}

int OctopartPartSearchModel::columnCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : 5;
}

Qt::ItemFlags OctopartPartSearchModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags theFlags = QAbstractTableModel::flags(index);
    if (index.isValid()){
        theFlags |= Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    }
    return theFlags;
}

QVariant OctopartPartSearchModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= _parts.size() || index.row() < 0)
        return QVariant();

    if (role == Qt::DisplayRole) {
        const PartSnippet & part = _parts.at(index.row());
        int column = index.column();
        switch (column) {
        case ColumnMpn:
            return part.mpn();
        case ColumnBrand:
            return part.brandName();
        case ColumnDescription:
            return part.description();
        case ColumnUrl:
            return part.url();
        case ColumnUid:
            return part.uid();
        default:
            return QVariant();
            break;
        }
    }
    return QVariant();
}

void OctopartPartSearchModel::searchByMpn(const QString & mpn)
{
    beginResetModel();
    _query = mpn;
    _hits = 0;
    _rows = 0;
    if(_query.isEmpty()){
      _state = Ready;
    }
    else{
        _state = Initializing;
    }
    endResetModel();
}

void OctopartPartSearchModel::searchByText(const QString & text)
{

}

void OctopartPartSearchModel::slotRequestFinished(const PartsQueryResponse &response)
{

    if(response.requestId != _activeRequest){
        return;
    }
    _activeRequest = -1;
    emit ready();

    const PartsQueryResult result = response.result;
    if(_state == Initializing){
        _parts.clear();
        _hits = result.count();
        _rows = qMin(_hits, 100);
        _state = Ready;
        if(_hits == 0){
            emit noMatchesFound();
        }
    }

    if(result.items().size() > 0){
        int first = _parts.size();
        int last = first + result.items().size() - 1;
        beginInsertRows(QModelIndex(), first, last);
        _parts.append(result.items());
        endInsertRows();
    }


}

void OctopartPartSearchModel::slotRequestError(const Octopart::ErrorResponse& response)
{
    if(response.requestId != _activeRequest){
        return;
    }
    _activeRequest = -1;
    emit ready();
    emit error(response.result);
}

void OctopartPartSearchModel::fetchMore(const QModelIndex &)
{    
    if(_activeRequest >= 0 || _query.isEmpty()) return;

    int limit;
    int start;
    if(_state == Initializing){
        limit = 10;
        start = 0;
    }
    else{
        int remainder = _rows - _parts.size();
        start = _parts.size();
        limit = qMin(10, remainder);
    }
    _activeRequest = _api->partsSearch(_query, start, limit);
    //_activeRequest = _api->partsMatch(_query, start, limit);
    emit busy();
}

bool OctopartPartSearchModel::canFetchMore(const QModelIndex &) const
{
    return _state == Initializing || _parts.size() < _rows;
}

}


