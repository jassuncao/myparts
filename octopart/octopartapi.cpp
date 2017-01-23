#include "octopartapi.h"

#include <QDebug>
#include <QUrl>

#include "qjsonarray.h"
#include "qjsondocument.h"

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QUrlQuery>

class UrlQueryBuilder {
public:
    UrlQueryBuilder(const QString &queryString) :
        _urlQuery(queryString)
    {}
    void addQueryItem(const QString &key, const QString &value)
    {
        _urlQuery.addQueryItem(key, value);
    }

    QUrl url(){
        QUrl url;
        url.setQuery(_urlQuery);
        return url;
    }

private:
    QUrlQuery _urlQuery;
};

#else

class UrlQueryBuilder {
public:
    UrlQueryBuilder(const QString &queryString) :
        _url(queryString)
    {}
    void addQueryItem(const QString &key, const QString &value)
    {
        _url.addQueryItem(key, value);
    }

    QUrl url(){
        return _url;
    }

private:
    QUrl _url;
};

#endif



namespace Octopart {



static QString readStringValue(const QJsonObject & json, const QString & key, bool * ok){
    Q_ASSERT(ok);
    QString strValue;
    const QJsonValue & jsonValue = json[key];
    if(jsonValue.isUndefined()){
        qWarning("Field %s is missing", qPrintable(key));
        *ok = false;
    }
    else{
        *ok = *ok | true;
        strValue = jsonValue.toString();
    }
    return strValue;
}

static int readIntValue(const QJsonObject & json, const QString & key, bool * ok){
    Q_ASSERT(ok);
    int value(-1);
    const QJsonValue & jsonValue = json[key];
    if(jsonValue.isUndefined()){
        qWarning("Field %s is missing", qPrintable(key));
        *ok = false;
    }
    else{
        *ok = *ok | true;
        value = jsonValue.toInt(-1);
    }
    return value;
}

PartBrief::PartBrief()
{
}

bool PartBrief::read(const QJsonObject & json)
{
    bool ok = true;
    _uid = readStringValue(json, "uid", &ok);
    _mpn = readStringValue(json, "mpn", &ok);
    _url = readStringValue(json, "octopart_url", &ok);
    _description = readStringValue(json, "short_description", &ok);
    const QJsonValue & brand = json["brand"];
    if(brand.isObject()){
        _brandName = readStringValue(brand.toObject(), "name", &ok);
    }
    else{
       qWarning("Brand object missing");
       ok = false;
    }
    return ok;
}

bool PartsMatchResult::read(const QJsonObject & json)
{
    bool ok = true;
    _error = readStringValue(json, "error", &ok);
    _count = readIntValue(json, "hits", &ok);
    const QJsonValue & items = json["items"];

    if(items.isArray()){

        QJsonArray itemsArray = items.toArray();
        for (int i = 0; i < itemsArray.size(); ++i) {
                QJsonObject itemObject = itemsArray[i].toObject();
                PartBrief part;
                part.read(itemObject);
                _items.append(part);
            }
    }
    else{
       qWarning("Items array missing");
       ok = false;
    }
    return ok;
}

PartsMatchResult PartsMatchResponseParser::parse(const QByteArray &json, QJsonParseError *error) const
{
    PartsMatchResult result;
    QJsonParseError localError;
    QJsonDocument doc = QJsonDocument::fromJson(json, &localError);
    if(localError.error == QJsonParseError::NoError) {
        QJsonObject obj = doc.object();
        QJsonValue results =  obj["results"];
        if(results.isArray()){
            QJsonArray resArray = results.toArray();
            if(resArray.size()>0){
                result.read(resArray.at(0).toObject());
            }
            else{
                qWarning("Query returned no results");
            }
        }
    }
    else{
        qWarning("Error parsing PartsMatchResponse. %s", qPrintable(localError.errorString()));
    }
    if(error){
        *error = localError;
    }
    return result;
}

OctopartAPI::OctopartAPI(const QString & apiKey, QObject * parent) :
    QObject(parent),
    _apiUrl("https://octopart.com/api/v3/"),
    _apiKey(apiKey)
{}


QList<PartBrief> OctopartAPI::partsMatch(const QString & mpn, int start, int limit)
{
    QJsonArray queries;
    QJsonObject query;
    query["mpn"] = mpn;
    query["start"] = start;
    query["limit"] = limit;
    queries.append(query);
    QJsonDocument doc(queries);
    QByteArray ba = doc.toJson(QJsonDocument::Compact);


    UrlQueryBuilder queryBuilder(_apiUrl+"parts/match");
    queryBuilder.addQueryItem("apikey", _apiKey);
    queryBuilder.addQueryItem("queries", QString(ba));
    queryBuilder.addQueryItem("include[]", "short_description");
    queryBuilder.addQueryItem("show[]", "mpn");
    queryBuilder.addQueryItem("show[]", "brand.name");
    queryBuilder.addQueryItem("show[]", "octopart_url");
    queryBuilder.addQueryItem("show[]", "short_description");
    queryBuilder.addQueryItem("show[]", "uid");

    QUrl url = queryBuilder.url();
    qDebug()<<"Request URL "<<qPrintable(url.toEncoded());

    return QList<PartBrief>();
}

}
