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
    ColorAttr(const QString& str, bool invertAlpha = false);

    bool isDefined() const { return mDefined == true; }

    QColor getColor() const;
    void setColor(const QColor& color);

    QString getName() const { return mName; }
    void setColorName(const QString& name);

    QString toStr(bool invertAlpha = false) const;
    bool fromStr(const QString& str, bool invertAlpha = false);

    void reload();

private:
    QString mName;
    QRgb mValue;
    bool mDefined;
};
Q_DECLARE_METATYPE(ColorAttr);

#endif // COLORATTR_HPP
