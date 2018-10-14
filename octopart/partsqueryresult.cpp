#include "partsqueryresult.h"

using namespace Octopart;

class Q_DECL_HIDDEN PartsQueryResult::Private : public QSharedData
{
public:
    Private(): count(0)
    {
    }

    QString error;
    int count;
    QList<PartSnippet> items;
};

PartsQueryResult::PartsQueryResult() : data(new PartsQueryResult::Private)
{

}

PartsQueryResult::PartsQueryResult(const PartsQueryResult &rhs) : data(rhs.data)
{

}

PartsQueryResult &PartsQueryResult::operator=(const PartsQueryResult &rhs)
{
    if (this != &rhs)
        data.operator=(rhs.data);
    return *this;
}

PartsQueryResult::~PartsQueryResult()
{

}

QString PartsQueryResult::error() const
{
    return data->error;
}

void PartsQueryResult::setError(const QString &value)
{
    data->error = value;
}

int PartsQueryResult::count() const
{
    return data->count;
}

void PartsQueryResult::setCount(int value)
{
    data->count = value;
}

QList<PartSnippet> PartsQueryResult::items() const
{
    return data->items;
}

void PartsQueryResult::setItems(const QList<PartSnippet> &value)
{
    data->items = value;
}
