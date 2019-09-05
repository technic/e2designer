#pragma once

#include <QGraphicsView>

class SceneView : public QGraphicsView
{
public:
    SceneView(QWidget* parent = Q_NULLPTR);

protected:
    // QWidget interface
    void wheelEvent(QWheelEvent* event) override;
};

