#ifndef POSITIONITEM_H
#define POSITIONITEM_H

#include "attritem.h"
#include "coordinate.h"

typedef QPair<int,int> AnchorPair;
Q_DECLARE_METATYPE(AnchorPair)

// Data

class PositionAttr
{
public:
    PositionAttr() { }
    PositionAttr(const QString &str) { fromStr(str); }

    bool isRelative() const { return mX.isRelative() || mY.isRelative(); }
    AnchorPair anchor() { return AnchorPair(mX.anchor(), mY.anchor()); }

    QPoint toPoint(const WidgetData &widget);
    void setPoint(const WidgetData &widget, const QPoint &pos);

    QString toStr() const;
    void fromStr(const QString &str);

private:
    Coordinate mX, mY;
};
Q_DECLARE_METATYPE(PositionAttr);


// Adapter

class AbsolutePositionItem : public AttrItem
{
public:
    AbsolutePositionItem(WidgetData *widget)
        : AttrItem(widget) { }
    QVariant data(int role = Qt::UserRole + 1) const override;
};


class PositionItem: public AttrItem
{
public:
    PositionItem(WidgetData *widget, int key);
    enum AxesName { X, Y };

    // AttrItem interface
    QVariant data(int role) const;
    bool setData(const QVariant &value, int role);

    class CoordinateItem : public AttrItem
    {
    public:
        CoordinateItem(WidgetData *widget, AxesName axes)
            : AttrItem(widget), axes(axes) { }

        QVariant data(int role) const final;
        bool setData(const QVariant &value, int role) final;
    private:
        AxesName axes;
    };

private:
    inline PositionAttr attr() const;
    inline void setAttr(const PositionAttr &value);
};



#endif // POSITIONITEM_H
