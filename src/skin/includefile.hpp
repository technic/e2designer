#pragma once

#include "widgetdata.hpp"

class IncludeFile : public WidgetData
{
public:
    static constexpr char tag[] = "include";

    IncludeFile();

    bool fromXml(QXmlStreamReader& xml);

    void toXml(XmlStreamWriter& xml) const override;

    QString getAttr(const QString& key) const override;

private:
    QString m_fileName;
    void read(const QString& fileName);
};
