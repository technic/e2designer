#ifndef POSITIONATTR_HPP
#define POSITIONATTR_HPP

#include <QString>
#include <QPair>
#include <QPoint>
#include <QMetaType>

/**
 * @brief The Coordinate class
 * represents coordinate in a way it is stored in the skin
 * allows getting actual value relative to parent
 * has string convert functions
 */
class Coordinate
{
public:
    enum Type {
        Pixel,
        Percent,
        Center,
    };
    Coordinate(int px) : Coordinate(Pixel, px) {}
    explicit Coordinate(Type type = Pixel, int value = 0);
    explicit Coordinate(const QString &str);

    inline Type type() const { return m_type; }
    inline int value() const { return m_value; }
    /// Load and save text
    void parseStr(const QString& str);
    QString toStr() const;

    /// Convert to numerical value
    int getInt(int my_size, int parent_size) const;
    /// Try to convert back to smart value aka "center"
    void parseInt(int value, int my_size, int parent_size);

    /// Depends on parent or on size
    bool isRelative() const;

    inline bool operator==(const Coordinate &other) {
        return m_type == other.m_type && m_value == other.m_value;
    }

private:
    Type m_type;
    int m_value;
};


class WidgetData;

typedef QPair<int, int> AnchorPair;

class PositionAttr
{
public:
    PositionAttr() {}
    PositionAttr(const Coordinate &x, const Coordinate &y);
    PositionAttr(const QString& str) { fromStr(str); }
    inline const Coordinate &x() const { return mX; }
    inline const Coordinate &y() const { return mY; }
    bool isRelative() const { return mX.isRelative() || mY.isRelative(); }
    QPoint toPoint(const WidgetData& widget) const;
    void setPoint(const WidgetData& widget, const QPoint& pos);

    QString toStr() const;
    void fromStr(const QString& str);

    inline bool operator==(const PositionAttr &other) {
        return mX == other.mX && mY == other.mY;
    }

private:
    Coordinate mX, mY;
};
Q_DECLARE_METATYPE(PositionAttr);

inline QString serialize(const PositionAttr &pos) {
    return pos.toStr();
}
inline void deserialize(const QString &str, PositionAttr &pos) {
    pos = PositionAttr(str);
}


#endif // POSITIONATTR_HPP
