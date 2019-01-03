#include "colorsmodel.hpp"
#include <QColor>
#include <QDebug>
#include <QMimeData>
#include <QDataStream>

// Color

Color::Color(const QString& name, QRgb value)
    : mName(name)
    , mValue(value)
{
}
QString Color::valueStr() const
{
    return QColor::fromRgba(mValue).name(QColor::HexArgb);
}
void Color::fromXml(QXmlStreamReader& xml)
{
    Q_ASSERT(xml.isStartElement() && xml.name() == "color");

    mName = xml.attributes().value("name").toString();
    QColor color(xml.attributes().value("value").toString());
    // enigma2 has inverted alpha representation
    color.setAlpha(255 - color.alpha());
    mValue = color.rgba();
    xml.skipCurrentElement();
}

void Color::toXml(QXmlStreamWriter& xml) const
{
    xml.writeStartElement("color");
    xml.writeAttribute("name", mName);
    QColor color = QColor::fromRgba(mValue);
    // enigma2 has inverted alpha representation
    color.setAlpha(255 - color.alpha());
    xml.writeAttribute("value", color.name(QColor::HexArgb));
    xml.writeEndElement();
}

// ColorsList

void ColorsList::fromXml(QXmlStreamReader& xml)
{
    Q_ASSERT(xml.isStartElement() && xml.name() == "colors");

    removeItems(0, itemsCount());

    while (nextXmlChild(xml)) {
        if (xml.isStartElement()) {
            if (xml.name() != "color") {
                xml.skipCurrentElement();
                qWarning() << "unknown element in colors";
                continue;
            }
            Color c;
            c.fromXml(xml);
            if (c.name().isEmpty()) {
                qWarning() << "empty color name";
                continue;
            }
            if (canInsertItem(c)) {
                appendItem(c);
            } else {
                qWarning() << "duplicate color" << c.name();
                continue;
            }
        }
    }
}

void ColorsList::toXml(QXmlStreamWriter& xml) const
{
    xml.writeStartElement("colors");
    for (const Color& item : *this) {
        item.toXml(xml);
    }
    xml.writeEndElement();
}

// ColorsModel

ColorsModel::ColorsModel(QObject* parent)
    : QAbstractTableModel(parent)
    , ColorsList()
{
}
QVariant ColorsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
        case ColumnName:
            return "Name";
        case ColumnValue:
            return "Hex argb";
        case ColumnColor:
            return "Preview";
        }
    }
    return QVariant();
}

int ColorsModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return itemsCount();
}

int ColorsModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return ColumnsCount;
}

QVariant ColorsModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (!hasIndex(index.row(), index.column(), index.parent()))
        return QVariant();

    switch (role) {
    case Qt::DisplayRole:
    case Qt::EditRole:
        switch (index.column()) {
        case ColumnName:
            return itemAt(index.row()).name();
        case ColumnValue:
            return itemAt(index.row()).valueStr();
        case ColumnColor:
            if (role == Qt::EditRole)
				return QColor::fromRgba(itemAt(index.row()).value());
        }
        break;
    case Qt::BackgroundColorRole:
        switch (index.column()) {
        case ColumnColor:
            return QColor(itemAt(index.row()).value());
        }
    }
    return QVariant();
}

bool ColorsModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (!index.isValid())
        return false;

    if (!hasIndex(index.row(), index.column(), index.parent()))
        return false;

    bool changed = false;
    switch (role) {
    case Qt::EditRole:
        switch (index.column()) {
        case ColumnName:
            changed = setItemName(index.row(), value.toString());
            break;
        case ColumnValue:
            changed = setItemValue(index.row(), QColor(value.toString()).rgba());
            break;
        case ColumnColor:
            changed = setItemValue(index.row(), qvariant_cast<QColor>(value).rgba());
            break;
        }
    }
    if (changed) {
        auto sibling = [index](int col) {
            return index.sibling(index.row(), col); };
        emit dataChanged(sibling(ColumnName), sibling(ColumnName));
        emit dataChanged(sibling(ColumnValue), sibling(ColumnValue));
        emit dataChanged(sibling(ColumnColor), sibling(ColumnColor));
    }
    return changed;
}

Qt::ItemFlags ColorsModel::flags(const QModelIndex& index) const
{
    Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsDragEnabled | Qt::ItemIsSelectable;
    if (!index.isValid())
        flags |= Qt::ItemIsDropEnabled;
    if (index.column() != ColumnColor) {
        flags |= Qt::ItemIsEditable;
    }
    return flags;
}

bool ColorsModel::insert(int row, const Color& c)
{
    if (canInsertItem(c)) {
        beginInsertRows(QModelIndex(), row, row);
        insertItem(row, c);
        endInsertRows();
        return true;
    }
    return false;
}

bool ColorsModel::append(const Color& c)
{
    if (canInsertItem(c)) {
        beginInsertRows(QModelIndex(), itemsCount(), itemsCount());
        appendItem(c);
        endInsertRows();
        return true;
    }
    return false;
}

/// Insert blank items to the table
bool ColorsModel::insertRows(int row, int count, const QModelIndex& parent)
{
    // Can only insert to root
    if (parent.isValid()) {
        return false;
    }
    if (row < 0 || row > rowCount(parent)) {
        return false;
    }
    beginInsertRows(parent, row, row + count - 1);
    for (int i = 0; i < count; ++i) {
        bool ok = insertItem(row, Color());
        Q_ASSERT(ok);
    }
    endInsertRows();
    return true;
}

bool ColorsModel::removeRows(int row, int count, const QModelIndex& parent)
{
    if (canRemoveItems(row, count) && count > 0) {
        beginRemoveRows(parent, row, row + count - 1);
        removeItems(row, count);
        endRemoveRows();
        return true;
    }
    return false;
}

bool ColorsModel::moveRows(const QModelIndex& sourceParent, int sourceRow, int count,
                           const QModelIndex& destinationParent, int destinationChild)
{
    if (!canMoveItems(sourceRow, count, destinationChild)) {
        return false;
    }
    bool allowed = beginMoveRows(sourceParent, sourceRow, sourceRow + count - 1,
                                 destinationParent, destinationChild);
    if (!allowed) {
        qWarning() << "Requested move is not allowed!";
        return false;
    }
    moveItems(sourceRow, count, destinationChild);
    endMoveRows();
    return true;
}

QMimeData* ColorsModel::mimeData(const QModelIndexList& indexes) const
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

bool ColorsModel::dropMimeData(const QMimeData* data, Qt::DropAction action,
                               int row, int column, const QModelIndex& parent)
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
    for (auto sRow: qAsConst(rows)) {
        ok |= moveRows(root, sRow, 1, parent, row);
    }
    // Default implemententation removes successfully moved out rows, return false to disable it.
    return false;
}

Qt::DropActions ColorsModel::supportedDropActions() const
{
    return Qt::MoveAction | Qt::CopyAction;
}

void ColorsModel::fromXml(QXmlStreamReader& xml)
{
    beginResetModel();
    ColorsList::fromXml(xml);
    endResetModel();
}

void ColorsModel::toXml(QXmlStreamWriter& xml) const
{
    ColorsList::toXml(xml);
}
void ColorsModel::emitValueChanged(const QString& name, const Color& value) const
{
    emit valueChanged(name, value.value());
}

void ColorsModel::encodeRows(const QModelIndexList& indexes, QDataStream& stream) const
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

QList<int> ColorsModel::decodeRows(QDataStream& stream) const
{
    QList<int> rows;
    while (!stream.atEnd()) {
        int r;
        stream >> r;
        rows.append(r);
    }
    return rows;
}
