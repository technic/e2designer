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
    QGraphicsScene* scene() const { return mScene; }
    RectSelector* selector() const { return mSelector; }
    void setScreen(QModelIndex index);
    //	SkinModel* model() const { return m_model; }

    void setSelectionModel(QItemSelectionModel* model);

    void deleteSelected();

private slots:
    // Screens Model
    void onRowsAboutToBeRemoved(const QModelIndex& parent, int first, int last);
    void onRowsInserted(const QModelIndex& parent, int first, int last);
    void onModelAboutToBeReset();
    void onModelReset();

    // Scene
    void onSceneSelectionChanged();

    // Selection Model
    void setCurrentWidget(const QModelIndex &current, const QModelIndex &previous);
    void updateSelection(const QItemSelection &selected, const QItemSelection &deselected);

    // Outputs
    void onOutputChanged(int id, const VideoOutput &output);
    void setSceneSize(const QSize &size);

private:
    QModelIndex normalizeIndex(const QModelIndex &index) const;
    QItemSelection makeRowSelection(const QModelIndex &index);
    //	bool isInOurView(QModelIndex index);
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
    RectSelector* mSelector;
    QGraphicsPixmapItem* mBackground;
    QGraphicsRectItem* mBackgroundRect;
    QHash<QPersistentModelIndex, WidgetView*> mWidgets;
};

#endif // SCREENVIEW_H
