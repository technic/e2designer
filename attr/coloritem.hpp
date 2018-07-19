#ifndef COLORITEM_H
#define COLORITEM_H

#include <QColor>
#include <QRgb>
#include "attr/attritem.hpp"

class ColorAttr
{
public:
    ColorAttr();
    ColorAttr(const QColor &color);
    ColorAttr(const QString &str, bool invertAlpha = false);

    bool isDefined() const;

    QColor getColor() const;
    void setColor(const QColor &color);

    QString getName() const { return mName; }
    void setColorName(const QString &name);

    QString toStr(bool invertAlpha = false) const;
    bool fromStr(const QString &str, bool invertAlpha = false);

    void reload();

private:
    QString mName;
    QRgb mValue;
    bool mDefined;
};
Q_DECLARE_METATYPE(ColorAttr);


class ColorItem : public AttrItem
{
public:
    explicit ColorItem(WidgetData *widget, int key)
        : AttrItem(widget, key) { }

    QVariant data(int role) const override;
    bool setData(const QVariant &value, int role) override;

private:
    inline ColorAttr attr() const;
    inline void setAttr(const ColorAttr &attr);
    ColorAttr mData;
};

#endif // COLORITEM_H
