#include "codeeditor.hpp"
#include <QPainter>
#include <QRect>
#include <QTextBlock>

CodeEditor::CodeEditor(QWidget* parent)
    : QPlainTextEdit(parent)
{
    m_numbers_area = new LineNumberArea(this);

    QObject::connect(this,
                     &QPlainTextEdit::blockCountChanged,
                     this,
                     &CodeEditor::updateLineNumberAreaWidth);
    QObject::connect(this, &QPlainTextEdit::updateRequest, this, &CodeEditor::updateLineNumberArea);
    QObject::connect(this,
                     &QPlainTextEdit::cursorPositionChanged,
                     this,
                     &CodeEditor::highlightCurrentLine);

    updateLineNumberAreaWidth(0);
    highlightCurrentLine();
}

void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent* event)
{
    QPainter painter(m_numbers_area);
    painter.fillRect(event->rect(), QColor(Qt::lightGray).lighter(120));
    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = int(blockBoundingGeometry(block).translated(contentOffset()).top());
    int bottom = top + int(blockBoundingRect(block).height());

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(Qt::black);
            painter.drawText(
              0, top, m_numbers_area->width() - 2, fontMetrics().height(), Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + int(blockBoundingRect(block).height());
        ++blockNumber;
    }
}

int CodeEditor::lineNumberAreaWidth()
{
    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }
    int space = 4 + fontMetrics().width(QLatin1Char('9')) * digits;
    return space;
}

void CodeEditor::resizeEvent(QResizeEvent* event)
{
    QPlainTextEdit::resizeEvent(event);

    QRect cr = contentsRect();
    m_numbers_area->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void CodeEditor::updateLineNumberAreaWidth(int newBlockCount)
{
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
    Q_UNUSED(newBlockCount);
}

void CodeEditor::updateLineNumberArea(const QRect& rect, int dy)
{
    if (dy)
        m_numbers_area->scroll(0, dy);
    else
        m_numbers_area->update(0, rect.y(), m_numbers_area->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth(0);
}

void CodeEditor::highlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;

        QColor lineColor = QColor(Qt::yellow).lighter(160);

        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }

    setExtraSelections(extraSelections);
}
