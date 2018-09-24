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
    void testPartGetParsing();
    void testPartsGetRequest();   
    void _requestFinished(const Octopart::RequestResult& result);
private:
    QString _apiKey;
    Octopart::RequestResult _lastRequestResult;
};

#endif // TESTOCTOPARTAPI_H
