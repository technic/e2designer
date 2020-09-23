#pragma once

#include <QVariant>
#include "skin/multicontent.hpp"

class QPainter;
class TemplatedContent;

namespace multicontent {
class EntryText;
class EntryPixmap;
class EntryProgress;
}

class Painter
{
public:
    Painter(QPainter* qtPainter, TemplatedContent content, QVariantList values);
    void paint();

    void accept(const multicontent::EntryText& e);
    void accept(const multicontent::EntryPixmap& e);
    void accept(const multicontent::EntryProgress& e);

private:
    QVariant getValue(int index);

    QPainter* painter;
    QVariantList values;
    TemplatedContent content;
};
