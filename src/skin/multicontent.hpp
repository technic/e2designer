#pragma once

#include "skin/colorattr.hpp"
#include "skin/fontattr.hpp"
#include <vector>
#include <memory>

class Painter;

namespace multicontent {

struct Coordinate
{
    int x = 0;
    int y = 0;
    bool operator==(const Coordinate& other) const { return x == other.x && y == other.y; }
};

class Entry
{
public:
    virtual ~Entry() = default;

    Coordinate pos;
    Coordinate size;
    int valueIdx = 0;
    ColorAttr bgCol;
    ColorAttr bgColSelected;

    bool operator==(const Entry& other) const
    {
        return pos == other.pos && size == other.size && valueIdx == other.valueIdx;
    }

    virtual void visit(Painter& visitor) = 0;
};

}

class TemplatedContent
{
public:
    using vecType = std::vector<std::unique_ptr<multicontent::Entry>>;
    std::shared_ptr<vecType> entries{ std::make_shared<vecType>() };
    std::vector<FontAttr> fonts;
    int itemHeight;
};
