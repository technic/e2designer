#include "xmlstreamwriter.hpp"

void XmlStreamWriter::writeQuotedCharacters(const QString& text)
{
    auto parts = text.split('"');
    for (const QString& part : parts) {
        writeCharacters(part);
        if (part != parts.back()) {
            // XML allows to have unquoted charectes inside the element
            device()->putChar('"');
        }
    }
}
