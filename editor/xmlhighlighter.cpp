#include <QRegExp>
#include <QTextCharFormat>
#include "xmlhighlighter.hpp"

XMLHighlighter::XMLHighlighter(QTextDocument *parent) :
    QSyntaxHighlighter(parent)
{
    elementFormat.setFontWeight(QFont::Bold);
    attrNameFormat.setForeground(Qt::darkGreen);
    attrValueFormat.setForeground(Qt::darkRed);
    commentFormat.setForeground(Qt::gray);
}

void XMLHighlighter::highlightBlock(const QString &text)
{

    int state = previousBlockState();
    int offset = 0;

    for (int i=0; i < text.length(); ++i) {

        switch (state) {
        case Element:
        case AttrName:
            if (text.mid(i,2) == "/>") {
                setFormat(offset, i - offset, getFormat(state));
                setFormat(i, 2, elementFormat);
                offset = i + 2;
                i++;
                state = Normal;
                continue;
            }
            else if (text.at(i) == '>') {
                setFormat(offset, i - offset, getFormat(state));
                setFormat(i, 1, elementFormat);
                offset = i + 1;
                state = Normal;
                continue;
            }
            break;
            // go to next switch
        }

        switch (state) {
        case Element:
            if (text.at(i) == ' ') {
                setFormat(offset, i - offset, elementFormat);
                state = AttrName;
                offset = i + 1;
            }
            break;
        case AttrName:
            if (text.at(i) == '\'') {
                state = AttrValue1;
                setFormat(offset, i - offset, attrNameFormat);
                offset = i;
            }
            if (text.at(i) == '"') {
                state = AttrValue2;
                setFormat(offset, i - offset, attrNameFormat);
                offset = i;
            }
            break;
        case AttrValue1:
            if (text.at(i) == '\'') {
                state = AttrName;
                setFormat(offset, i - offset + 1, attrValueFormat);
                offset = i + 1;
            }
            break;
        case AttrValue2:
            if (text.at(i) == '"') {
                state = AttrName;
                setFormat(offset, i - offset + 1, attrValueFormat);
                offset = i + 1;
            }
            break;
        case Comment:
            if (text.mid(i, 3) == "-->") {
                state = Normal;
                setFormat(offset, i - offset + 3, commentFormat);
                offset = i + 3;
            }
            break;
        default: // Normal
            if (text.mid(i, 4) == "<!--") {
                state = Comment;
                offset = i;
            }
            else if (text.at(i) == '<') {
                state = Element;
                offset = i;
            }
            break;
        }
    }
    setFormat(offset, text.length() - offset, getFormat(state));
    setCurrentBlockState(state);
}


