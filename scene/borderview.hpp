#ifndef BORDERVIEW_H
#define BORDERVIEW_H

#include "model/borderset.hpp"
#include <QGraphicsRectItem>

class BorderView : public QObject, public QGraphicsRectItem
{
    Q_OBJECT
public:
    BorderView(QGraphicsRectItem* parent);
    void setBorderSet(BorderSet* bs);
    void setInnerRect(const QRect& innerRect);

private:
    BorderSet* mBorders;
    QRect mRect;
};

#endif // BORDERVIEW_H
