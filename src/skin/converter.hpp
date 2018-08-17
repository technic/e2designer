#ifndef CONVERTER_HPP
#define CONVERTER_HPP

#include <QString>

class QXmlStreamReader;
class QXmlStreamWriter;

class Converter
{
public:
    void fromXml(QXmlStreamReader& xml);
    void toXml(QXmlStreamWriter& xml) const;

private:
    QString mType;
    QString mText;
};

#endif // CONVERTER_HPP
