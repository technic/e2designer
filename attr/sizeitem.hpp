#ifndef SIZEITEM_H
#define SIZEITEM_H

#include "attritem.hpp"
#include "dimension.hpp"

class SizeAttr
{
public:
    SizeAttr(Dimension w = Dimension(), Dimension h = Dimension())
       : mWidth(w), mHeight(h) { }
    SizeAttr(const QString &str) { fromStr(str); }

    bool isRelative() const { return mWidth.isRelative() || mHeight.isRelative(); }
    QSize getSize(const WidgetData &widget);
    void setSize(const WidgetData &widget, const QSize size);

    QString toStr() const;
    void fromStr(const QString &str);

private:
    Dimension mWidth, mHeight;
};
Q_DECLARE_METATYPE(SizeAttr);


class SizeItem : public AttrItem
{
public:
    SizeItem(WidgetData *widget, int key) : AttrItem(widget, key) { }
    QVariant data(int role = Qt::UserRole + 1) const override;
    bool setData(const QVariant &value, int role) override;

private:
    inline SizeAttr attr() const;
    inline void setAttr(const SizeAttr &attr);
};


#endif // SIZEITEM_H
