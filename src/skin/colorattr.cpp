#include "colorattr.hpp"
#include "repository/skinrepository.hpp"
#include <QColor>

ColorAttr::ColorAttr()
    : mName()
    , mValue(0)
    , mDefined(false)
{
}
ColorAttr::ColorAttr(const QColor& color)
    : ColorAttr()
{
    setColor(color);
}
ColorAttr::ColorAttr(const QString& str, bool invertAlpha)
    : ColorAttr()
{
    fromStr(str, invertAlpha);
}

QColor ColorAttr::getColor() const
{
    if (mDefined) {
        return QColor::fromRgba(mValue);
    } else {
        return QColor();
    }
}
void ColorAttr::setColor(const QColor& color)
{
    // FIXME!
    mName = SkinRepository::colors()->getName(color.rgba());
//    mName = QString();
    mValue = color.rgba();
    mDefined = true;
}

void ColorAttr::setColorName(const QString& name)
{
    mName = name;
    mDefined = !name.isEmpty();
    reload();
}

QString ColorAttr::toStr(bool invertAlpha) const
{
    if (!mDefined) {
        return QString();
    }

    if (mName.isEmpty()) {
        QColor color = getColor();
        if (invertAlpha) {
            // enigma2 representation is inverted
            // 255=transparent, 0=opaque
            color.setAlpha(255 - color.alpha());
        }
        return color.name(QColor::HexArgb);
    } else {
        return mName;
    }
}

bool ColorAttr::fromStr(const QString& str, bool invertAlpha)
{
    if (str.length() > 0 && str[0] == '#') {
        mName = QString();
        QColor color(str);
        if (color.isValid()) {
            if (invertAlpha) {
                // enigma2 representation is inverted
                // 255=transparent, 0=opaque
                color.setAlpha(255 - color.alpha());
            }
            mValue = color.rgba();
            mDefined = true;
        } else {
            mValue = qRgb(0, 0, 0);
            mDefined = false;
        }
    } else {
        setColorName(str);
    }
    return mDefined;
}

void ColorAttr::reload()
{
    if (!mName.isEmpty()) {
        mValue = SkinRepository::colors()->getValue(mName).value();
    }
}
