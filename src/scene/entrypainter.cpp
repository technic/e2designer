#include "entrypainter.hpp"
#include "parser/parser.hpp"
#include "repository/skinrepository.hpp"
#include <QPainter>

Painter::Painter(QPainter* qtPainter, TemplatedContent content, QVariantList values)
    : painter(qtPainter)
    , values(values)
    , content(content)
{}

void Painter::paint()
{
    for (auto& e : *content.entries) {
        e->visit(*this);
    }
}

void Painter::accept(const multicontent::EntryText& e)
{
    if (0 <= e.fontIdx && e.fontIdx < content.fonts.size()) {
        painter->setFont(content.fonts[e.fontIdx].getFont());
    }
    painter->drawText(QRect(e.pos.x, e.pos.y, e.size.x, e.size.y),
                      e.flags,
                      getValue(e.valueIdx).toString());
}

void Painter::accept(const multicontent::EntryPixmap& e)
{
    QString path = getValue(e.valueIdx).toString();
    QPixmap pixmap{ SkinRepository::instance().resolveFilename(path) };

    QRect dstRect(e.pos.x, e.pos.y, e.size.x, e.size.y);
    QRect srcRect;
    if (e.flags & multicontent::EntryPixmap::scaleOn) {
        srcRect = QRect(QPoint(0, 0), pixmap.size());
    } else {
        srcRect = QRect(QPoint(0, 0), dstRect.size());
    }

    painter->save();
    if (e.alpha == Property::Alphatest::blend || e.alpha == Property::Alphatest::on) {
        painter->setCompositionMode(QPainter::CompositionMode_SourceOver);
    }
    painter->drawPixmap(dstRect, pixmap, srcRect);
    painter->restore();
}

void Painter::accept(const multicontent::EntryProgress& e)
{
    int percent = qBound(0, getValue(e.valueIdx).toInt(), 100);
    QRect r{ e.pos.x, e.pos.y, e.size.x, e.size.y };
    r.setWidth(r.width() * percent / 100);
    painter->fillRect(r, QBrush());
}

QVariant Painter::getValue(int index)
{
    // Indexing in template code starts with 1
    index -= 1;
    if (0 <= index && index < values.size()) {
        return values[index];
    }
    return QVariant();
}
