#ifndef POSITIONITEM_H
#define POSITIONITEM_H

#include "attritem.hpp"
#include "skin/positionattr.hpp"

typedef QPair<int, int> AnchorPair;
Q_DECLARE_METATYPE(AnchorPair)

class AbsolutePositionItem : public AttrItem
{
public:
    AbsolutePositionItem(WidgetData* widget)
        : AttrItem(widget)
    {
    }
    QVariant data(int role = Qt::UserRole + 1) const override;
};

class PositionItem : public AttrItem
{
public:
    PositionItem(WidgetData* widget, int key);
    enum AxesName { X, Y };

    // AttrItem interface
    QVariant data(int role) const;
    bool setData(const QVariant& value, int role);

    class CoordinateItem : public AttrItem
    {
    public:
        CoordinateItem(WidgetData* widget, AxesName axes)
            : AttrItem(widget)
            , axes(axes)
        {
        }
        QVariant data(int role) const final;
        bool setData(const QVariant& value, int role) final;

    private:
        AxesName axes;
    };

private:
    inline PositionAttr attr() const;
    inline void setAttr(const PositionAttr& value);
};

#endif // POSITIONITEM_H
