#ifndef OCTOPARTAPI_H
#define OCTOPARTAPI_H

#include <QObject>
#include <QList>
#include "qjsonobject.h"
#include "qjsondocument.h"

namespace Octopart {

class PartBrief {
public:
    PartBrief();
    bool read(const QJsonObject & json);
private:
    QString _uid;
    QString _mpn;
    QString _url;
    QString _description;
    QString _brandName;
};

class PartsMatchResult {
public:
    bool read(const QJsonObject & json);
    inline int count() const { return _count;}
    inline QList<PartBrief> items() const { return _items;}
    inline QString error() const { return _error;}
private:
    int _start;
    int _limit;
    int _count;
    QList<PartBrief> _items;
    QString _error;
};

class PartsMatchResponseParser {
public:
    PartsMatchResult parse(const QByteArray &json, QJsonParseError *error) const;
};

class OctopartAPI : public QObject
{
    Q_OBJECT
public:
    OctopartAPI(const QString &apiKey, QObject * parent = 0);
    QList<PartBrief> partsMatch(const QString & mpn, int start=0, int limit=10);
private:
    QString _apiUrl;
    QString _apiKey;
};
}


#endif // OCTOPARTAPI_H
