#include <QtTest>
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
    void test_slot1();
};

TestAttr::TestAttr()
{

}

TestAttr::~TestAttr()
{

}

void TestAttr::test_slot1()
{

}

QTEST_APPLESS_MAIN(TestAttr)

#include "tst_attr.moc"
