#include <QtTest>

#include "skin/sizeattr.hpp"
#include "skin/positionattr.hpp"
#include "skin/colorattr.hpp"
#include "skin/attrcontainer.hpp"
#include <QString>

class TestContainer : public QObject
{
    Q_OBJECT
public:
    TestContainer() {}
    ~TestContainer() {}

    enum
    {
        size,
        pos,
        color,
        text
    };

    class SampleContainer : public AttrContainer
    {
    public:
        SampleContainer()
        {
            addAttr<SizeAttr>(size);
            addAttr<PositionAttr>(pos);
            addAttr<ColorAttr>(color);
            addAttr<QString>(text);
        }
    };

private slots:
    void test_set()
    {
        auto p1 = PositionAttr(7, 8);
        c.setAttr(pos, p1);
        QVERIFY(c.getAttr<PositionAttr>(pos) == p1);

        auto s1 = QString("foo");
        c.setAttr(text, s1);
        QVERIFY(c.getAttr<QString>(text) == s1);
    }

    void test_badSet()
    {
        auto p1 = c.getAttr<PositionAttr>(pos);
        c.setAttr(pos, 200);
        c.setAttr(pos, QString("bar"));
        QVERIFY(c.getAttr<PositionAttr>(pos) == p1);

        QVERIFY(c.getAttr<QString>(pos).isNull());
        auto s = c.getAttr<QString>(text);
        c.setAttr(text, SizeAttr());
        QVERIFY(c.getAttr<QString>(text) == s);
    }

private:
    SampleContainer c;
};

QTEST_APPLESS_MAIN(TestContainer)

#include "tst_testcontainer.moc"
