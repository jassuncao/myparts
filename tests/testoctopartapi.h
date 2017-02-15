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
    void _partsMatchResultFinished(int id, Octopart::PartsMatchResult res);
private:
    QString _apiKey;
    Octopart::PartsMatchResult _lastPartsMatchResult;
};

#endif // TESTOCTOPARTAPI_H
