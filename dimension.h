#ifndef DIMENSION_H
#define DIMENSION_H

#include <QString>
#include <QSize>

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
    void parseStr(const QString &str);
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

#endif // DIMENSION_H
