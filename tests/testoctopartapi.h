#ifndef TESTOCTOPARTAPI_H
#define TESTOCTOPARTAPI_H

#include "octopartapi.h"
#include <QObject>

class TestOctopartApi : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void testPartsMatchParsing();
    void testPartsMatchRequest();
    void testPartParsing();
    void testPartsGetRequest();
    void _partsMatchResultFinished(int id, Octopart::PartsMatchResult res);
    void _partsGetFinished(int id, Octopart::PartFull res);
private:
    QString _apiKey;
    Octopart::PartsMatchResult _lastPartsMatchResult;
    Octopart::PartFull _lastPart;
};

#endif // TESTOCTOPARTAPI_H
