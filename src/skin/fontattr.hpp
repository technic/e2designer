#ifndef FONTATTR_HPP
#define FONTATTR_HPP

#include <QString>
#include <QFont>

class FontAttr
{
public:
    FontAttr()
        : mName()
        , mSize(0)
    {}
    FontAttr(const QString& str)
        : FontAttr()
    {
        fromStr(str);
    }
    QString name() const { return mName; }
    QFont getFont() const;
    QString toStr() const;
    void fromStr(const QString& str);

private:
    QString mName;
    int mSize;
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

#endif // FONTATTR_HPP
