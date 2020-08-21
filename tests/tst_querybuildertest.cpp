#include <QtTest>

// add necessary includes here

class BitBuilder {
public:
    long raw = 0;

    BitBuilder& append(int value, int bitLen) {
        int mask = -1;
        mask = mask << bitLen;
        mask = ~mask;
        raw = raw << bitLen;
        raw = raw | (value & mask);
        return *this;
    }

    long computeCRC4(long cod) {
        unsigned char chk = 0;
        for(int i=4; i<= 63; i++) {
            chk |= (unsigned char) (( cod >> i) % 2) << (i % 4);
        }
        chk &= 0x0F;
        qDebug() << QString::number(chk);
        return cod | chk;
    }
};

class QueryBuilderTest : public QObject
{
    Q_OBJECT

public:
    QueryBuilderTest();
    ~QueryBuilderTest();

private slots:
    void test_case1();

};

QueryBuilderTest::QueryBuilderTest()
{

}

QueryBuilderTest::~QueryBuilderTest()
{

}

void QueryBuilderTest::test_case1()
{
    BitBuilder bitBuilder;
    bitBuilder
        .append(22, 6) // sec
        .append(56, 6) // min
        .append(18, 5) // hour
        .append(14, 5) // day
        .append(10, 4) // month
        .append(18, 7) // year
        .append(16, 12) //cfmaster
        .append(12331, 15) //transit
        .append(0, 4);

    qDebug() << QString::number(bitBuilder.raw);
    long cod = bitBuilder.computeCRC4(bitBuilder.raw);
    qDebug() << QString::number(cod);
}

QTEST_APPLESS_MAIN(QueryBuilderTest)

#include "tst_querybuildertest.moc"
