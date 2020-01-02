#include "skin/converter.hpp"
#include <QtTest>

// add necessary includes here

class TestConverter : public QObject
{
    Q_OBJECT

public:
    TestConverter() = default;

private slots:
    void initTestCase()
    {
        Q_INIT_RESOURCE(resources);
        // Initialize singleton:
        MockSourceFactory::instance();
    }

    void test_factory()
    {
        auto name = ServiceName::staticMetaObject.className();
        auto object = ConverterFactory::instance().createConverterByName(name);
        QVERIFY(object);
        QVERIFY(dynamic_cast<ServiceName*>(object.get()));
    }

    void test_boolQVariant()
    {
        QCOMPARE(QVariant("true").toBool(), true);
        QCOMPARE(QVariant("false").toBool(), false);
    }

    void test_clock()
    {
        auto s = MockSourceFactory::instance().getReference("global.CurrentTime");
        QVERIFY(s);
        QCOMPARE(s->getVariant("time").toInt(), -1);

        MockSource fixedTime;
        fixedTime.setValue("time", 1544911869);

        auto clock = ConverterFactory::instance().createConverterByName("ClockToText");
        clock->attach(&fixedTime);
        clock->setArg("Format:%Y-%m-%d");
        QCOMPARE(clock->getText(), "2018-12-15");
    }

    void test_service()
    {
        auto service = MockSourceFactory::instance().getReference("session.CurrentService");
        QVERIFY(service);
        auto c = ConverterFactory::instance().createConverterByName("ServiceInfo");
        c->attach(service);

        c->setArg("VideoWidth");
        QCOMPARE(c->getValue(), 1280);
        c->setArg("VideoHeight");
        QCOMPARE(c->getValue(), 720);
    }
};

QTEST_APPLESS_MAIN(TestConverter)

#include "tst_testconverter.moc"
