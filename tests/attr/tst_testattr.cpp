#include <QtTest>

#include "skin/sizeattr.hpp"
#include "skin/positionattr.hpp"
#include "skin/colorattr.hpp"
#include "skin/attributes.hpp"
#include "skin/enumattr.hpp"

class TestAttr : public QObject
{
    Q_OBJECT
public:
    TestAttr();

private slots:
    void test_size() {
        SizeAttr sz;
        QVERIFY(sz.isRelative() == false);

        sz = SizeAttr("100,200");
        QVERIFY(sz.w().value() == 100 && sz.h().value() == 200);
        QVERIFY(serialize(sz) == "100,200");
    }

    void test_position() {
        PositionAttr pos("50,70");
        QVERIFY(pos.x().value() == 50 && pos.y().value() == 70);
        QVERIFY(serialize(pos) == "50,70");
    }

    void test_bool() {
        bool b;

        deserialize(QString(), b);
        QVERIFY(b == false);
        QVERIFY(serialize(b).isNull());

        deserialize("1", b);
        QVERIFY(b == true);
        QVERIFY(serialize(b) == "1");
    }

    void test_int() {
        int a;
        deserialize(QString(), a);
        QVERIFY(a == 0);
        QVERIFY(serialize(a).isNull());

        deserialize("-40", a);
        QVERIFY(a == -40);
        QVERIFY(serialize(a) == "-40");
    }

    void test_enum() {
        using Alphatest = Property::Alphatest;
        Alphatest a;
        deserialize("on", a);
        QVERIFY(a == Alphatest::on);
        QVERIFY(serialize(a) == "on");

        deserialize(QString(), a);
        QVERIFY(a == Alphatest::off);
    }
};

TestAttr::TestAttr()
{

}

QTEST_APPLESS_MAIN(TestAttr)

#include "tst_testattr.moc"
