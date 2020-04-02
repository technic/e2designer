#pragma once
#include <QTreeView>
#include <QKeyEvent>

class CustomTreeView : public QTreeView
{
    Q_OBJECT
public:
    CustomTreeView(QWidget* parent = nullptr)
        : QTreeView(parent)
    {}

    void keyPressEvent(QKeyEvent* event) override
    {
        QTreeView::keyPressEvent(event);
        if (!event->isAccepted()) {
            switch (event->key()) {
            // Qt only allows start editing with Key_F2
            // I want to also start editing with Enter
            // TODO: According to comments in the original keyPressEvent method
            //       this code might cause inifinite loop for some editors
            case Qt::Key_Return:
            case Qt::Key_Enter:
                if (state() != EditingState && !edit(currentIndex(), EditKeyPressed, event)) {
                    event->ignore();
                }
                break;
            default:
                event->ignore();
            }
        }
    }
};
