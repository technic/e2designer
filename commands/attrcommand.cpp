#include "attrcommand.h"
#include "repository/skinrepository.h"

AttrCommand::AttrCommand(QModelIndex idx, int key, QVariant value)
    : mKey(key)
    , mValue(value)
{
    mId = idx.data(ScreensModel::IdRole).toUInt();
    mOldValue = SkinRepository::screens()->getWidgetAttr(idx, key, Roles::DataRole);
}

void AttrCommand::undo()
{
//    SkinRepository::screens()->setWidgetAttr(mId, mKey, mOldValue, Roles::DataRole);
}

void AttrCommand::redo()
{
//    SkinRepository::screens()->setWidgetAttr(mId, mKey, mValue, Roles::DataRole);
}

QList<int> pathFromIndex(QModelIndex idx)
{
    QList<int> path;
    while (idx.isValid()) {
        path.prepend(idx.row());
        idx = idx.parent();
    }
    return path;
}

QModelIndex pathToIndex(QList<int> path, QAbstractItemModel *model)
{
    QModelIndex idx;
    for (auto p: path) {
        idx = model->index(p, 0, idx);
    }
    return idx;
}
