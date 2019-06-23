#include <QtTest>
#include "base/flagsetter.hpp"

class TestMisc : public QObject
{
    Q_OBJECT

private slots:
    void test_flagSetter()
    {
        bool b = false;
        {
            FlagSetter fs(&b);
            QCOMPARE(b, true);
        }
        QCOMPARE(b, false);

        bool b1 = true;
        {
            FlagSetter fs(&b1);
            QCOMPARE(b1, true);
        }
        QCOMPARE(b1, true);
    }
};

QTEST_APPLESS_MAIN(TestMisc)

#include "tst_testmisc.moc"
