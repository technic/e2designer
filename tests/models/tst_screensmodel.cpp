#include <QTest>
#include <QAbstractItemModel>
#include <QAbstractItemModelTester>
#include "autotests.hpp"
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
                     "widget");
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
};

QTEST_APPLESS_MAIN(TestScreensModel)

// static Test test([](int argc, char** argv) -> int {
//     TestScreensModel tc;
//     return QTest::qExec(&tc, argc, argv);
// });

#include "tst_screensmodel.moc"
