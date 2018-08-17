#ifndef BORDERSET_H
#define BORDERSET_H

#include "repository/xmlnode.hpp"

class QXmlStreamReader;
class QXmlStreamWriter;

/**
 * @brief Stores (border position, pixmap file name) pair defined in skin
 */
class Border : public XmlData
{
public:
    enum { bpInvalid = -1 };

    Border();

    void fromXml(QXmlStreamReader& xml);
    void toXml(QXmlStreamWriter& xml) const;

    inline int bp() const { return m_bp; }
    inline bool isValid() const { return m_bp != bpInvalid; }
    void reset();

private:
    int m_bp;
    QString m_fname;
};

class BorderSet : public XmlData
{
public:
    enum { bsInvalid = -1 };

    BorderSet();

    void fromXml(QXmlStreamReader& xml);
    void toXml(QXmlStreamWriter& xml) const;

    const Border& getBorder(int bp);

private:
    int m_bs;
    QVector<Border> m_borders;
};

#endif // BORDERSET_H
