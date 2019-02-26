#include "colorattr.hpp"
#include "base/meta.hpp"
#include "model/colorsmodel.hpp"
#include <QColor>

ColorAttr::ColorAttr()
    : mName()
    , mValue(0)
    , m_state(State::Null)
{}
ColorAttr::ColorAttr(const QColor& color)
    : mName()
    , mValue(color.rgba())
    , m_state(State::Fixed)
{}

ColorAttr::ColorAttr(const QString& str, bool invertAlpha)
    : ColorAttr()
{
    if (str.length() > 0 && str[0] == '#') {
        QColor color(str);
        if (color.isValid()) {
            if (invertAlpha) {
                // enigma2 representation is inverted
                // 255=transparent, 0=opaque
                color.setAlpha(255 - color.alpha());
            }
            mValue = color.rgba();
            m_state = State::Fixed;
        }
    } else {
        mName = str;
        m_state = State::Named;
    }
}

const QColor ColorAttr::value() const
{
    if (m_state == State::Fixed)
        return QColor::fromRgba(mValue);
    else
        return qRgb(0, 0, 0);
}

const QString& ColorAttr::name() const
{
    return mName;
}

void ColorAttr::setRawValue(QRgb value)
{
    Q_ASSERT(m_state == State::Named);
    mValue = value;
}

QString ColorAttr::toStr(bool invertAlpha) const
{
    if (m_state == State::Null) {
        return QString();
    }

    if (m_state == State::Fixed) {
        QColor color = value();
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

static ConverterRegistrator cr(&ColorAttr::toString);

// CachedColor

CachedColor::CachedColor(const ColorAttr& other)
    : ColorAttr(other)
{}

QColor CachedColor::getQColor() const
{
    if (state() != State::Null) {
        return QColor::fromRgba(getRawValue());
    } else {
        return QColor();
    }
}

void CachedColor::reload(const ColorsModel& model)
{
    if (state() == State::Named) {
        setRawValue(model.getValue(name()).value());
    }
}

void CachedColor::updateValue(QRgb value)
{
    setRawValue(value);
}
