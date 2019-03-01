#include <QtTest>
#include <QVector>
#include <vector>
#include "model/namedlist.hpp"
#include "skin/colorattr.hpp"

// add necessary includes here

class Item
{
public:
    using Value = int;
    Item()
        : _name()
        , _value(0)
    {}
    Item(const QString& name, int value)
        : _name(name)
        , _value(value)
    {}
    QString name() const { return _name; }
    bool isNull() const { return _name.isNull(); }

    bool operator==(const Item& other) const
    {
        return other._name == _name && other._value == _value;
    }

private:
    QString _name;
    int _value;
};

QDebug operator<<(QDebug d, const Item& item)
{
    d << item.name();
    return d;
}

class ItemList : public NamedList<Item>
{
    friend class TestCore;

    // NamedList interface
protected:
    void emitValueChanged(const QString& name, const Item& value) const final{};
};

class TestCore : public QObject
{
    Q_OBJECT

private slots:
    void test_moveItems()
    {
        ItemList list;
        list.appendItem(Item("a", 0));
        list.appendItem(Item("b", 1));
        list.appendItem(Item("c", 2));

        QVERIFY(list.moveItems(0, 1, 2));
        {
            std::vector<Item> v1(list.begin(), list.end());
            std::vector<Item> v2({ Item("b", 1), Item("a", 0), Item("c", 2) });
            qDebug() << v1 << v2;
            QCOMPARE(v1, v2);
        }
        QVERIFY(list.moveItems(2, 1, 1));
        {
            std::vector<Item> v1(list.begin(), list.end());
            std::vector<Item> v2({ Item("b", 1), Item("c", 2), Item("a", 0) });
            qDebug() << v1 << v2;
            QCOMPARE(v1, v2);
        }
    }
};

QTEST_APPLESS_MAIN(TestCore)

#include "tst_testcore.moc"
