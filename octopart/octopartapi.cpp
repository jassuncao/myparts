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

    void addQueryItem(const QString &key, const int value)
    {
        addQueryItem(key, QString::number(value));
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

    void addQueryItem(const QString &key, const int value)
    {
        addQueryItem(key, QString::number(value));
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
    PartsMatch,
    PartsSearch,
    PartsGet
};

namespace {

 QString readStringValue(const QJsonObject & json, const QString & key, bool * ok){
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

int readIntValue(const QJsonObject & json, const QString & key, bool * ok){
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

QVariant readNumberValue(const QJsonObject & json, const QString & key)
{
    const QJsonValue & jsonValue = json.value(key);
    if(jsonValue.isDouble()){
        return QVariant(jsonValue.toDouble());
    }
    return QVariant(QVariant::Double);
}

///Accepts a Datasheet or SpecValue and returns the first source name
QString readSourceName(const QJsonObject & json, bool * ok){
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


QString parseClientErrorResponse(const QByteArray &json)
{
    QJsonParseError localError;
    QJsonDocument doc = QJsonDocument::fromJson(json, &localError);
    if(doc.isEmpty()){
        return localError.errorString();
    }
    return doc.object()["message"].toString();
}

PartsQueryResult processPartsMatchResult(const QJsonObject &json);



PartsQueryResult processPartsMatchResult(const QJsonObject &json)
{
    bool ok = true;

    const QString error = readStringValue(json, "error", &ok);
    const int count = readIntValue(json, "hits", &ok);

    if(!ok){
        return PartsQueryResult(error);
    }

    const QJsonArray & items = json["items"].toArray();
    QList<PartBrief> partList;
    for (QJsonArray::const_iterator it = items.begin(); it != items.end(); ++it){
        const QJsonObject item = (*it).toObject();
        PartBrief part;
        part.read(item);
        partList.append(part);
    }
    return PartsQueryResult(error, count, partList);
}

}  // namespace

PartsQueryResult parsePartsMatchResponse(const QJsonDocument &doc)
{
    const QJsonObject obj = doc.object();
    const QJsonArray results =  obj["results"].toArray();
    if(results.isEmpty()){
        qWarning("Query returned no results");
        return PartsQueryResult();
    }

    const QJsonObject partsMatchResultJson = results.at(0).toObject();
    return processPartsMatchResult(partsMatchResultJson);
}

PartFull parsePartGetResponse(const QJsonDocument &doc)
{

    PartFull result;
    result.read(doc.object());
    return result;
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
    qDebug("Reading Datasheet object");
    bool ok = true;
    _sourceName = readSourceName(json, &ok);
    _mimetype = readStringValue(json, "mimetype", &ok);
    _url = readStringValue(json, "url", &ok);
    const QJsonValue & metadata = json["metadata"];
    if(metadata.isObject()){
        _numPages = readIntValue(metadata.toObject(), "num_pages", &ok);
    }
    else if(metadata.isUndefined()) {
       qDebug("metadata object missing");
    }
    return ok;
}

PartSpec::PartSpec()
{}

bool PartSpec::read(const QJsonObject & json)
{
    qDebug("Reading PartSpec object");
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
    qDebug("Reading Seller object");
    bool ok = true;  

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
    qDebug("Reading Offer object");
    bool ok = true;

    _moq = readNumberValue(json, "moq");
    _packaging = readStringValue(json, "packaging", &ok);
    _sku = readStringValue(json, "sku", &ok);
    _url = json.value("product_url").toString();

    const QJsonObject sellerObj = json.value("seller").toObject();
    if(!sellerObj.isEmpty()){
        ok = ok && _seller.read(sellerObj);
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
    if(tmp2.isObject()){
        QJsonObject specsMap = tmp2.toObject();

        const QStringList keys = specsMap.keys();
        for (int i = 0; i < keys.count(); ++i) {
            const QString key = keys.at(i);
            QJsonObject obj = specsMap.value(key).toObject();
            PartSpec spec;
            bool specOk = spec.read(obj);
            if(specOk){
                _specs.append(spec);
            }
            ok = ok && specOk;
        }
    }
    else{
        qDebug()<<"No Specs";
    }

    const QJsonValue & tmp3 = json.value("offers");
    if(tmp3.isArray()){
        QJsonArray offersArr = tmp3.toArray();
        for (int i = 0; i < offersArr.size(); ++i) {
            QJsonObject obj = offersArr[i].toObject();
            Offer offer;
            bool offerOk = offer.read(obj);
            if(offerOk){
                _offers.append(offer);
            }
            ok = ok && offerOk;
        }
    }

    return ok;
}

PartsQueryResult::PartsQueryResult() : _count(0)
{
}

PartsQueryResult::PartsQueryResult(const QString& error, int count, const QList<PartBrief> items) :
    _error(error), _count(count), _items(items)
{
}

PartsQueryResult::~PartsQueryResult()
{ }

RequestResult::RequestResult() : requestId(0), result(QVariant()), errorMessage(QString())
{}

RequestResult::RequestResult(int requestId, const QVariant & result, const QString& errorMsg) :
    requestId(requestId), result(result), errorMessage(errorMsg)
{}

OctopartAPI::OctopartAPI(const QString & apiKey, QObject * parent) :
    QObject(parent),
    _apiUrl("https://octopart.com/api/v3/"),
    _apiKey(apiKey)
{
    _manager = new QNetworkAccessManager(this);
    connect(_manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(slotReplyFinished(QNetworkReply*)));
}

int OctopartAPI::partsSearch(const QString & text, int start, int limit)
{
    UrlQueryBuilder queryBuilder(_apiUrl+"parts/search");
    queryBuilder.addQueryItem("apikey", _apiKey);
    queryBuilder.addQueryItem("q", text);
    queryBuilder.addQueryItem("start", start);
    queryBuilder.addQueryItem("limit", limit);
    /*
    queryBuilder.addQueryItem("include[]", "short_description");
    queryBuilder.addQueryItem("show[]", "mpn");
    queryBuilder.addQueryItem("show[]", "brand.name");
    queryBuilder.addQueryItem("show[]", "octopart_url");
    queryBuilder.addQueryItem("show[]", "short_description");
    queryBuilder.addQueryItem("show[]", "uid");
    */

    QUrl url = queryBuilder.url();
    qDebug()<<"Request URL "<<qPrintable(url.toEncoded());

    int id = _requestCounter.fetchAndAddRelaxed(1);
    QNetworkRequest request(url);
    QNetworkReply *reply = _manager->get(request);
    reply->setProperty("type", PartsSearch);
    reply->setProperty("id", id);
    return id;
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

int OctopartAPI::partsGet(const QString & partUid)
{
    UrlQueryBuilder queryBuilder(_apiUrl+"parts/"+partUid);
    queryBuilder.addQueryItem("apikey", _apiKey);

    queryBuilder.addQueryItem("include[]", "short_description");
    queryBuilder.addQueryItem("include[]", "datasheets");
    queryBuilder.addQueryItem("include[]", "specs");

    QUrl url = queryBuilder.url();
    qDebug()<<"Request URL "<<qPrintable(url.toEncoded());

    int id = _requestCounter.fetchAndAddRelaxed(1);
    QNetworkRequest request(url);
    QNetworkReply *reply = _manager->get(request);
    reply->setProperty("type", PartsGet);
    reply->setProperty("id", id);
    return id;
}

void OctopartAPI::slotReplyFinished(QNetworkReply *reply)
{
    QString errorMsg;
    QVariant result;

    const int requestId = reply->property("id").toInt();
    qDebug()<<"Processing reply for requestId "<<requestId;

    QByteArray jsonData = reply->readAll();
    if(reply->error()){
        qDebug("Response error %s", qPrintable(reply->errorString()));

        errorMsg = parseClientErrorResponse(jsonData);
        //Use the HTTP error message if the json error msg is unvailable
        if(errorMsg.isEmpty()){
           errorMsg = reply->errorString();
        }
        qWarning("Request failed with error %s", qPrintable(errorMsg));
    }
    else {
        QJsonParseError parseError;
        const QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData, &parseError);
        if(jsonDoc.isEmpty()){
            qWarning("Error parsing json response. %s", qPrintable(parseError.errorString()));
            qWarning("Offset. %d", parseError.offset);
            errorMsg = parseError.errorString();
        }
        else {
            const int type = reply->property("type").toInt();
            switch (type) {
            case PartsMatch:
            {
                const PartsQueryResult queryResult = parsePartsMatchResponse(jsonDoc);
                result = QVariant::fromValue(queryResult);
                break;
            }
            case PartsGet:
            {
                const PartFull partFull = parsePartGetResponse(jsonDoc);
                result = QVariant::fromValue(partFull);
                break;
            }
            default:
                qWarning("Invalid reply type: %d", type);
                errorMsg = QString("Invalid reply");
                break;
            }
        }
    }
    reply->deleteLater();
    const RequestResult requestResult(requestId, result, errorMsg);
    emit requestFinished(requestResult);
}

}
