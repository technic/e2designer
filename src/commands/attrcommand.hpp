#ifndef ATTRCOMMAND_H
#define ATTRCOMMAND_H

#include <QAbstractItemModel>
#include <QUndoCommand>

QList<int> pathFromIndex(QModelIndex idx);
QModelIndex pathToIndex(QList<int> path, QAbstractItemModel* model);

class AttrCommand : public QUndoCommand
{
public:
    AttrCommand(QModelIndex idx, int key, QVariant value);

    void undo() override;
    void redo() override;

private:
    uint mId;
    int mKey;
    QVariant mOldValue;
    QVariant mValue;
    QAbstractItemModel* pModel;
};

#endif // ATTRCOMMAND_H
