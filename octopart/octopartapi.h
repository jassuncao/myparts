#ifndef OCTOPARTAPI_H
#define OCTOPARTAPI_H

#include <QObject>
#include <QList>
#include <QtNetwork/QNetworkReply>
#include <QAtomicInt>
#include "qjsonobject.h"
#include "qjsondocument.h"

class QNetworkAccessManager;
class QNetworkReply;

namespace Octopart {

class PartBrief {
public:
    PartBrief();
    bool read(const QJsonObject & json);
    QString uid() const { return _uid;}
    QString mpn() const { return _mpn;}
    QString url() const { return _url;}
    QString description() const { return _description;}
    QString brandName() const { return _brandName;}
private:
    QString _uid;
    QString _mpn;
    QString _url;
    QString _description;
    QString _brandName;
};

class Datasheet {
public:
    Datasheet();
    bool read(const QJsonObject & json);
    QString sourceName() const { return _sourceName;}
    QString mimetype() const { return _mimetype;}
    QString url() const { return _url;}
    int numPages() const { return _numPages;}
private:
    QString _sourceName;
    QString _mimetype;
    QString _url;
    int _numPages;
};

class PartSpec {
public:
    PartSpec();
    virtual bool read(const QJsonObject & json);
    QVariant maxValue() const { return _maxValue;}
    QVariant minValue() const { return _minValue;}
    QVariant value() const { return _value;}
    QString displayValue() const { return _displayValue;}
    QString datatype() const { return _datatype;}
    QString key() const { return _key;}
    QString name() const { return _name;}
    QString unitName() const { return _unitName;}
    QString unitSymbol() const { return _unitSymbol;}
private:
    QVariant _maxValue;
    QVariant _minValue;
    QVariant _value;
    QString _displayValue;
    QString _datatype;
    QString _key;
    QString _name;
    QString _unitName;
    QString _unitSymbol;
};

class Seller {
public:
    Seller();
    bool read(const QJsonObject & json);
    QString name() const {return _name;}
    QString url() const {return _url;}
    QString uid() const {return _uid;}
    QString countryCode() const {return _countryCode;}
private:
    QString _name;
    QString _url;
    QString _uid;
    QString _countryCode;
};

class Offer {
public:
    Offer();
    bool read(const QJsonObject & json);
    QVariant moq() const {return _moq;}
    QString packaging() const {return _packaging;}
    QString sku() const {return _sku;}
    Seller seller() const {return _seller;}
    QVariant moqPrice() const {return _moqPrice;}
    QString currency() const {return _currency;}
    QString url() const {return _url;}

private:
    QVariant findMoqPrice(const QJsonArray & pairs, QVariant moq) const;
    QVariant _moq;
    QString _packaging;
    QString _sku;
    Seller _seller;
    QVariant _moqPrice;
    QString _currency;
    QString _url;
};

class PartFull : public PartBrief{
public:
    PartFull();
    virtual bool read(const QJsonObject & json);
    QList<Datasheet> datasheets() const { return _datasheets;}
    QList<PartSpec> specs() const { return _specs;}
    QList<Offer> offers() const { return _offers;}
private:
    QList<Datasheet> _datasheets;
    QList<PartSpec> _specs;
    QList<Offer> _offers;
};

class PartsQueryResult {
public:
    explicit PartsQueryResult();
    explicit PartsQueryResult(const QString& error, int count=0, const QList<PartBrief> items = QList<PartBrief>());
    virtual ~PartsQueryResult();
    inline int count() const { return _count;}
    inline QList<PartBrief> items() const { return _items;}
    inline QString error() const { return _error;}
private:   
    QString _error;
    int _count;
    QList<PartBrief> _items;
};

struct RequestResult {
    int requestId;
    QVariant result;
    QString errorMessage;
    RequestResult();
    RequestResult(int requestId, const QVariant & result, const QString& errorMsg = QString());
};

class OctopartAPI : public QObject
{
    Q_OBJECT
public:
    OctopartAPI(const QString &apiKey, QObject * parent = 0);
    int partsSearch(const QString & text, int start=0, int limit=10);
    int partsMatch(const QString & mpn, int start=0, int limit=10);
    int partsGet(const QString & partUid);
signals:
    void partsMatchResultFinished(int id, Octopart::PartsQueryResult result);
    void partsGetFinished(int id, Octopart::PartFull result);
    void requestFinished(const Octopart::RequestResult& result);
private slots:
    void slotReplyFinished(QNetworkReply *reply);
private:
    void handlePartsMatchResponse(const QByteArray &json);
    QString _apiUrl;
    QString _apiKey;
    QNetworkAccessManager * _manager;
    QAtomicInt _requestCounter;
};

PartsQueryResult parsePartsMatchResponse(const QJsonDocument &doc);
PartFull parsePartGetResponse(const QJsonDocument &doc);

}

Q_DECLARE_METATYPE(Octopart::PartsQueryResult)
Q_DECLARE_METATYPE(Octopart::RequestResult)
Q_DECLARE_METATYPE(Octopart::PartFull)

#endif // OCTOPARTAPI_H
