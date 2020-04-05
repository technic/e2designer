#include <QTest>
#include <QAbstractItemModel>
#include <QAbstractItemModelTester>
#include "model/screensmodel.hpp"
#include "model/colorsmodel.hpp"
#include "model/fontsmodel.hpp"

class TestScreensModel : public QObject
{
    Q_OBJECT
private slots:
    void test_insertRemoveRows()
    {
        auto* colors = new ColorsModel(this);
        auto* colorRoles = new ColorRolesModel(*colors, this);
        auto* fonts = new FontsModel(this);
        ScreensModel model(*colors, *colorRoles, *fonts, this);
        QAbstractItemModelTester modelTester(&model, this);

        auto root = QModelIndex();
        model.insertRows(0, 4, root);
        for (int i = 0; i < 4; i++) {
            QCOMPARE(model.data(model.index(i, ScreensModel::ColumnElement, root), Qt::DisplayRole)
                       .toString(),
                     "screen");
        }
        model.undoStack()->undo();
        QCOMPARE(model.rowCount(), 0);
        model.undoStack()->redo();
        QCOMPARE(model.rowCount(), 4);

        for (int i = 0; i < 4; i++) {
            QVERIFY(
              model.setData(model.index(i, ScreensModel::ColumnName, root), QString("w%1").arg(i)));
        }

        model.removeRows(1, 2, root);
        QCOMPARE(model.rowCount(), 2);
        QCOMPARE(
          model.data(model.index(0, ScreensModel::ColumnName, root), Qt::DisplayRole).toString(),
          "w0");
        QCOMPARE(
          model.data(model.index(1, ScreensModel::ColumnName, root), Qt::DisplayRole).toString(),
          "w3");
        model.undoStack()->undo();
        for (int i = 0; i < 4; i++) {
            QCOMPARE(model.data(model.index(i, ScreensModel::ColumnName, root), Qt::DisplayRole),
                     QString("w%1").arg(i));
        }
    }

    void test_xml()
    {
        auto* colors = new ColorsModel(this);
        auto* colorRoles = new ColorRolesModel(*colors, this);
        auto* fonts = new FontsModel(this);
        ScreensModel model(*colors, *colorRoles, *fonts, this);

        model.insertRow(0, QModelIndex());
        auto s = model.index(0, 0, QModelIndex());
        model.setWidgetAttr(s, Property::name, "sn");

        model.insertRow(0, s);
        auto w = model.index(0, 0, s);
        model.setWidgetAttr(w, Property::name, "wn");
        model.setWidgetAttr(w, Property::text, "text_a");
        model.setWidgetAttr(w, Property::previewValue, "value");

        QCOMPARE(model.widgetAttr(w, Property::text), "text_a");
        QCOMPARE(model.widgetAttr(w, Property::previewValue), "value");

        QXmlStreamReader xml_data(QString(R"#(<widget name="wn" text="text_b"/>)#"));
        xml_data.readNextStartElement();
        model.setWidgetDataFromXml(w, xml_data);

        w = model.index(0, 0, s); // Get new index after widget was reloaded
        QCOMPARE(model.widgetAttr(w, Property::text), "text_b");
        QCOMPARE(model.widgetAttr(w, Property::previewValue), "value");
    }
};

QTEST_APPLESS_MAIN(TestScreensModel)

#include "tst_screensmodel.moc"
