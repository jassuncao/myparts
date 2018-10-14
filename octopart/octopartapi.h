#ifndef OCTOPARTAPI_H
#define OCTOPARTAPI_H

#include <QObject>
#include <QList>
#include <QtNetwork/QNetworkReply>
#include <QAtomicInt>
#include "qjsonobject.h"
#include "qjsondocument.h"
#include "part.h"
#include "partsnippet.h"
#include "partsqueryresult.h"

class QNetworkAccessManager;
class QNetworkReply;

namespace Octopart {

template<typename T>
struct Response {
    int requestId;
    T result;
    Response() :
        requestId(0), result()
    {}
    Response(int requestId, const T & result) :
        requestId(requestId), result(result)
    {}
};

typedef struct Response<PartsQueryResult> PartsQueryResponse;
typedef struct Response<Part> PartFullResponse;
typedef struct Response<QString> ErrorResponse;

class OctopartAPI : public QObject
{
    Q_OBJECT
public:
    OctopartAPI(const QString &apiKey, QObject * parent = 0);
    int partsSearch(const QString & text, int start=0, int limit=10);
    int partsMatch(const QString & mpn, int start=0, int limit=10);
    int partsGet(const QString & partUid);
    static PartsQueryResult parsePartsMatchResponse(const QJsonDocument &doc);
    static PartsQueryResult parsePartsSearchResponse(const QJsonDocument &doc);
    static Part parsePartGetResponse(const QJsonDocument &doc);

signals:    
    void requestFinished(const Octopart::PartsQueryResponse& response);
    void requestFinished(const Octopart::PartFullResponse& response);
    void requestError(const Octopart::ErrorResponse& response);
private slots:
    void slotReplyFinished(QNetworkReply *reply);
private:
    void handlePartsMatchResponse(const QByteArray &json);    
    static PartsQueryResult processPartsMatchResult(const QJsonObject &json);
    static Datasheet readDatasheet(const QJsonObject & json, bool * okOut = 0);
    static PartSpec readPartSpec(const QJsonObject & json, bool * okOut = 0);
    static Part readPart(const QJsonObject & json, bool * ok = 0);
    static PartSnippet readPartSnippet(const QJsonObject & json, bool * okOut = 0);
    static Offer readOffer(const QJsonObject & json, bool * ok = 0);
    static QVariant findMoqPrice(const QJsonArray & pairs, QVariant moq);
    static Seller readSeller(const QJsonObject & json, bool * okOut=0);

    QString _apiUrl;
    QString _apiKey;
    QNetworkAccessManager * _manager;
    QAtomicInt _requestCounter;
};

}

#endif // OCTOPARTAPI_H
