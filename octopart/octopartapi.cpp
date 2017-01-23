#include "octopartapi.h"

#include <QDebug>
#include <QUrl>
#include "qjsonarray.h"
#include "qjsondocument.h"

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

    QUrl url(_apiUrl+"parts/match");
    url.addQueryItem("apikey", _apiKey);
    url.addQueryItem("queries", QString(ba));
    url.addQueryItem("include[]", "short_description");
    url.addQueryItem("show[]", "mpn");
    url.addQueryItem("show[]", "brand.name");
    url.addQueryItem("show[]", "octopart_url");
    url.addQueryItem("show[]", "short_description");
    url.addQueryItem("show[]", "uid");


    qDebug()<<"Request URL "<<qPrintable(url.toEncoded());

    return QList<PartBrief>();
}

}
