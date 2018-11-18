#ifndef PARTSQUERYRESULT_H
#define PARTSQUERYRESULT_H

#include "octopart_global.h"
#include <QSharedDataPointer>
#include <QList>
#include "partsnippet.h"

namespace Octopart
{

class OCTOPART_SHARED_EXPORT PartsQueryResult
{
public:
    PartsQueryResult();
    PartsQueryResult(const PartsQueryResult &);
    PartsQueryResult &operator=(const PartsQueryResult &);
    ~PartsQueryResult();
    QString error() const;
    void setError(const QString &value);
    int count() const;
    void setCount(int value);
    QList<PartSnippet> items() const;
    void setItems(const QList<PartSnippet> &value);
private:
    class Private;
    QSharedDataPointer<Private> data;
};

} // namespace

#endif // PARTSQUERYRESULT_H
