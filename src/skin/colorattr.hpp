#pragma once

#include <QMetaType>
#include <QRgb>
#include <QString>

class ColorAttr
{
public:
    enum class State
    {
        Null,
        Fixed,
        Named,
    };

    ColorAttr();
    ColorAttr(const QColor& color);
    ColorAttr(const QString& str, bool invertAlpha = true);

    bool isDefined() const { return m_state != State::Null; }
    State state() const { return m_state; }
    /// Value for a fixed color
    const QColor value() const;
    /// Name for a named color
    const QString& name() const;

    QString toString() const { return toStr(false); }
    inline QString toXml() const { return toStr(true); }

protected:
    void setRawValue(QRgb value);
    QRgb getRawValue() const { return m_value; };

private:
    QString toStr(bool invertAlpha) const;

    QString m_name;
    QRgb m_value;
    State m_state;
};
Q_DECLARE_METATYPE(ColorAttr);

class ColorsModel;

class CachedColor : public ColorAttr
{
public:
    CachedColor() {}
    CachedColor(const ColorAttr& other);
    QColor getQColor() const;
    void reload(const ColorsModel& model);
    void updateValue(QRgb value);
};

inline QString serialize(const ColorAttr& color)
{
    return color.toXml();
}
inline void deserialize(const QString& str, ColorAttr& color)
{
    color = ColorAttr(str);
}
