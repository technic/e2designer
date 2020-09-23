#pragma once

#include <memory>
#include <vector>
#include <unordered_map>
#include <QString>
#include "base/singleton.hpp"
#include "skin/colorattr.hpp"
#include "skin/fontattr.hpp"
#include "skin/enums.hpp"
#include "scene/entrypainter.hpp"
#include "skin/multicontent.hpp"

namespace multicontent {

class EntryText : public Entry
{
public:
    int fontIdx = 0;
    int flags = Qt::AlignLeft | Qt::AlignTop;

    bool operator==(const EntryText& other) const
    {
        return Entry::operator==(other) && fontIdx == other.fontIdx && flags == other.flags;
    }

    void visit(Painter& visitor) override { visitor.accept(*this); }
};

class EntryPixmap : public Entry
{
public:
    int flags = 0;
    ColorAttr bgCol;
    ColorAttr bgColSelected;
    Property::Alphatest alpha = Property::Alphatest::off;

    enum Scale
    {
        scaleOff = 0,
        scaleOn = 1,
    };

    explicit EntryPixmap(Property::Alphatest alpha = Property::Alphatest::off)
        : alpha(alpha)
    {}

    bool operator==(const EntryPixmap& other) const
    {
        return Entry::operator==(other) && flags == other.flags && alpha == other.alpha
               && bgCol == other.bgCol && bgColSelected == other.bgColSelected;
    }

    void visit(Painter& visitor) override { visitor.accept(*this); }
};

class EntryProgress : public Entry
{
public:
    int borderWidth;
    void visit(Painter& visitor) override { visitor.accept(*this); }
};

class EntryProgressPixmap : public EntryProgress
{
public:
    std::string pixmap;
    void visit(Painter& visitor) override { visitor.accept(*this); }
};

} // multicontent

namespace parser {
using Context = std::unordered_map<std::string_view, long>;
}

class PseudoPythonParser
{
public:
    PseudoPythonParser();
    std::vector<std::unique_ptr<multicontent::Entry>> parse(const QString& s);
    TemplatedContent parseFull(const QString& s);
    static const parser::Context ctx;
};
