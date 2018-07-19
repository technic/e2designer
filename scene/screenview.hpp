#ifndef SCREENVIEW_H
#define SCREENVIEW_H

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
    ScreenView(ScreensModel *model);
    // returns current screen index
    QModelIndex currentIndex() { return mRoot; }
    QGraphicsScene* scene() const { return mScene; }
    RectSelector* selector() const { return mSelector; }

    void setScreen(QModelIndex index);
    //	SkinModel* model() const { return m_model; }

    void point(QItemSelectionModel *model);

    void deleteSelected();


signals:
    void selectionChanged(QModelIndex index);

private slots:
    // Screens Model
    void onRowsAboutToBeRemoved(const QModelIndex &parent, int first, int last);
    void onRowsInserted(const QModelIndex &parent, int first, int last);
    void onModelAboutToBeReset();
    void onModelReset();

    // Scene
    void onSceneSelectionChanged();

    // Selection Model
    void selectCurrentWidget(QModelIndex selected, QModelIndex deselected);

private:
    //	bool isInOurView(QModelIndex index);

    // ref
    ScreensModel *mModel;
    QItemSelectionModel *mSelectionModel;

    QPersistentModelIndex mRoot;

    // QObject owned
    QGraphicsScene *mScene;
    // references within mScene
    RectSelector *mSelector;
    QHash<QPersistentModelIndex, WidgetView*> mWidgets;

};

#endif // SCREENVIEW_H
