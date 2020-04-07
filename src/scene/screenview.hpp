#pragma once

#include <QPointer>
#include <QGraphicsScene>
#include <QItemSelectionModel>

#include "widgetview.hpp"

// QModelIndex normalizeIndex(const QModelIndex& index);
// bool screenLike(int type)
//{
//    using t = WidgetData::WidgetType;
//    return type == static_cast<int>(t::Screen) || type == static_cast<int>(t::Panel);
//}

/**
 * @brief Main editor scene, contains several @a ScreenView's
 */
class SkinScene : public QGraphicsScene
{
    Q_OBJECT

public:
    SkinScene(ScreensModel* model);

    // Outputs
    int outputId() const { return m_outputId; }
    void onOutputChanged(int id, const VideoOutput& output);
    void setSceneSize(const QSize& size);

    void setSelectionModel(QItemSelectionModel* model);

public slots:
    void setScreen(QModelIndex index);
    void displayBorders(bool display);

private slots:
    void setCurrentWidget(const QModelIndex& current, const QModelIndex& previous);

private:
    void addScreenRecursive(QModelIndex index);

    const int m_outputId = 0;
    // ref
    ScreensModel* m_model;
    QPointer<QItemSelectionModel> m_selectionModel;
    // QGraphicsScene owned
    QGraphicsPixmapItem* m_background;
    QGraphicsRectItem* m_backgroundRect;

    std::vector<std::unique_ptr<ScreenView>> m_screens;
};

/**
 * @brief The ScreenView class
 * provides view of specified Screen with the QGraphicsScene
 */
class ScreenView : public QObject
{
    Q_OBJECT

public:
    ScreenView(ScreensModel* model, QModelIndex index, SkinScene* scene);
    ~ScreenView();
    /// Returns current screen index
    QModelIndex currentIndex() { return m_root; }
    QGraphicsScene* scene() const { return m_scene; }
    void setScreen(QModelIndex index);
    ScreensModel* model() const { return m_model; }

    QModelIndex rootIndex() const { return m_root; }

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
    void onRowsAboutToBeMoved(const QModelIndex& sourceParent,
                              int sourceStart,
                              int sourceEnd,
                              const QModelIndex& destinationParent,
                              int destinationRow);
    void onRowsMoved(const QModelIndex& parent,
                     int start,
                     int end,
                     const QModelIndex& destination,
                     int row);
    void onRowsInserted(const QModelIndex& parent, int first, int last);
    void onModelAboutToBeReset();
    void onModelReset();

    // Scene
    void onSceneSelectionChanged();

    // Selection Model
    void setCurrentWidget(const QModelIndex& current, const QModelIndex& previous);
    void updateSelection(const QItemSelection& selected, const QItemSelection& deselected);

private:
    QItemSelection makeRowSelection(const QModelIndex& index);
    bool belongsToRoot(QModelIndex index) const;
    bool containsOurRoot(const QModelIndex& index, int first, int last) const;
    void removeChildren(const QModelIndex& parent, int first, int last);

    // ref
    ScreensModel* m_model;
    QGraphicsScene* m_scene;
    QPointer<QItemSelectionModel> m_selectionModel;

    QPersistentModelIndex m_root;
    // flag to ignore signals from selectionModel
    bool m_disableSelectionSlots;

    // references within scene
    QHash<QPersistentModelIndex, WidgetGraphicsItem*> m_widgets;

    bool m_showBorders;
};
