#ifndef TESTOCTOPARTAPI_H
#define TESTOCTOPARTAPI_H

#include <QObject>

class TestOctopartApi : public QObject
{
    Q_OBJECT
private slots:
    void testPartsMatch();
};

#endif // TESTOCTOPARTAPI_H
