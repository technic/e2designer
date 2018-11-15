#include <QtTest>
#include <QMetaProperty>
#include <iostream>
#include "skin/enums.hpp"
#include "skin/positionattr.hpp"
#include "skin/widgetdata.hpp"

#include "model/propertytree.hpp"

#include "model/propertiesmodel.hpp"
#include "model/colorsmodel.hpp"
#include "model/screensmodel.hpp"
#include "model/fontsmodel.hpp"

class TestWidget : public QObject
{
    Q_OBJECT
public:
    TestWidget() {}
    ~TestWidget() {}

    using Alphatest = Property::Alphatest;

private slots:
    void generate_code() {
        auto const fileName = QStringLiteral("funcs.cpp");
        qDebug() << "Generated code is in"
                 << QDir::current().filePath(fileName);
        QFile file(fileName);
        bool ok = file.open(QIODevice::WriteOnly);
        QVERIFY(ok);
        QTextStream ts(&file);

        auto metaEnum = Property::propertyEnum();
        for (int i = 0; i < metaEnum.keyCount(); ++i) {
            auto k = metaEnum.value(i);
            if (k == Property::invalid)
                continue;
            QString name = metaEnum.key(i);
            QString capName(name);
            capName[0] = capName[0].toUpper();
            ts << QString("add(p::%1, &w::%1, &w::set%2);").arg(name, capName)
               << endl;
        }
        file.close();
    }

    void test_allProperties()
    {

    }

    void test_color() {
        WidgetData w;
        ColorAttr col(Qt::red);
        w.setColor(Property::backgroundColor, col);
        QVERIFY(w.color(Property::backgroundColor).getColor() == Qt::red);
        QVERIFY(w.getAttr(Property::backgroundColor).value<ColorAttr>()
                .getColor() == Qt::red);
    }
    void test_alphatest() {
        WidgetData w;
        Alphatest alpha(Alphatest::on);
        w.setAlphatest(alpha);
        QVERIFY(w.alphatest() == Alphatest::on);

        w.setAttr(Property::alphatest, Alphatest::blend);
        QVERIFY(w.alphatest() == Alphatest::blend);
    }

    void test_xml() {
        QFile file(QFINDTESTDATA("widget.xml"));
        qDebug() << file;
        QVERIFY(file.open(QIODevice::ReadOnly));
        QXmlStreamReader xml(&file);
        xml.readNextStartElement();

        WidgetData w;
        w.fromXml(xml);
        QVERIFY(w.name() == "test1");
        QVERIFY(w.alphatest() == Alphatest::blend);
        QVERIFY(w.position() == PositionAttr(10, 20));
        QVERIFY(w.size() == SizeAttr(100, 300));
        QVERIFY(w.color(Property::backgroundColor).getColor() == QColor(Qt::red));
    }

    void test_tree() {
        QFile file(QFINDTESTDATA("widget.xml"));
        qDebug() << file;

        QVERIFY(file.open(QIODevice::ReadOnly));
        QXmlStreamReader xml(&file);
        xml.readNextStartElement();

        WidgetData w;
        w.fromXml(xml);

        PropertyTree tree(&w);
        QVERIFY(tree.getItemPtr(Property::position)->data(Qt::DisplayRole)
                == "10,20");
        QVERIFY(tree.getItemPtr(Property::size)->data(Qt::DisplayRole)
                == "100,300");
        QVERIFY(tree.getItemPtr(Property::alphatest)->data(Qt::DisplayRole)
                == "blend");
    }

    void test_modelSignal() {
        auto colors = new ColorsModel(this);
        auto fonts = new FontsModel(this);
        auto widgets = new ScreensModel(colors, fonts, this);

        widgets->insertRows(0, 1);
        auto i = widgets->index(0, 0);
        QCOMPARE(widgets->widgetAttr(i, Property::transparent), false);

        QSignalSpy spy(widgets, &ScreensModel::widgetChanged);
        bool ok = widgets->setWidgetAttr(i, Property::transparent, true);

        QVERIFY(ok);
        QCOMPARE(widgets->widgetAttr(i, Property::transparent), true);
        QCOMPARE(spy.count(), 1);
        QCOMPARE(spy.first().at(0), i);
        QCOMPARE(spy.first().at(1), Property::transparent);
    }

    void test_PropertiesModel() {
//        auto colors = new ColorsModel(this);
//        auto fonts = new FontsModel(this);
//        auto widgets = new ScreensModel(colors, fonts, this);

//        QFile file(QFINDTESTDATA("widget.xml"));
//        QVERIFY(file.open(QIODevice::ReadOnly));
//        QXmlStreamReader xml(&file);
//        xml.readNextStartElement();
        QFAIL("TODO");
    }
};

QTEST_APPLESS_MAIN(TestWidget)

#include "tst_testwidget.moc"
