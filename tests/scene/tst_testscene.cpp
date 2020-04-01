#include <QtTest>
#include "scene/screenview.hpp"
#include "repository/skinrepository.hpp"

// add necessary includes here

class TestScene : public QObject
{
    Q_OBJECT

public:
    TestScene()
        : m_model(SkinRepository::screens())
        , m_view(new ScreenView(m_model))
    {
        bool ok = SkinRepository::instance().open(
          QFileInfo(QFINDTESTDATA("skin.xml")).absoluteDir().path());
        Q_ASSERT(ok);
        printTree();
    }
    ~TestScene() = default;

private slots:
    void test_move()
    {
        auto selection = new QItemSelectionModel(m_model, this);
        m_view->setSelectionModel(selection);

        QPersistentModelIndex s0 = m_model->index(0, 0);
        // Move widget out of screen
        m_model->moveRows(s0, 0, 1, QModelIndex(), 0);
        // Display that screen
        m_view->setScreen(s0);
        // Move widget into the screen and select
        m_model->moveRows(QModelIndex(), 0, 1, s0, 0);
        selection->setCurrentIndex(s0.child(0, 0), QItemSelectionModel::Current);
        qDebug() << selection->currentIndex();
    }

private:
    ScreensModel* m_model;
    ScreenView* m_view;

    void printTree() { printSubTree(QModelIndex()); }

    void printSubTree(QModelIndex index, int level = 0)
    {
        qDebug() << "*" + QString("-").repeated(level)
                 << m_model->data(index, Qt::DisplayRole).toString()
                 << m_model->data(index.sibling(index.row(), 1), Qt::DisplayRole).toString();
        for (int r = 0; r < m_model->rowCount(index); ++r) {
            printSubTree(m_model->index(r, 0, index), level + 1);
        }
    }
};

QTEST_MAIN(TestScene)

#include "tst_testscene.moc"
