#pragma once

#include "repository/xmlnode.hpp"
#include "skin/enums.hpp"

class QXmlStreamReader;
class QXmlStreamWriter;

/**
 * @brief Stores (border position, pixmap file name) pair defined in skin
 */
class Border : public XmlData
{
public:
    enum
    {
        bpInvalid = -1
    };

    Border();

    void fromXml(QXmlStreamReader& xml);
    void toXml(QXmlStreamWriter& xml) const;

    inline int bp() const { return m_bp; }
    inline bool isValid() const { return m_bp != bpInvalid; }
    inline QString fileName() const { return m_fname; }
    void reset();

private:
    int m_bp;
    QString m_fname;
};

class BorderSet : public XmlData
{
public:
    enum
    {
        bsInvalid = -1
    };

    BorderSet();
    static int count() { return QMetaEnum::fromType<Property::BorderPosition>().keyCount(); }

    void fromXml(QXmlStreamReader& xml);
    void toXml(QXmlStreamWriter& xml) const;

    const Border& getBorder(Property::BorderPosition bp) const;

private:
    int m_bs;
    QVector<Border> m_borders;
};
