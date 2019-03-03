#include "movablelistmodel.hpp"
#include <QMimeData>
#include <QDataStream>

MovableListModel::MovableListModel(QObject* parent)
    : QAbstractTableModel(parent)
{}

Qt::ItemFlags MovableListModel::flags(const QModelIndex& index) const
{
    Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsDragEnabled | Qt::ItemIsSelectable;
    if (!index.isValid())
        flags |= Qt::ItemIsDropEnabled;
    return flags;
}

QMimeData* MovableListModel::mimeData(const QModelIndexList& indexes) const
{
    if (indexes.count() <= 0)
        return nullptr;

    auto types = mimeTypes();
    Q_ASSERT(types.count() > 0);
    QString format = types.at(0);

    // Store list of rows in the QMimeData
    QMimeData* data = new QMimeData();
    QByteArray encoded;
    QDataStream stream(&encoded, QIODevice::WriteOnly);
    encodeRows(indexes, stream);
    data->setData(format, encoded);
    return data;
}

bool MovableListModel::dropMimeData(const QMimeData* data,
                                    Qt::DropAction action,
                                    int row,
                                    int column,
                                    const QModelIndex& parent)
{
    if (action == Qt::IgnoreAction)
        return true;
    if (action != Qt::MoveAction)
        return false;

    auto types = mimeTypes();
    Q_ASSERT(types.count() > 0);
    QString format = types.at(0);

    // Decode list of rows to move within the model
    Q_UNUSED(column);
    QByteArray encoded = data->data(format);
    QDataStream stream(&encoded, QIODevice::ReadOnly);
    QList<int> rows = decodeRows(stream);

    bool ok = true;
    auto root = QModelIndex();
    for (auto sRow : qAsConst(rows)) {
        ok |= moveRows(root, sRow, 1, parent, row);
    }
    // Default implemententation removes successfully moved out rows, return false to disable it.
    return false;
}

Qt::DropActions MovableListModel::supportedDropActions() const
{
    return Qt::MoveAction | Qt::CopyAction;
}

void MovableListModel::encodeRows(const QModelIndexList& indexes, QDataStream& stream) const
{
    QList<int> rows;
    for (const auto& index : qAsConst(indexes)) {
        auto r = index.row();
        if (!rows.contains(r)) {
            rows.append(r);
        }
    }
    for (const int r : rows) {
        stream << r;
    }
}

QList<int> MovableListModel::decodeRows(QDataStream& stream) const
{
    QList<int> rows;
    while (!stream.atEnd()) {
        int r;
        stream >> r;
        rows.append(r);
    }
    return rows;
}
