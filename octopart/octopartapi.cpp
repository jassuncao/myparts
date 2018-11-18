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
        QJsonArray sourcesArray = sources.toArray();
        if(!sourcesArray.isEmpty()){
            QJsonValue source = sourcesArray.first();
            return readStringValue(source.toObject(), "name", ok);
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

}  // namespace

namespace Octopart {

enum RequestType {
    PartsMatch,
    PartsSearch,
    PartsGet
};


PartsQueryResult OctopartAPI::parsePartsMatchResponse(const QJsonDocument &doc)
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

PartsQueryResult OctopartAPI::processPartsMatchResult(const QJsonObject &json)
{
    bool ok = true;

    const QString error = readStringValue(json, "error", &ok);
    const int count = readIntValue(json, "hits", &ok);

    PartsQueryResult result;
    if(ok){
        const QJsonArray & items = json["items"].toArray();
        QList<PartSnippet> partList;
        for (QJsonArray::const_iterator it = items.begin(); it != items.end(); ++it){
            const QJsonObject item = (*it).toObject();
            Octopart::PartSnippet part = readPartSnippet(item, &ok);
            if(ok){
                partList.append(part);
            }
        }
        result.setCount(count);
        result.setItems(partList);
    }
    else{
        result.setError(error);
    }

    return result;
}

PartsQueryResult OctopartAPI::parsePartsSearchResponse(const QJsonDocument &doc)
{
    const QJsonObject json = doc.object();
    bool ok = true;

    const int count = readIntValue(json, "hits", &ok);

    PartsQueryResult result;
    if(ok){
        const QJsonArray & items = json["results"].toArray();
        QList<PartSnippet> partList;
        for (QJsonArray::const_iterator it = items.begin(); it != items.end(); ++it){
            const QJsonObject item = (*it).toObject()["item"].toObject();
            Octopart::PartSnippet part = readPartSnippet(item, &ok);
            if(ok){
                partList.append(part);
            }
            else{
                qDebug()<<"Failed to convert Part "<<item;
            }
        }
        result.setCount(count);
        result.setItems(partList);
    }
    else{
        result.setError("Invalid response");
    }

    return result;
}


Part OctopartAPI::parsePartGetResponse(const QJsonDocument &doc)
{
    return readPart(doc.object());
}

/*
PartsQueryResult::PartsQueryResult() : _count(0)
{
}

PartsQueryResult::PartsQueryResult(const QString& error, int count, const QList<PartSnippet> items) :
    _error(error), _count(count), _items(items)
{
}

PartsQueryResult::~PartsQueryResult()
{ }
*/

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
    queryBuilder.addQueryItem("include[]", "short_description");
    queryBuilder.addQueryItem("hide[]", "offers");


    QUrl url = queryBuilder.url();
    qDebug()<<"Request URL "<<qPrintable(url.toEncoded());

    int id = _requestCounter.fetchAndAddRelaxed(1);
    QNetworkRequest request(url);
    QNetworkReply *reply = _manager->get(request);
    reply->setProperty("type", PartsSearch);
    reply->setProperty("id", id);
    _activeRequests.append(reply);
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
    _activeRequests.append(reply);
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
    _activeRequests.append(reply);
    return id;
}

void OctopartAPI::abortRequest(const int requestId)
{
    for (int i = 0; i < _activeRequests.size(); ++i) {
        QNetworkReply* reply = _activeRequests.at(i);
        if(reply->property("id").toInt() == requestId){
            reply->abort();
        }
    }
}

void OctopartAPI::abortAll()
{
    for (int i = 0; i < _activeRequests.size(); ++i) {
        QNetworkReply* reply = _activeRequests.at(i);
        reply->abort();
    }
}

void OctopartAPI::slotReplyFinished(QNetworkReply *reply)
{
    QString errorMsg;
    _activeRequests.removeOne(reply);
    const int requestId = reply->property("id").toInt();
    qDebug()<<"Processing reply for requestId "<<requestId;

    QByteArray jsonData = reply->readAll();
    if(reply->error()){
        qDebug("Response error %s", qPrintable(reply->errorString()));
        if(reply->error() > 400){
            errorMsg = parseClientErrorResponse(jsonData);
        }
        //Use the HTTP error message if the json error msg is unvailable
        if(errorMsg.isEmpty()){
           errorMsg = reply->errorString();
        }
        qWarning("Request failed with error %s", qPrintable(errorMsg));
        emit requestError(ErrorResponse(requestId,errorMsg));
    }
    else {
        QJsonParseError parseError;
        const QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData, &parseError);
        if(jsonDoc.isEmpty()){
            qWarning("Error parsing json response. %s", qPrintable(parseError.errorString()));
            qWarning("Offset. %d", parseError.offset);
            errorMsg = parseError.errorString();
            emit requestError(ErrorResponse(requestId,errorMsg));
        }
        else {
            const int type = reply->property("type").toInt();
            switch (type) {
            case PartsMatch:
            {
                const PartsQueryResult queryResult = parsePartsMatchResponse(jsonDoc);                
                const PartsQueryResponse response(requestId, queryResult);
                emit requestFinished(response);
                break;
            }
            case PartsSearch:
            {
                const PartsQueryResult queryResult = parsePartsSearchResponse(jsonDoc);
                const PartsQueryResponse response(requestId, queryResult);
                emit requestFinished(response);
                break;
            }
            case PartsGet:
            {
                const Part part = parsePartGetResponse(jsonDoc);
                const PartFullResponse response(requestId, part);
                emit requestFinished(response);
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
}

Datasheet OctopartAPI::readDatasheet(const QJsonObject & json, bool * okOut)
{
    qDebug("Reading Datasheet object");
    bool ok = true;
    const QString sourceName = readSourceName(json, &ok);
    const QString mimetype = readStringValue(json, "mimetype", &ok);
    const QString url = readStringValue(json, "url", &ok);
    const QJsonValue & metadata = json["metadata"];
    int numPages = 0;
    if(metadata.isObject()){
        numPages = readIntValue(metadata.toObject(), "num_pages", &ok);
    }
    else if(metadata.isUndefined()) {
       qDebug("metadata object missing");
    }
    if(okOut){
        *okOut = *okOut | ok;
    }
    Datasheet datasheet;
    datasheet.setUrl(url);
    datasheet.setMimetype(mimetype);
    datasheet.setSourceName(sourceName);
    datasheet.setNumPages(numPages);
    return datasheet;
}


PartSpec OctopartAPI::readPartSpec(const QJsonObject & json, bool * okOut)
{
    qDebug("Reading PartSpec object");
    bool ok = true;
    PartSpec partSpec;

    partSpec.setMaxValue(readNumberValue(json, "max_value"));
    partSpec.setMinValue(readNumberValue(json, "min_value"));
    const QJsonValue tmp = json.value("value");

    QVariant value;
    if(tmp.isArray()){
        const QJsonValue val = tmp.toArray().at(0);
        if(val.isDouble()){
            value = QVariant(val.toDouble());
        }
        else if(val.isString()){
            value = QVariant(val.toString());
        }
        else{
            qDebug()<<"PartSpec value is invalid: "<<val;
        }
    }
    ok = ok | value.isValid();
    partSpec.setValue(value);

    partSpec.setDisplayValue(json.value("display_value").toString());
    const QJsonValue tmp1 = json.value("metadata");
    if(tmp1.isObject()){
        const QJsonObject metadata = tmp1.toObject();
        partSpec.setKey(readStringValue(metadata, "key", &ok));
        partSpec.setDatatype(readStringValue(metadata, "datatype", &ok));
        partSpec.setName(readStringValue(metadata, "name", &ok));
        const QJsonValue unitTmp = metadata.value("unit");
        if(unitTmp.isObject()){
            const QJsonObject unit = unitTmp.toObject();
            partSpec.setUnitName(unit.value("name").toString());
            partSpec.setUnitSymbol(unit.value("symbol").toString());
        }
    }
    else{
        ok = false;
    }
    if(okOut){
        *okOut = *okOut | ok;
    }


    return partSpec;
}

Part OctopartAPI::readPart(const QJsonObject & json, bool * ok)
{
    bool aux;
    Part part;
    part.setUid(readStringValue(json, "uid", &aux));
    part.setMpn(readStringValue(json, "mpn", &aux));
    part.setUrl(readStringValue(json, "octopart_url", &aux));
    part.setDescription(readStringValue(json, "short_description", &aux));
    const QJsonValue & brand = json["brand"];
    if(brand.isObject()){
        part.setBrandName(readStringValue(brand.toObject(), "name", &aux));
    }
    else{
       qWarning("Brand object missing");
       aux = false;
    }

    const QJsonValue & tmp1 = json.value("datasheets");
    if(tmp1.isArray()){
        QList<Datasheet> datasheets;
        QJsonArray datasheetsArr = tmp1.toArray();
        for (int i = 0; i < datasheetsArr.size(); ++i) {
            QJsonObject obj = datasheetsArr[i].toObject();
            bool datasheetOk;
            Datasheet datasheet = readDatasheet(obj, &datasheetOk);
            if(datasheetOk){
                datasheets.append(datasheet);
            }
            aux = aux && datasheetOk;
        }
        part.setDatasheets(datasheets);
    }

    const QJsonValue & tmp2 = json.value("specs");
    if(tmp2.isObject()){
        QJsonObject specsMap = tmp2.toObject();
        QList<PartSpec> specsList;
        const QStringList keys = specsMap.keys();
        for (int i = 0; i < keys.count(); ++i) {
            const QString key = keys.at(i);
            QJsonObject obj = specsMap.value(key).toObject();
            bool specOk;
            PartSpec spec = readPartSpec(obj, &specOk);
            if(specOk){
                specsList.append(spec);
            }
            aux = aux && specOk;
        }
        part.setSpecs(specsList);
    }
    else{
        qDebug()<<"No Specs";
    }

    const QJsonValue & tmp3 = json.value("offers");
    if(tmp3.isArray()){
        QJsonArray offersArr = tmp3.toArray();
        QList<Offer> offers;
        for (int i = 0; i < offersArr.size(); ++i) {
            bool offerOk;
            QJsonObject obj = offersArr[i].toObject();
            Offer offer = readOffer(obj, &offerOk);
            if(offerOk){
                offers.append(offer);
            }
            aux = aux && offerOk;
        }
        part.setOffers(offers);
    }
    if(ok){
        *ok = aux;
    }
    return part;
}

PartSnippet OctopartAPI::readPartSnippet(const QJsonObject & json, bool * okOut)
{
    bool ok = true;
    PartSnippet partSnippet;
    partSnippet.setUid(readStringValue(json, "uid", &ok));
    partSnippet.setMpn(readStringValue(json, "mpn", &ok));
    partSnippet.setUrl(readStringValue(json, "octopart_url", &ok));
    partSnippet.setDescription(readStringValue(json, "short_description", &ok));
    const QJsonValue & brand = json["brand"];
    if(brand.isObject()){
        partSnippet.setBrandName(readStringValue(brand.toObject(), "name", &ok));
    }
    else{
       qWarning("Brand object missing");
       ok = false;
    }
    if(okOut){
        *okOut = ok;
    }
    return partSnippet;
}

Offer OctopartAPI::readOffer(const QJsonObject & json, bool * okOut)
{
    qDebug("Reading Offer object");
    bool ok = true;

    Offer offer;
    const QVariant moq = readNumberValue(json, "moq");
    offer.setMoq(moq);
    offer.setPackaging(readStringValue(json, "packaging", &ok));
    offer.setSku(readStringValue(json, "sku", &ok));
    offer.setUrl(json.value("product_url").toString());

    const QJsonObject sellerObj = json.value("seller").toObject();
    if(!sellerObj.isEmpty()){
        const Seller seller = readSeller(sellerObj, &ok);
        offer.setSeller(seller);
    }

    if(json.value("prices").isObject()){
        const QJsonObject prices = json.value("prices").toObject();
        QStringList currencies = prices.keys();
        if(!currencies.isEmpty()){
            //XXX: For now use the first one
            const QString currency = currencies.at(0);
            const QJsonArray pricePairs = prices.value(currency).toArray();
            const QVariant moqPrice = findMoqPrice(pricePairs, moq);
            offer.setCurrency(currency);
            offer.setMoqPrice(moqPrice);
        }
    }
    if(okOut){
        *okOut = *okOut | ok;
    }

    return offer;
}

QVariant OctopartAPI::findMoqPrice(const QJsonArray & pairs, QVariant moq)
{
    for(int i=0; i< pairs.size(); ++i){
        const QJsonArray pair = pairs.at(i).toArray();
        int priceBreak = pair.at(0).toInt();
        if(!moq.isValid() || moq.toInt() == priceBreak){
            QString priceStr = pair.at(1).toString();
            QLocale locale("C");
            bool ok = true;
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


Seller OctopartAPI::readSeller(const QJsonObject & json, bool * okOut)
{
    qDebug("Reading Seller object");
    bool ok = true;   
    Seller seller;
    seller.setName(readStringValue(json, "name", &ok));
    seller.setUid(readStringValue(json, "uid", &ok));
    seller.setUrl(json.value("homepage_url").toString());
    seller.setCountryCode(json.value("display_flag").toString());

    if(okOut){
        *okOut = *okOut | ok;
    }
    return seller;
}

}
