#include <QtTest>
#include <QMetaProperty>
#include <QDateTime>
#include <iostream>
#include "skin/enums.hpp"
#include "skin/positionattr.hpp"
#include "skin/widgetdata.hpp"

#include "model/propertytree.hpp"

#include "model/propertiesmodel.hpp"
#include "model/colorsmodel.hpp"
#include "model/windowstyle.hpp"
#include "model/screensmodel.hpp"
#include "model/fontsmodel.hpp"

class TestWidget : public QObject
{
    Q_OBJECT
public:
    using Alphatest = Property::Alphatest;

private slots:
    void initTestCase() { Q_INIT_RESOURCE(resources); }

    void generate_code()
    {
        auto const fileName = QStringLiteral("funcs.cpp");
        qDebug() << "Generated code is in" << QDir::current().filePath(fileName);
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
            ts << QString("add(p::%1, &w::%1, &w::set%2);").arg(name, capName) << endl;
        }
        file.close();
    }

    //    void test_allProperties()
    //    {

    //    }

    void test_color()
    {
        WidgetData w;
        ColorAttr col(Qt::red);
        w.setColor(Property::backgroundColor, col);
        QVERIFY(w.color(Property::backgroundColor).value() == Qt::red);
        QVERIFY(w.getAttr(Property::backgroundColor).value<ColorAttr>().value() == Qt::red);
    }
    void test_alphatest()
    {
        WidgetData w;
        Alphatest alpha(Alphatest::on);
        w.setAlphatest(alpha);
        QVERIFY(w.alphatest() == Alphatest::on);

        w.setAttr(Property::alphatest, Alphatest::blend);
        QVERIFY(w.alphatest() == Alphatest::blend);
    }

    void test_xml()
    {
        QFile file(QFINDTESTDATA("widget.xml"));
        QVERIFY(file.open(QIODevice::ReadOnly));
        QXmlStreamReader xml(&file);
        xml.readNextStartElement();

        WidgetData w;
        QVERIFY(w.fromXml(xml));
        QVERIFY(w.name() == "test1");
        QVERIFY(w.alphatest() == Alphatest::blend);
        QVERIFY(w.position() == PositionAttr(10, 20));
        QVERIFY(w.size() == SizeAttr(100, 300));
        QVERIFY(w.color(Property::backgroundColor).value() == QColor(Qt::red));
    }

    void test_invalid_xml()
    {
        QXmlStreamReader xml(R"(<widget name="foobar"><widget_bad/>)");
        xml.readNextStartElement();

        WidgetData w;
        QCOMPARE(w.fromXml(xml), false);
        QVERIFY(xml.hasError());
        qDebug() << xml.error() << xml.errorString();
    }

    void test_invalid_xml_tag()
    {
        QXmlStreamReader xml(R"(<badtag name="foobar"><badtag/>)");
        xml.readNextStartElement();

        WidgetData w;
        QCOMPARE(w.fromXml(xml), false);
        QVERIFY(xml.hasError());
        qDebug() << xml.error() << xml.errorString();
    }

    void test_tree()
    {
        QFile file(QFINDTESTDATA("widget.xml"));
        qDebug() << file.fileName();

        QVERIFY(file.open(QIODevice::ReadOnly));
        QXmlStreamReader xml(&file);
        xml.readNextStartElement();

        WidgetData w;
        w.fromXml(xml);

        PropertyTree tree(&w);
        QVERIFY(tree.getItemPtr(Property::position)->data(Qt::DisplayRole) == "10,20");
        QVERIFY(tree.getItemPtr(Property::size)->data(Qt::DisplayRole) == "100,300");
        QVERIFY(tree.getItemPtr(Property::alphatest)->data(Qt::DisplayRole) == "blend");
    }

    void test_modelSignal()
    {
        auto colors = new ColorsModel(this);
        auto colorRoles = new ColorRolesModel(*colors, this);
        auto fonts = new FontsModel(this);
        auto widgets = new ScreensModel(*colors, *colorRoles, *fonts, this);

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

    void test_colorPalleteSignals()
    {
        auto colors = new ColorsModel(this);
        auto colorRoles = new ColorRolesModel(*colors, this);
        auto fonts = new FontsModel(this);
        auto widgets = new ScreensModel(*colors, *colorRoles, *fonts, this);

        auto colRed = QColor(Qt::red);
        auto colDarkRed = QColor(Qt::darkRed);

        colors->append(Color("red", colRed.rgba()));

        widgets->insertRows(0, 2);
        auto i0 = widgets->index(0, 0);
        auto i1 = widgets->index(1, 0);
        auto col = ColorAttr(QString("red"));
        for (auto& i : { i0, i1 }) {
            bool ok =
              widgets->setWidgetAttr(i, Property::foregroundColor, QVariant::fromValue(col));
            QVERIFY(ok);
        }

        // Tell model to send notifications about our widget
        WidgetObserverRegistrator r0{ widgets, i0 };
        QSignalSpy spy(widgets, &ScreensModel::widgetChanged);

        colors->setData(colors->index(0, ColorsModel::ColumnColor),
                        QVariant::fromValue(colDarkRed));

        QCOMPARE(spy.count(), 1);
        QCOMPARE(spy.first().at(0), i0);
        QCOMPARE(spy.first().at(1), Property::foregroundColor);
        QCOMPARE(widgets->widget(i0).getQColor(Property::foregroundColor), colDarkRed);

        WidgetObserverRegistrator r1{ widgets, i1 };
        QCOMPARE(widgets->widget(i1).getQColor(Property::foregroundColor), colDarkRed);
    }

    //    void test_PropertiesModel() {
    //        auto colors = new ColorsModel(this);
    //        auto fonts = new FontsModel(this);
    //        auto widgets = new ScreensModel(colors, fonts, this);

    //        QFile file(QFINDTESTDATA("widget.xml"));
    //        QVERIFY(file.open(QIODevice::ReadOnly));
    //        QXmlStreamReader xml(&file);
    //        xml.readNextStartElement();
    //        QFAIL("TODO");
    //    }

    void test_converter()
    {
        QFile file(QFINDTESTDATA("widget.xml"));
        qDebug() << file.fileName();
        QVERIFY(file.open(QIODevice::ReadOnly));
        QXmlStreamReader xml(&file);
        xml.readNextStartElement();
        WidgetData w;
        w.fromXml(xml);

        QCOMPARE(w.scenePreview().toString(), QDateTime::currentDateTime().toString("h:mm"));
    }

    void test_ColorRoles()
    {
        auto colors = new ColorsModel(this);
        auto colorRoles = new ColorRolesModel(*colors, this);
        auto fonts = new FontsModel(this);
        auto widgets = new ScreensModel(*colors, *colorRoles, *fonts, this);
        WindowStyle style;
        colorRoles->setStyle(&style);

        using Role = WindowStyleColor::ColorRole;
        style.setColor(Role::Background, ColorAttr(Qt::red));

        widgets->insertRows(0, 1);
        auto index = widgets->index(0, 0);
        WidgetObserverRegistrator reg{ widgets, index };
        QCOMPARE(widgets->widget(index).getQColor(Property::backgroundColor), Qt::red);

        style.setColor(Role::LabelForeground, ColorAttr(QString("green")));
        colors->append(Color(QString("green"), QColor(Qt::green).rgba()));
        QCOMPARE(widgets->widget(index).getQColor(Property::foregroundColor), Qt::green);

        QSignalSpy spy(widgets, &ScreensModel::widgetChanged);
        colors->setData(colors->index(0, ColorsModel::ColumnColor), QColor(Qt::darkGreen));
        QCOMPARE(spy.count(), 1);
        QCOMPARE(spy.first().at(0), index);
        QCOMPARE(spy.first().at(1), Property::foregroundColor);
        QCOMPARE(widgets->widget(index).getQColor(Property::foregroundColor), Qt::darkGreen);
    }
};

QTEST_APPLESS_MAIN(TestWidget)

#include "tst_testwidget.moc"
