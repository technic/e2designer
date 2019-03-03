#ifndef SCREENVIEW_H
#define SCREENVIEW_H

#include <QPointer>
#include <QGraphicsScene>
#include <QItemSelectionModel>

#include "widgetview.hpp"

/**
 * @brief The ScreenView class
 * provides view of specified Screen with the QGraphicsScene
 */
class ScreenView : public QObject
{
    Q_OBJECT

public:
    ScreenView(ScreensModel* model);
    // returns current screen index
    QModelIndex currentIndex() { return mRoot; }
    int outputId() const { return mOutputId; }
    QGraphicsScene* scene() const { return mScene; }
    void setScreen(QModelIndex index);
    ScreensModel* model() const { return mModel; }

    void setSelectionModel(QItemSelectionModel* model);

    void deleteSelected();

    // Widget borders
    bool haveBorders() const { return m_showBorders; }

public slots:
    void displayBorders(bool display);

private slots:
    // Screens Model
    void onWidgetChanged(const QModelIndex& index, int key);
    void onRowsAboutToBeRemoved(const QModelIndex& parent, int first, int last);
    void onRowsInserted(const QModelIndex& parent, int first, int last);
    void onModelAboutToBeReset();
    void onModelReset();

    // Scene
    void onSceneSelectionChanged();

    // Selection Model
    void setCurrentWidget(const QModelIndex& current, const QModelIndex& previous);
    void updateSelection(const QItemSelection& selected, const QItemSelection& deselected);

    // Outputs
    void onOutputChanged(int id, const VideoOutput& output);
    void setSceneSize(const QSize& size);

private:
    QModelIndex normalizeIndex(const QModelIndex& index) const;
    QItemSelection makeRowSelection(const QModelIndex& index);
    const int mOutputId = 0;

    // ref
    ScreensModel* mModel;
    QPointer<QItemSelectionModel> mSelectionModel;

    QPersistentModelIndex mRoot;
    // flag to ignore signals from selectionModel
    bool mDisableSelectionSlots;

    // QObject owned
    QGraphicsScene* mScene;
    // references within mScene
    QGraphicsPixmapItem* mBackground;
    QGraphicsRectItem* mBackgroundRect;
    QHash<QPersistentModelIndex, WidgetGraphicsItem*> mWidgets;

    bool m_showBorders;
};

#endif // SCREENVIEW_H
