#include "fontsmodel.hpp"
#include "repository/skinrepository.hpp"
#include <QCoreApplication>
#include <QDebug>
#include <QFontDatabase>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

// Font

Font::Font(const QString& name, const QString& fileName)
    : mName(name)
    , mFileName(fileName)
    , mFontId(-1)
{
    load();
}

QFont Font::font() const
{
    // FIXME: implement!
    return QFont();
}

void Font::fromXml(QXmlStreamReader& xml)
{
    Q_ASSERT(xml.isStartElement() && xml.name() == "font");
    mName = xml.attributes().value("name").toString();
    mFileName = xml.attributes().value("filename").toString();
    xml.skipCurrentElement();
    load();
}

void Font::toXml(QXmlStreamWriter& xml) const
{
    xml.writeStartElement("font");
    xml.writeAttribute("name", mName);
    xml.writeAttribute("filename", mFileName);
    xml.writeEndElement();
}

void Font::load()
{
    mFontId = -1;
    QString fname = mFileName;
    if (SkinRepository::instance().dir().exists(fname)) {
        // add font from skin
        fname = SkinRepository::instance().dir().filePath(fname);
    } else {
        // add font from default collection
        QDir dir = QDir(QCoreApplication::applicationDirPath());
        if (dir.exists(fname)) {
            fname = dir.filePath(fname);
        } else {
            qWarning() << "font file not found" << mFileName;
            return;
        }
    }
    mFontId = QFontDatabase::addApplicationFont(fname);
    if (mFontId < 0) {
        qWarning() << "invalid font file" << fname;
    }
}

// FontsList

void FontsList::fromXml(QXmlStreamReader& xml)
{
    Q_ASSERT(xml.isStartElement() && xml.name() == "fonts");

    removeItems(0, itemsCount());

    while (nextXmlChild(xml)) {
        if (xml.isStartElement()) {
            if (xml.name() != "font") {
                xml.skipCurrentElement();
                qWarning() << "unknown element in fonts";
                continue;
            }
            Font f;
            f.fromXml(xml);
            if (f.name().isEmpty() || f.name().isNull()) {
                qWarning() << "empty or null color name";
                continue;
            }
            if (canInsertItem(f)) {
                appendItem(f);
            } else {
                qWarning() << "duplicate font" << f.name();
                continue;
            }
        }
    }
}

void FontsList::toXml(QXmlStreamWriter& xml) const
{
    xml.writeStartElement("fonts");
    for (const Font& f : *this) {
        f.toXml(xml);
    }
    xml.writeEndElement();
}

// FontsModel

FontsModel::FontsModel(QObject* parent)
    : QAbstractTableModel(parent)
{}
QVariant FontsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
        case ColumnName:
            return "Name";
        case ColumnFile:
            return "Filename";
        case ColumnFont:
            return "Family";
        }
    }
    return QVariant();
}

int FontsModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return itemsCount();
}

int FontsModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent)
    return ColumnsCount;
}

QVariant FontsModel::data(const QModelIndex& index, int role) const
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
        case ColumnFile:
            return itemAt(index.row()).value();
        case ColumnFont:
            return itemAt(index.row()).font().family();
        }
    case Qt::FontRole:
        switch (index.column()) {
        case ColumnFont:
            return itemAt(index.row()).font();
        }
    }
    return QVariant();
}

bool FontsModel::setData(const QModelIndex& index, const QVariant& value, int role)
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
        case ColumnFile:
            changed = setItemValue(index.row(), value.toString());
            break;
        }
    }
    if (changed) {
        emit dataChanged(index.sibling(index.row(), 0), index.sibling(index.row(), ColumnsCount));
    }
    return changed;
}

Qt::ItemFlags FontsModel::flags(const QModelIndex& index) const
{
    Qt::ItemFlags flags = Qt::ItemIsEnabled;
    if (index.column() != ColumnFont) {
        flags |= Qt::ItemIsEditable | Qt::ItemIsSelectable;
    }
    return flags;
}

bool FontsModel::append(const Font& f)
{
    if (canInsertItem(f)) {
        beginInsertRows(QModelIndex(), itemsCount(), itemsCount());
        appendItem(f);
        endInsertRows();
        return true;
    }
    return false;
}

bool FontsModel::removeRows(int row, int count, const QModelIndex& parent)
{
    if (canRemoveItems(row, count) && count > 0) {
        beginRemoveRows(parent, row, row + count - 1);
        removeItems(row, count);
        endRemoveRows();
        return true;
    }
    return false;
}

void FontsModel::fromXml(QXmlStreamReader& xml)
{
    beginResetModel();
    FontsList::fromXml(xml);
    endResetModel();
}

void FontsModel::toXml(QXmlStreamWriter& xml) const
{
    FontsList::toXml(xml);
}
void FontsModel::emitValueChanged(const QString& name, const Font& value) const
{
    emit valueChanged(name, value);
}
