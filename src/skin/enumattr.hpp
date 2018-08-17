#ifndef ENUMATTR_HPP
#define ENUMATTR_HPP

#include <QString>
#include <QStringList>

/**
 * @brief Attribute represented by enum
 */
template <class Enum>
class EnumAttr
{
public:
    EnumAttr();
    EnumAttr(const QString& str);
    EnumAttr(const int value);

    inline int value() const { return mValue; }
    QString toStr(bool emptyDefault = false) const;
    QStringList toStrList() const;

private:
    int mValue;
};

#endif // ENUMATTR_HPP
