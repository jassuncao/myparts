#include "testoctopartapi.h"

#include <QtTest>
#include <QtCore>
#include <QCoreApplication>
#include "octopartapi.h"

void TestOctopartApi::initTestCase()
{
    QFile file("://api_key.txt");
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        QFAIL("API test resource not available");
    }
    QTextStream textStream(&file);
    _apiKey = textStream.readLine();
}

void TestOctopartApi::testPartsMatchParsing()
{
    QFile file("://octopart_query_response.json");
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        QFAIL("JSON test resource not available");
    }
    QByteArray contents = file.readAll();
    //qDebug()<<"Contents "<<qPrintable(contents);
    QJsonParseError parseError;
    const QJsonDocument jsonDoc = QJsonDocument::fromJson(contents, &parseError);
    Q_ASSERT(QJsonParseError::NoError == parseError.error);
    Octopart::PartsQueryResult res = Octopart::parsePartsMatchResponse(jsonDoc);
    QCOMPARE(res.count(), 5);
    QCOMPARE(res.items().size(), 3);

}

void TestOctopartApi::testPartsMatchRequest()
{
    Octopart::OctopartAPI api(_apiKey);
    QSignalSpy spy(&api, SIGNAL(requestFinished(Octopart::RequestResult)));
    connect(&api, SIGNAL(requestFinished(Octopart::RequestResult)), this, SLOT(_requestFinished(Octopart::RequestResult)));

    int id = api.partsMatch("NE555D", 0, 3);
    QTest::qWait(1000);

    QCOMPARE(spy.count(), 1);
    QCOMPARE(_lastRequestResult.requestId, id);
    const Octopart::PartsQueryResult queryResult = qvariant_cast<Octopart::PartsQueryResult>(_lastRequestResult.result);
    QCOMPARE(queryResult.count(), 5);
    QCOMPARE(queryResult.items().size(), 3);
}

void TestOctopartApi::testPartGetParsing()
{
    QFile file("://octopart_part_response.json");
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        QFAIL("JSON test resource not available");
    }
    QByteArray contents = file.readAll();
    //qDebug()<<"Contents "<<qPrintable(contents);
    QJsonParseError parseError;
    const QJsonDocument jsonDoc = QJsonDocument::fromJson(contents, &parseError);
    Q_ASSERT(QJsonParseError::NoError == parseError.error);

    Octopart::PartFull res = Octopart::parsePartGetResponse(jsonDoc);
    QCOMPARE(res.brandName(), QString("Texas Instruments"));

}

void TestOctopartApi::testPartsGetRequest()
{
    Octopart::OctopartAPI api(_apiKey);
    QSignalSpy spy(&api, SIGNAL(requestFinished(const Octopart::RequestResult&)));
    connect(&api, SIGNAL(requestFinished(Octopart::RequestResult)), this, SLOT(_requestFinished(Octopart::RequestResult)));
    int id = api.partsGet("7a97b39d1223a550");
    QTest::qWait(1000);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(_lastRequestResult.requestId, id);
    const Octopart::PartFull partData = qvariant_cast<Octopart::PartFull>(_lastRequestResult.result);
    QCOMPARE(partData.uid(), QString("7a97b39d1223a550"));
    QCOMPARE(partData.offers().at(0).seller().name(), QString("Digi-Key"));
}

void TestOctopartApi::_requestFinished(const Octopart::RequestResult& result)
{
    _lastRequestResult = result;
}
