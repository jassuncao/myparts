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
    QJsonParseError error;
    Octopart::PartsMatchResult res = Octopart::parsePartsMatchResult(contents, &error);
    QCOMPARE(error.error, QJsonParseError::NoError);
    QCOMPARE(res.count(), 5);
    QCOMPARE(res.items().size(), 3);

}

void TestOctopartApi::testPartsMatchRequest()
{
    Octopart::OctopartAPI api(_apiKey);
    QSignalSpy spy(&api, SIGNAL(partsMatchResultFinished(int,Octopart::PartsMatchResult)));
    connect(&api, SIGNAL(partsMatchResultFinished(int,Octopart::PartsMatchResult)), this, SLOT(_partsMatchResultFinished(int,Octopart::PartsMatchResult)));
    int id = api.partsMatch("NE555D", 0, 3);
    QTest::qWait(1000);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(_lastPartsMatchResult.count(), 5);
    QCOMPARE(_lastPartsMatchResult.items().size(), 3);
}

void TestOctopartApi::testPartParsing()
{
    QFile file("://octopart_part_response.json");
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        QFAIL("JSON test resource not available");
    }
    QByteArray contents = file.readAll();
    //qDebug()<<"Contents "<<qPrintable(contents);
    QJsonParseError error;
    Octopart::PartFull res = Octopart::parsePartObject(contents, &error);
    QCOMPARE(error.error, QJsonParseError::NoError);
    QCOMPARE(res.brandName(), QString("Texas Instruments"));

}

void TestOctopartApi::testPartsGetRequest()
{
    Octopart::OctopartAPI api(_apiKey);
    QSignalSpy spy(&api, SIGNAL(partsGetFinished(int,Octopart::PartFull)));
    connect(&api, SIGNAL(partsGetFinished(int,Octopart::PartFull)), this, SLOT(_partsGetFinished(int,Octopart::PartFull)));
    int id = api.partsGet("7a97b39d1223a550");
    QTest::qWait(1000);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(_lastPart.uid(), QString("7a97b39d1223a550"));
    QCOMPARE(_lastPart.offers().at(0).seller().name(), QString("Digi-Key"));
}


void TestOctopartApi::_partsMatchResultFinished(int id, Octopart::PartsMatchResult res)
{
    _lastPartsMatchResult = res;
}

void TestOctopartApi::_partsGetFinished(int id, Octopart::PartFull res)
{
    _lastPart = res;
}

