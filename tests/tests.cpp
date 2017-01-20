#include <QtTest> 
#include <QtCore>
#include "smdresistorcalculator.h"
#include <QJsonObject>

class TestSmdResistorCalculator: public QObject {
   Q_OBJECT
private slots:   
  void testParse3DigitCodes();
  void testParse4DigitCodes();
  void testParseEIA96Codes();
};

void TestSmdResistorCalculator::testParse3DigitCodes()
{
    SmdResistorCalculator calc;
    bool ok;
    double value;


    value = calc.parse("4R7", &ok);
    QVERIFY(ok);
    QVERIFY(qFuzzyCompare(value, 4.7));

    value = calc.parse("R47", &ok);
    QVERIFY(ok);
    QVERIFY(qFuzzyCompare(value, 0.47));

    value = calc.parse("470", &ok);
    QVERIFY(ok);
    QVERIFY(qFuzzyCompare(value, 47.0));

    value = calc.parse("471", &ok);
    QVERIFY(ok);
    QVERIFY(qFuzzyCompare(value, 470));
}

void TestSmdResistorCalculator::testParse4DigitCodes()
{
    SmdResistorCalculator calc;
    bool ok;
    double value;


    value = calc.parse("0R11", &ok);
    QVERIFY(ok);
    QVERIFY(qFuzzyCompare(value, 0.11));

    value = calc.parse("1R10", &ok);
    QVERIFY(ok);
    QVERIFY(qFuzzyCompare(value, 1.1));

    value = calc.parse("11R0", &ok);
    QVERIFY(ok);
    QVERIFY(qFuzzyCompare(value, 11.0));

    value = calc.parse("1100", &ok);
    QVERIFY(ok);
    QVERIFY(qFuzzyCompare(value, 110));

    value = calc.parse("1151", &ok);
    QVERIFY(ok);
    QVERIFY(qFuzzyCompare(value, 1150));
}

void TestSmdResistorCalculator::testParseEIA96Codes()
{
    SmdResistorCalculator calc;
    bool ok;
    double value;

    value = calc.parse("12Z", &ok);
    QVERIFY(ok);
    QVERIFY(qFuzzyCompare(value, 0.13));

    value = calc.parse("12R", &ok);
    QVERIFY(ok);
    QVERIFY(qFuzzyCompare(value, 1.3));

    value = calc.parse("12S", &ok);
    QVERIFY(ok);
    QVERIFY(qFuzzyCompare(value, 13));

    value = calc.parse("01Y", &ok);
    QVERIFY(ok);
    QVERIFY(qFuzzyCompare(value, 1));

    value = calc.parse("68X", &ok);
    QVERIFY(ok);
    QVERIFY(qFuzzyCompare(value, 49.9));

    value = calc.parse("76X", &ok);
    QVERIFY(ok);
    QVERIFY(qFuzzyCompare(value, 60.4));

    value = calc.parse("01A", &ok);
    QVERIFY(ok);
    QVERIFY(qFuzzyCompare(value, 100));

    value = calc.parse("29B", &ok);
    QVERIFY(ok);
    QVERIFY(qFuzzyCompare(value, 1960));

    value = calc.parse("01C", &ok);
    QVERIFY(ok);
    QVERIFY(qFuzzyCompare(value, 10000));
}


QTEST_MAIN(TestSmdResistorCalculator)
#include "tests.moc"
