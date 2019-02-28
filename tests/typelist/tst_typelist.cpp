#include <QtTest>
#include "base/typelist.hpp"

class TestTypeList : public QObject
{
    Q_OBJECT

public:
    TestTypeList() {}
    ~TestTypeList() {}

    class C1
    {};
    class C2
    {};

    using ClassList = TypeList<C1, C2>;
    static_assert(ClassList::getIndex<C1>() == 0, "Test failed");
    static_assert(ClassList::getIndex<C2>() == 1, "Test failed");

private slots:
    void test_case()
    {
        QCOMPARE(ClassList::getIndex<C1>(), 0);
        QCOMPARE(ClassList::getIndex<C2>(), 1);
    }
};

QTEST_APPLESS_MAIN(TestTypeList)

#include "tst_typelist.moc"
