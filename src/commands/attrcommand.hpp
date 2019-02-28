#ifndef ATTRCOMMAND_H
#define ATTRCOMMAND_H

#include "skin/positionattr.hpp"
#include "skin/sizeattr.hpp"
#include "base/typelist.hpp"
#include <QAbstractItemModel>
#include <QUndoCommand>
#include <QPoint>
#include <QSize>

QVector<int> pathFromIndex(QModelIndex idx);
QModelIndex pathToIndex(QVector<int> path, QAbstractItemModel* model);

class AttrCommand;
class MoveWidgetCommand;
class ResizeWidgetCommand;
using CommandClasses = TypeList<AttrCommand, MoveWidgetCommand, ResizeWidgetCommand>;

class AttrCommand : public QUndoCommand
{
public:
    AttrCommand(WidgetData* m_widget,
                int key,
                const QVariant& value,
                QUndoCommand* parent = nullptr);
    void redo() final;
    void undo() final;

private:
    WidgetData* m_widget;
    int m_key;
    QVariant m_oldValue;
    QVariant m_value;
};

class MoveWidgetCommand : public QUndoCommand
{
public:
    MoveWidgetCommand(WidgetData* widget, QPointF pos);
    int id() const final { return CommandClasses::getIndex(this); }
    void redo() final;
    void undo() final;
    bool mergeWith(const QUndoCommand* other) final;

private:
    void updateText();
    WidgetData* m_widget;
    QPointF m_point;
    PositionAttr m_pos;
};

class ResizeWidgetCommand : public QUndoCommand
{
public:
    ResizeWidgetCommand(WidgetData* widget, QSizeF size);
    int id() const final { return CommandClasses::getIndex(this); }
    void redo() final;
    void undo() final;
    bool mergeWith(const QUndoCommand* other) final;

private:
    void updateText();
    WidgetData* m_widget;
    QSizeF m_size;
    SizeAttr m_value;
};

#endif // ATTRCOMMAND_H
