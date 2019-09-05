#pragma once

#include <QString>
#include <QFont>

class FontAttr
{
public:
    FontAttr()
        : m_name()
        , m_size(0)
    {}
    FontAttr(const QString& str)
        : FontAttr()
    {
        fromStr(str);
    }
    QString name() const { return m_name; }
    QFont getFont() const;
    QString toStr() const;
    void fromStr(const QString& str);

private:
    QString m_name;
    int m_size;
};
Q_DECLARE_METATYPE(FontAttr);

inline QString serialize(const FontAttr& font)
{
    return font.toStr();
}
inline void deserialize(const QString& str, FontAttr& font)
{
    font = FontAttr(str);
}

