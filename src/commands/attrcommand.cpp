#include "attrcommand.hpp"
#include "skin/widgetdata.hpp"

AttrCommand::AttrCommand(WidgetData *widget, int key, const QVariant& value, QUndoCommand *parent)
    : QUndoCommand(parent)
    , m_widget(widget)
    , m_key(key)
    , m_value(value)
{
    m_oldValue = m_widget->getAttr(m_key);

    int index = key + 1; // first key is invalid
    Q_ASSERT(Property::propertyEnum().value(index) == key);

    const QString name(Property::propertyEnum().key(index));
    setText(QString("%1 = %2").arg(name, value.toString()));
}

void AttrCommand::redo()
{
    m_widget->setAttr(m_key, m_value);
}

void AttrCommand::undo()
{
    m_widget->setAttr(m_key, m_oldValue);
}

QVector<int> pathFromIndex(QModelIndex idx)
{
    QVector<int> path;
    while (idx.isValid()) {
        path.prepend(idx.row());
        idx = idx.parent();
    }
    return path;
}

QModelIndex pathToIndex(QVector<int> path, QAbstractItemModel* model)
{
    QModelIndex idx;
    for (auto p : path) {
        idx = model->index(p, 0, idx);
    }
    return idx;
}

MoveWidgetCommand::MoveWidgetCommand(WidgetData *widget, QPointF pos)
    : m_widget(widget)
    , m_point(pos)
    , m_pos(m_widget->position())
{
    updateText();
}

void MoveWidgetCommand::redo()
{
    m_widget->move(m_point);
}

void MoveWidgetCommand::undo()
{
    m_widget->setPosition(m_pos);
}

bool MoveWidgetCommand::mergeWith(const QUndoCommand *other)
{
    if (id() != other->id()) {
        return false;
    }
    auto otherCommand = static_cast<const MoveWidgetCommand *>(other);
    if (otherCommand->m_widget == m_widget) {
        m_point = otherCommand->m_point;
        updateText();
        return true;
    }
    return false;
}

void MoveWidgetCommand::updateText()
{
    setText(QString("Move %1,%2")
            .arg(m_point.toPoint().x())
            .arg(m_point.toPoint().y()));
}

ResizeWidgetCommand::ResizeWidgetCommand(WidgetData *widget, QSizeF size)
    : m_widget(widget)
    , m_size(size)
    , m_value(m_widget->size())
{
    updateText();
}

void ResizeWidgetCommand::redo()
{
    m_widget->resize(m_size);
}

void ResizeWidgetCommand::undo()
{
    m_widget->setSize(m_value);
}

bool ResizeWidgetCommand::mergeWith(const QUndoCommand *other)
{
    if (id() != other->id()) {
        return false;
    }
    auto otherCommand = static_cast<const ResizeWidgetCommand *>(other);
    if (otherCommand->m_widget == m_widget) {
        m_size = otherCommand->m_size;
        updateText();
        return true;
    }
    return false;
}

void ResizeWidgetCommand::updateText()
{
    setText(QString("Resize %1x%2")
            .arg(m_size.toSize().width())
            .arg(m_size.toSize().height()));
}
