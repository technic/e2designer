#ifndef XMLHIGHLIGHTER_H
#define XMLHIGHLIGHTER_H

#include <QSyntaxHighlighter>

class XMLHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    XMLHighlighter(QTextDocument *parent);

protected:
    void highlightBlock(const QString &text) override;

private:
    enum {
        Normal = -1, // default
        Element,
        AttrName,
        AttrValue1,
        AttrValue2,
        Comment,
    };

    QTextCharFormat getFormat(int type)
    {
        switch (type) {
        case Element:
            return elementFormat;
        case AttrName:
            return attrNameFormat;
        case AttrValue1:
            return attrValueFormat;
        case AttrValue2:
            return attrValueFormat;
        case Comment:
            return commentFormat;
        default:
            // Normal
            return QTextCharFormat();
        }
    }

    QTextCharFormat elementFormat;
    QTextCharFormat attrNameFormat;
    QTextCharFormat attrValueFormat;
    QTextCharFormat commentFormat;
};

#endif // XMLHIGHLIGHTER_H
