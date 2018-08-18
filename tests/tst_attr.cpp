#include "testcase.hpp"
#include "skin/sizeattr.hpp"
#include "skin/positionattr.hpp"
#include "skin/colorattr.hpp"

class TestAttr : public QObject
{
    Q_OBJECT
public:
    TestAttr();
    ~TestAttr();

private slots:
    void test_size() {
        SizeAttr sz;
        QVERIFY(sz.isRelative() == false);

        sz = SizeAttr("100,200");
        QVERIFY(sz.w().value() == 100 && sz.h().value() == 200);
    }

    void test_position() {
        PositionAttr pos("50,70");
        QVERIFY(pos.x().value() == 50 && pos.y().value() == 70);
    }
};

TestAttr::TestAttr()
{

}

TestAttr::~TestAttr()
{

}

static TestCase<TestAttr> t;
#include "tst_attr.moc"
