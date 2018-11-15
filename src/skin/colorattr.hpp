#ifndef COLORATTR_HPP
#define COLORATTR_HPP

#include <QMetaType>
#include <QRgb>
#include <QString>

class ColorAttr
{
public:
    ColorAttr();
    ColorAttr(const QColor& color);
    ColorAttr(const QString& str, bool invertAlpha = true);

    bool isDefined() const { return mDefined; }

    QColor getColor() const;
    void setColor(const QColor& color);

    const QString& name() const { return mName; }
    void setColorName(const QString& name);

    QString toStr(bool invertAlpha = true) const;
    inline QString toXml() const { return toStr(true); }
    bool fromStr(const QString& str, bool invertAlpha = false);

    void reload();
    void updateValue(QRgb value);

private:
    QString mName;
    QRgb mValue;
    bool mDefined;
};
Q_DECLARE_METATYPE(ColorAttr);

inline QString serialize(const ColorAttr &color) {
    return color.toStr();
}
inline void deserialize(const QString &str, ColorAttr &color) {
    color = ColorAttr(str);
}


#endif // COLORATTR_HPP
