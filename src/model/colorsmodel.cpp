#include "colorsmodel.hpp"
#include <QColor>
#include <QDebug>
#include <QMimeData>
#include <QDataStream>

// Color

Color::Color(const QString& name, QRgb value)
    : m_name(name)
    , m_value(value)
{}
QString Color::valueStr() const
{
    return QColor::fromRgba(m_value).name(QColor::HexArgb);
}
void Color::fromXml(QXmlStreamReader& xml)
{
    Q_ASSERT(xml.isStartElement() && xml.name() == "color");

    m_name = xml.attributes().value("name").toString();
    QColor color(xml.attributes().value("value").toString());
    // enigma2 has inverted alpha representation
    color.setAlpha(255 - color.alpha());
    m_value = color.rgba();
    xml.skipCurrentElement();
}

void Color::toXml(QXmlStreamWriter& xml) const
{
    xml.writeStartElement("color");
    xml.writeAttribute("name", m_name);
    QColor color = QColor::fromRgba(m_value);
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
    : MovableListModel(parent)
{}
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
    if (parent.isValid())
        return 0;
    return itemsCount();
}

int ColorsModel::columnCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return 0;
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
        auto sibling = [index](int col) { return index.sibling(index.row(), col); };
        emit dataChanged(sibling(ColumnName), sibling(ColumnName));
        emit dataChanged(sibling(ColumnValue), sibling(ColumnValue));
        emit dataChanged(sibling(ColumnColor), sibling(ColumnColor));
    }
    return changed;
}

Qt::ItemFlags ColorsModel::flags(const QModelIndex& index) const
{
    Qt::ItemFlags flags = MovableListModel::flags(index);
    if (index.column() != ColumnColor) {
        flags |= Qt::ItemIsEditable;
    }
    return flags;
}

bool ColorsModel::insert(int row, const Color& item)
{
    if (canInsertItem(item)) {
        beginInsertRows(QModelIndex(), row, row);
        insertItem(row, item);
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

bool ColorsModel::moveRows(const QModelIndex& sourceParent,
                           int sourceRow,
                           int count,
                           const QModelIndex& destinationParent,
                           int destinationChild)
{
    if (!canMoveItems(sourceRow, count, destinationChild)) {
        return false;
    }
    bool allowed = beginMoveRows(
      sourceParent, sourceRow, sourceRow + count - 1, destinationParent, destinationChild);
    if (allowed) {
        moveItems(sourceRow, count, destinationChild);
        endMoveRows();
        return true;
    }
    qWarning() << "Requested move is not allowed!";
    return false;
}

void ColorsModel::fromXml(QXmlStreamReader& xml)
{
    beginResetModel();
    ColorsList::fromXml(xml);
    endResetModel();
}

void ColorsModel::emitValueChanged(const QString& name, const Color& value) const
{
    emit valueChanged(name, value.value());
}
