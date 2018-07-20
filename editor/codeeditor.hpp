#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include <QPlainTextEdit>

class CodeEditor : public QPlainTextEdit
{
    Q_OBJECT

public:
    CodeEditor(QWidget* parent = nullptr);

    void lineNumberAreaPaintEvent(QPaintEvent* event);
    int lineNumberAreaWidth();

protected:
    void resizeEvent(QResizeEvent* event) override;

private slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void updateLineNumberArea(const QRect& rect, int dy);
    void highlightCurrentLine();

private:
    QWidget* m_numbers_area;
};

class LineNumberArea : public QWidget
{
public:
    LineNumberArea(CodeEditor* editor)
        : QWidget(editor)
        , m_editor(editor)
    {
    }
    QSize sizeHint() const override { return QSize(m_editor->lineNumberAreaWidth(), 0); }

protected:
    void paintEvent(QPaintEvent* event) override { m_editor->lineNumberAreaPaintEvent(event); }

private:
    CodeEditor* m_editor;
};

#endif // CODEEDITOR_H
