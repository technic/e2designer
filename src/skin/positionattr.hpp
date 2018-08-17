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

    Coordinate(Type type = Pixel, int value = 0);

    int anchor() const { return m_type; }
    /// Load and save text
    void parseStr(const QString& str);
    QString toStr() const;

    /// Convert to numerical value
    int getInt(int my_size, int parent_size) const;
    /// Try to convert back to smart value aka "center"
    void parseInt(int value, int my_size, int parent_size);

    /// Depends on parent or on size
    bool isRelative() const;

private:
    int m_type;
    int m_value;
};


class WidgetData;

typedef QPair<int, int> AnchorPair;

class PositionAttr
{
public:
    PositionAttr() {}
    PositionAttr(const QString& str) { fromStr(str); }
    bool isRelative() const { return mX.isRelative() || mY.isRelative(); }
    AnchorPair anchor() { return AnchorPair(mX.anchor(), mY.anchor()); }
    QPoint toPoint(const WidgetData& widget);
    void setPoint(const WidgetData& widget, const QPoint& pos);

    QString toStr() const;
    void fromStr(const QString& str);

private:
    Coordinate mX, mY;
};
Q_DECLARE_METATYPE(PositionAttr);


#endif // POSITIONATTR_HPP
