#include "colorsmodel.hpp"
#include <QColor>
#include <QDebug>

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
            return QString();
        }
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
        }
    }
    if (changed) {
        emit dataChanged(index.sibling(index.row(), 0), index.sibling(index.row(), ColumnsCount));
    }
    return changed;
}

Qt::ItemFlags ColorsModel::flags(const QModelIndex& index) const
{
    Qt::ItemFlags flags = Qt::ItemIsEnabled;
    if (index.column() != ColumnColor) {
        flags |= Qt::ItemIsEditable | Qt::ItemIsSelectable;
    }
    return flags;
}

bool ColorsModel::append(Color c)
{
    if (canInsertItem(c)) {
        beginInsertRows(QModelIndex(), itemsCount(), itemsCount());
        appendItem(c);
        endInsertRows();
        return true;
    }
    return false;
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
