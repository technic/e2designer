#ifndef OFFSETATTR_HPP
#define OFFSETATTR_HPP

#include <QString>
#include <QVariant>

class OffsetAttr
{
public:
    explicit OffsetAttr(int x = 0, int y = 0);
    OffsetAttr(const QString &str);
    QString toStr() const;
private:
    int m_x, m_y;
};
Q_DECLARE_METATYPE(OffsetAttr)

inline QString serialize(OffsetAttr offset) {
    return offset.toStr();
}
inline void deserialize(const QString &str, OffsetAttr &offset) {
    offset = OffsetAttr(str);
}

#endif // OFFSETATTR_HPP
