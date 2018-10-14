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
    void testPartsSearchParsing();
    void testPartsMatchRequest();
    void testPartGetParsing();
    void testPartsGetRequest();   
    void _requestFinished(const Octopart::PartsQueryResponse& response);
    void _requestFinished(const Octopart::PartFullResponse& response);
private:
    QString _apiKey;
    Octopart::PartsQueryResponse _lastQueryResponse;
    Octopart::PartFullResponse _lastPartResponse;
};

#endif // TESTOCTOPARTAPI_H
