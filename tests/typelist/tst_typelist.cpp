#include <QtTest>
#include "base/typelist.hpp"

class TestTypeList : public QObject
{
    Q_OBJECT

public:
    TestTypeList() {}
    ~TestTypeList() {}

    class C1;
    class C2;
    using ClassList = TypeList<C1, C2>;

    class C1
    {
    public:
        int id() const { return ClassList::getIndex(this); }
    };
    class C2
    {
    public:
        int id() const { return ClassList::getIndex(this); }
    };

    static_assert(ClassList::getIndex<C1>() == 0, "Test failed");
    static_assert(ClassList::getIndex<C2>() == 1, "Test failed");

private slots:
    void test_case()
    {
        QCOMPARE(ClassList::getIndex<C1>(), 0);
        QCOMPARE(ClassList::getIndex<C2>(), 1);

        QCOMPARE(C1().id(), 0);
        QCOMPARE(C2().id(), 1);
    }
};

QTEST_APPLESS_MAIN(TestTypeList)

#include "tst_typelist.moc"
