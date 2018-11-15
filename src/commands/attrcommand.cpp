#include "attrcommand.hpp"
#include "repository/skinrepository.hpp"

AttrCommand::AttrCommand(WidgetData *widget, int key, QVariant value, QUndoCommand *parent)
    : QUndoCommand(parent)
    , m_widget(widget)
    , m_key(key)
    , m_value(value)
{
    m_oldValue = m_widget->getAttr(m_key);

    int index = key + 1; // first key is invalid
    Q_ASSERT(Property::propertyEnum().value(index) == key);

    const QString name(Property::propertyEnum().key(index));
    setText(QString("%1 = %2").arg(name).arg(value.toString()));
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
    setText(QString("Move %1,%2")
            .arg(pos.toPoint().x())
            .arg(pos.toPoint().y()));
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
    m_point = static_cast<const MoveWidgetCommand *>(other)->m_point;
    return true;
}

ResizeWidgetCommand::ResizeWidgetCommand(WidgetData *widget, QSizeF size)
    : m_widget(widget)
    , m_size(size)
    , m_value(m_widget->size())
{
    setText(QString("Resize %1x%2")
            .arg(size.toSize().width())
            .arg(size.toSize().height()));
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
    m_size = static_cast<const ResizeWidgetCommand *>(other)->m_size;
    return true;
}
