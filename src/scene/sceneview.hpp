#ifndef SCENEVIEW_H
#define SCENEVIEW_H

#include <QGraphicsView>

class SceneView : public QGraphicsView
{
public:
    SceneView(QWidget* parent = Q_NULLPTR);

protected:
    // QWidget interface
    void wheelEvent(QWheelEvent* event) override;
};

#endif // SCENEVIEW_H
