#ifndef WINDOWSTYLE_H
#define WINDOWSTYLE_H

#include "attr/coloritem.h"
#include "attr/fontitem.h"
#include "attr/positionitem.h"
#include "borderset.h"
#include "repository/xmlnode.h"


class WindowStyleTitle
{
public:
    void fromXml(QXmlStreamReader &xml);
    void toXml(QXmlStreamWriter &xml) const;

    PositionAttr position;
    FontAttr font;
};


class WindowStyleColor
{
public:
    void fromXml(QXmlStreamReader &xml);
    void toXml(QXmlStreamWriter &xml) const;

    QString name;
    ColorAttr color;
};


class WindowStyle: public XmlData
{
public:
    WindowStyle();
    QString name() const { return QString::number(m_id); }

    void fromXml(QXmlStreamReader &xml);
    void toXml(QXmlStreamWriter &xml) const;

private:
    QString m_type;
    int m_id;
    WindowStyleTitle m_title;
    QVector<WindowStyleColor> m_colors;
    BorderSet m_borderSet;
};


class WindowStylesList
{
public:
    void append(WindowStyle style);

    // Xml:
    void toXml(QXmlStreamWriter &xml) const;

protected:
//    void emitValueChanged(const QString &name, const WindowStyle &value) const final;
    QVector<WindowStyle> mItems;
};

#endif // WINDOWSTYLE_H
