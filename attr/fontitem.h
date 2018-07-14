#ifndef FONTITEM_H
#define FONTITEM_H

#include <QObject>
#include "attr/attritem.h"
#include "model/fontsmodel.h"

// Do I need this???

class FontAttr
{
public:
    FontAttr() : mName(), mSize(0) { }
    FontAttr(const QString &str) : FontAttr() { fromStr(str); }

    QString name() const { return mName; }
    QFont getFont() const;
    QString toStr() const;
    void fromStr(const QString &str);

private:
    QString mName;
    int mSize;
};
Q_DECLARE_METATYPE(FontAttr);


class FontItem : public AttrItem
{
public:
    explicit FontItem(WidgetData *widget, int key)
        : AttrItem(widget, key)  { }

    QVariant data(int role) const override;
    bool setData(const QVariant &value, int role) override;

private:
    inline FontAttr attr() const;
    inline void setAttr(const FontAttr &attr);

};

#endif // FONTITEM_H
