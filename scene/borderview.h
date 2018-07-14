#ifndef BORDERVIEW_H
#define BORDERVIEW_H

#include <QGraphicsRectItem>
#include "model/borderset.h"

class BorderView : public QObject, public QGraphicsRectItem
{
    Q_OBJECT
public:
    BorderView(QGraphicsRectItem *parent);
    void setBorderSet(BorderSet *bs);
    void setInnerRect(const QRect &innerRect);

private:
    BorderSet *mBorders;

};


#endif // BORDERVIEW_H
