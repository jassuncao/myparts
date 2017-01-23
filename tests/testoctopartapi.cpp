#include "testoctopartapi.h"

#include <QtTest>
#include <QtCore>
#include "octopartapi.h"

void TestOctopartApi::testPartsMatch()
{
    //Octopart::OctopartAPI api("12621337");
    //api.partsMatch("NE555D");

    QFile file("://octopart_query_response.json");
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        QFAIL("JSON test resource not available");
    }
    QByteArray contents = file.readAll();
    //qDebug()<<"Contents "<<qPrintable(contents);
    QJsonParseError error;
    Octopart::PartsMatchResponseParser parser;
    Octopart::PartsMatchResult res = parser.parse(contents, &error);
    QCOMPARE(error.error, QJsonParseError::NoError);
    QCOMPARE(res.count(), 5);
    QCOMPARE(res.items().size(), 3);

}
