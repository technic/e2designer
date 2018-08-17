#ifndef SIZEATTR_HPP
#define SIZEATTR_HPP

#include <QSize>
#include <QString>
#include <QMetaType>

class Dimension
{
public:
    Dimension();
    enum {
        Fill, // TODO: implement this
        Percent, // TODO: implement this
        Number,
    };

    /// Load and save text
    void parseStr(const QString& str);
    QString toStr() const;

    /// Convert to numerical value
    int getInt(int parent_size) const;
    /// Force set numerical value
    void setInt(int value);
    /// Try to convert back to percent if needed
    void parseInt(int value, int parent_size);

    /// Depends on parent or on size
    bool isRelative() const;

private:
    int m_type;
    int m_value;
};

class WidgetData;

class SizeAttr
{
public:
    SizeAttr(Dimension w = Dimension(), Dimension h = Dimension())
        : mWidth(w)
        , mHeight(h)
    {
    }
    SizeAttr(const QString& str) { fromStr(str); }
    bool isRelative() const { return mWidth.isRelative() || mHeight.isRelative(); }
    QSize getSize(const WidgetData& widget);
    void setSize(const WidgetData& widget, const QSize size);

    QString toStr() const;
    void fromStr(const QString& str);

private:
    Dimension mWidth, mHeight;
};
Q_DECLARE_METATYPE(SizeAttr);


#endif // SIZEATTR_HPP
