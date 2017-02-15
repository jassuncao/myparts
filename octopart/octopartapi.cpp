#include "octopartapi.h"

#include <QDebug>
#include <QUrl>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QStringList>

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

enum RequestType {
    PartsMatch

};


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

static QVariant readNumberValue(const QJsonObject & json, const QString & key)
{
    const QJsonValue & jsonValue = json.value(key);
    if(jsonValue.isDouble()){
        return QVariant(jsonValue.toDouble());
    }
    return QVariant(QVariant::Double);
}

///Accepts a Datasheet or SpecValue and returns the first source name
static QString readSourceName(const QJsonObject & json, bool * ok){
    Q_ASSERT(ok);
    const QJsonValue & attribution = json.value("attribution");
    if(attribution.isObject()){
        const QJsonValue & sources = attribution.toObject().value("sources");
        if(sources.isArray()){
            QJsonArray tmp = sources.toArray();
            if(!tmp.isEmpty()){
                QJsonValue source = tmp.at(0);
                return readStringValue(source.toObject(), "name", ok);
            }
        }
    }
    *ok = false;
    return QString();
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

Datasheet::Datasheet() :
    _numPages(-1)
{
}

bool Datasheet::read(const QJsonObject & json)
{
    bool ok = true;
    _sourceName = readSourceName(json, &ok);
    _mimetype = readStringValue(json, "mimetype", &ok);
    _url = readStringValue(json, "octopart_url", &ok);
    const QJsonValue & metadata = json["metadata"];
    if(metadata.isObject()){
        _numPages = readIntValue(metadata.toObject(), "num_pages", &ok);
    }
    else{
       qDebug("metadata object missing");
    }
    return ok;
}

PartSpec::PartSpec()
{}

bool PartSpec::read(const QJsonObject & json)
{
    bool ok = true;

    _maxValue = readNumberValue(json, "max_value");
    _minValue = readNumberValue(json, "min_value");
    const QJsonValue tmp = json.value("value");
    if(tmp.isArray()){
        const QJsonValue val = tmp.toArray().at(0);
        if(val.isDouble()){
            _value = QVariant(val.toDouble());
        }
        else if(val.isString()){
            _value = QVariant(val.toString());
        }
        else{
            qDebug()<<"PartSpec value is invalid: "<<val;
        }
    }
    ok = ok | _value.isValid();

    _displayValue = json.value("display_value").toString();
    const QJsonValue tmp1 = json.value("metadata");
    if(tmp1.isObject()){
        const QJsonObject metadata = tmp1.toObject();
        _key = readStringValue(metadata, "key", &ok);
        _datatype = readStringValue(metadata, "datatype", &ok);
        _name = readStringValue(metadata, "name", &ok);
        const QJsonValue unitTmp = metadata.value("unit");
        if(unitTmp.isObject()){
            const QJsonObject unit = unitTmp.toObject();
            _unitName = unit.value("name").toString();
            _unitSymbol = unit.value("symbol").toString();
        }
    }
    else{
        ok = false;
    }
    return ok;
}


Seller::Seller()
{}

bool Seller::read(const QJsonObject & json)
{
    bool ok = true;

    QString _name;
    QString _url;
    QString _uid;
    QString _countryCode;

    _name = readStringValue(json, "name", &ok);
    _uid = readStringValue(json, "uid", &ok);
    _url = json.value("homepage_url").toString();
    _countryCode = json.value("display_flag").toString();

    return ok;
}

Offer::Offer()
{}

bool Offer::read(const QJsonObject & json)
{
    bool ok = true;

    QVariant _moq;
    QString _packaging;
    QString _sku;
    Seller _seller;
    QVariant _moqPrice;
    QString _currency;
    QString _url;

    _moq = readNumberValue(json, "moq");
    _packaging = readStringValue(json, "packaging", &ok);
    _sku = readStringValue(json, "sku", &ok);
    _url = json.value("product_url").toString();

    const QJsonObject seller = json.value("seller").toObject();
    if(!seller.isEmpty()){
        ok = ok && _seller.read(seller);
    }

    if(json.value("prices").isObject()){
        const QJsonObject prices = json.value("prices").toObject();
        QStringList currencies = prices.keys();
        if(currencies.size() > 0){
            //XXX: For now use the first one
            _currency = currencies.at(0);
            const QJsonArray pricePairs = prices.value(_currency).toArray();
            _moqPrice = findMoqPrice(pricePairs, _moq);
        }
    }
    return ok;
}

QVariant Offer::findMoqPrice(const QJsonArray & pairs, QVariant moq) const
{
    for(int i=0; i< pairs.size(); ++i){
        const QJsonArray pair = pairs.at(i).toArray();
        int priceBreak = pair.at(0).toInt();
        if(!moq.isValid() || moq.toInt() == priceBreak){
            QString priceStr = pair.at(1).toString();
            QLocale locale("C");
            bool ok;
            double price = locale.toDouble(priceStr, &ok);
            if(ok){
                return QVariant(price);
            }
            else{
                qWarning("Failed to parse price: %s", qPrintable(priceStr));
            }
        }
    }
    return QVariant(QVariant::Double);
}

PartFull::PartFull() : PartBrief()
{
}

bool PartFull::read(const QJsonObject & json)
{
    bool ok = PartBrief::read(json);
    const QJsonValue & tmp1 = json.value("datasheets");
    if(tmp1.isArray()){
        QJsonArray datasheetsArr = tmp1.toArray();
        for (int i = 0; i < datasheetsArr.size(); ++i) {
            QJsonObject obj = datasheetsArr[i].toObject();
            Datasheet datasheet;
            bool datasheetOk = datasheet.read(obj);
            if(datasheetOk){
                _datasheets.append(datasheet);
            }
            ok = ok && datasheetOk;
        }
    }

    const QJsonValue & tmp2 = json.value("specs");
    if(tmp2.isArray()){
        QJsonArray specsArr = tmp2.toArray();
        for (int i = 0; i < specsArr.size(); ++i) {
            QJsonObject obj = specsArr[i].toObject();
            PartSpec spec;
            bool specOk = spec.read(obj);
            if(specOk){
                _specs.append(spec);
            }
            ok = ok && specOk;
        }
    }

    return ok;
}

PartsMatchResult::PartsMatchResult()
{
}

PartsMatchResult::~PartsMatchResult()
{ }

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

PartsMatchResult parsePartsMatchResult(const QByteArray &json, QJsonParseError *error)
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

PartFull parsePartObject(const QByteArray &json, QJsonParseError *error)
{
    PartFull result;
    QJsonParseError localError;
    QJsonDocument doc = QJsonDocument::fromJson(json, &localError);
    if(localError.error == QJsonParseError::NoError) {
        result.read(doc.object());
    }
    else{
        qWarning("Error parsing PartObject. %s", qPrintable(localError.errorString()));
        qWarning("Offset. %d", localError.offset);
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
{
    _manager = new QNetworkAccessManager(this);
    connect(_manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(slotReplyFinished(QNetworkReply*)));
}

QString OctopartAPI::parseClientErrorResponse(const QByteArray &json) const
{
    QJsonParseError localError;
    QJsonDocument doc = QJsonDocument::fromJson(json, &localError);
    if(localError.error == QJsonParseError::NoError) {
        return doc.object()["message"].toString();
    }
    return QString();
}

int OctopartAPI::partsMatch(const QString & mpn, int start, int limit)
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

    int id = _requestCounter.fetchAndAddRelaxed(1);
    QNetworkRequest request(url);    
    QNetworkReply *reply = _manager->get(request);
    reply->setProperty("type", PartsMatch);
    reply->setProperty("id", id);
    return id;
}

void OctopartAPI::slotReplyFinished(QNetworkReply *reply)
{
    QByteArray json = reply->readAll();
    if(reply->error()){
        QString errorMessage = parseClientErrorResponse(json);
        if(errorMessage.isEmpty()){
           errorMessage = reply->errorString();
        }
        qWarning("Request failed with error %s ", qPrintable(errorMessage));
    }
    else {
        int type = reply->property("type").toInt();
        int id = reply->property("id").toInt();
        qDebug()<<"Got reply with id "<<id;

        if(type == PartsMatch){
            QJsonParseError parseError;
            PartsMatchResult result = parsePartsMatchResult(json, &parseError);
            int count = result.count();
            int size = result.items().size();
            qDebug("Query returned %d of %d", size, count);
            emit partsMatchResultFinished(id, result);
        }

    }
    reply->deleteLater();
}

}
