#pragma once

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
    /// Returns current screen index
    QModelIndex currentIndex() { return m_root; }
    int outputId() const { return m_outputId; }
    QGraphicsScene* scene() const { return m_scene; }
    void setScreen(QModelIndex index);
    ScreensModel* model() const { return m_model; }

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
    const int m_outputId = 0;

    // ref
    ScreensModel* m_model;
    QPointer<QItemSelectionModel> m_selectionModel;

    QPersistentModelIndex m_root;
    // flag to ignore signals from selectionModel
    bool m_disableSelectionSlots;

    // QObject owned
    QGraphicsScene* m_scene;
    // references within mScene
    QGraphicsPixmapItem* m_background;
    QGraphicsRectItem* m_backgroundRect;
    QHash<QPersistentModelIndex, WidgetGraphicsItem*> m_widgets;

    bool m_showBorders;
};

