#include "outputsmodel.hpp"
#include <QDebug>

VideoOutputRepository::VideoOutputRepository() = default;
void VideoOutputRepository::addFromXml(QXmlStreamReader& xml)
{
    VideoOutput out;
    out.fromXml(xml);
    appendItem(out);
}

void VideoOutputRepository::toXml(QXmlStreamWriter& xml) const
{
    for (const VideoOutput& output : *this) {
        output.toXml(xml);
    }
}

VideoOutput::VideoOutput(const QString& id, const VideoOutputData& data)
    : m_xres(data.resolution.width())
    , m_yres(data.resolution.height())
    , m_bpp(data.bpp)
    , m_id(id.toInt())
{}

void VideoOutput::fromXml(QXmlStreamReader& xml)
{
    Q_ASSERT(xml.isStartElement() && xml.name() == "output");
    m_id = xml.attributes().value("id").toInt();

    while (nextXmlChild(xml)) {
        if (xml.isStartElement()) {
            if (xml.name() != "resolution") {
                qWarning() << "unknown tag" << xml.name();
                xml.skipCurrentElement();
                continue;
            }
            m_xres = xml.attributes().value("xres").toInt();
            m_yres = xml.attributes().value("yres").toInt();
            m_bpp = xml.attributes().value("bpp").toInt();
        }
    }
    xml.skipCurrentElement();
}

void VideoOutput::toXml(QXmlStreamWriter& xml) const
{
    xml.writeStartElement("output");
    xml.writeAttribute("id", QString::number(m_id));

    xml.writeStartElement("resolution");
    xml.writeAttribute("xres", QString::number(m_xres));
    xml.writeAttribute("yres", QString::number(m_yres));
    xml.writeAttribute("bpp", QString::number(m_bpp));
    xml.writeEndElement();

    xml.writeEndElement();
}

OutputsModel::OutputsModel(QObject* parent)
    : MovableListModel(parent)
{}

QVariant OutputsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
        case ColumnId:
            return "Id";
        case ColumnXRes:
            return "xres";
        case ColumnYRes:
            return "yres";
        case ColumnBpp:
            return "bpp";
        }
    }
    return QVariant();
}

int OutputsModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return 0;
    return itemsCount();
}

int OutputsModel::columnCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return 0;
    return ColumnsCount;
}

QVariant OutputsModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (!hasIndex(index.row(), index.column(), index.parent()))
        return QVariant();

    switch (role) {
    case Qt::DisplayRole:
    case Qt::EditRole:
        switch (index.column()) {
        case ColumnId:
            return itemAt(index.row()).id();
        case ColumnXRes:
            return itemAt(index.row()).xres();
        case ColumnYRes:
            return itemAt(index.row()).yres();
        case ColumnBpp:
            return itemAt(index.row()).bpp();
        }
    }
    return QVariant();
}

bool OutputsModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (!index.isValid())
        return false;

    if (!hasIndex(index.row(), index.column(), index.parent()))
        return false;

    bool changed = false;
    switch (role) {
    case Qt::EditRole:
        if (index.column() == ColumnId) {
            changed = setItemName(index.row(), value.toString());
        } else {
            auto data = itemAt(index.row()).value();
            switch (index.column()) {
            case ColumnXRes:
                data.resolution.setWidth(value.toInt());
                break;
            case ColumnYRes:
                data.resolution.setHeight(value.toInt());
                break;
            case ColumnBpp:
                data.bpp = value.toInt();
                break;
            }
            changed = setItemValue(index.row(), data);
        }
    }
    if (changed) {
        emit dataChanged(index, index);
    }
    return changed;
}

Qt::ItemFlags OutputsModel::flags(const QModelIndex& index) const
{
    Qt::ItemFlags flags = MovableListModel::flags(index);
    return flags | Qt::ItemIsEditable;
}

bool OutputsModel::insert(int row, const VideoOutput& item)
{
    if (canInsertItem(item)) {
        beginInsertRows(QModelIndex(), row, row);
        insertItem(row, item);
        endInsertRows();
        return true;
    }
    return false;
}

// bool OutputsModel::insertRows(int row, int count, const QModelIndex& parent)
//{
//    beginInsertRows(parent, row, row + count - 1);
//    // FIXME: Implement me!
//    endInsertRows();
//}

bool OutputsModel::removeRows(int row, int count, const QModelIndex& parent)
{
    if (canRemoveItems(row, count) && count > 0) {
        beginRemoveRows(parent, row, row + count - 1);
        removeItems(row, count);
        endRemoveRows();
        return true;
    }
    return false;
}

bool OutputsModel::moveRows(const QModelIndex& sourceParent,
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

void OutputsModel::emitValueChanged(const QString& name, const VideoOutput& value) const
{
    Q_UNUSED(name);
    emit valueChanged(value.id(), value);
}
