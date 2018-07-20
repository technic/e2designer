#ifndef XMLNODE_H
#define XMLNODE_H

#include <QXmlStreamReader>
#include <QXmlStreamWriter>

/**
 * @brief Abstract xml node
 */
class XmlData
{
protected:
    XmlData();
    bool nextXmlChild(QXmlStreamReader& xml);

    //    virtual void toXml(QXmlStreamWriter &xml) const;

private:
};

class RawXmlElement : public XmlData
{
public:
    RawXmlElement()
        : XmlData()
    {
    }
    void toXml(QXmlStreamWriter& xml) const;
    void fromXml(QXmlStreamReader& xml);

private:
    QString mTag;
    QXmlStreamAttributes mAttrs;
    QVector<RawXmlElement> mChilds;
};

#endif // XMLNODE_H
