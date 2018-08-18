#include <QtTest>

// add necessary includes here

class TestWidget : public QObject
{
    Q_OBJECT

public:
    TestWidget();
    ~TestWidget();

private slots:
    void test_case1();

};

TestWidget::TestWidget()
{

}

TestWidget::~TestWidget()
{

}

void TestWidget::test_case1()
{
    QVERIFY(false);
}

QTEST_APPLESS_MAIN(TestWidget)

#include "tst_testwidget.moc"
