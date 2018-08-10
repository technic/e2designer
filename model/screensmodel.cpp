#include "screensmodel.hpp"
#include "repository/skinrepository.hpp"

// ScreensTree

void ScreensTree::loadPreviews()
{
    mPreviews.clear();

    QFile file(SkinRepository::instance().dir().filePath("preview.xml"));
    bool ok = file.open(QIODevice::ReadOnly);
    if (!ok)
        return;

    QXmlStreamReader xml(&file);
    auto converter = QMetaEnum::fromType<Property::Render>();

    while (xml.readNextStartElement()) {
        if (xml.name() == "screens")
            break;
        xml.skipCurrentElement();
    }
    // we are inside screens
    while (xml.readNextStartElement()) {
        if (xml.name() != "screen") {
            xml.skipCurrentElement();
            continue;
        }

        // we are inside screen
        QString screenName;
        QMap<QString, Preview> map;

        while (xml.readNextStartElement()) {
            if (xml.name() == "name") {
                screenName = xml.readElementText();

            } else if (xml.name() == "entries") {
                for (; xml.readNextStartElement(); xml.skipCurrentElement()) {
                    if (xml.name() != "entry")
                        continue;

                    // we are inside entry
                    const QString widgetName = xml.attributes().value("name").toString();
                    const QString value = xml.attributes().value("value").toString();
                    QStringRef rstr = xml.attributes().value("render");
                    bool ok;
                    int render = converter.keyToValue(rstr.toLatin1().data(), &ok);
                    if (!ok)
                        render = Property::Widget;
                    if (!widgetName.isNull() && !widgetName.isEmpty()) {
                        map.insert(widgetName, Preview(value, render));
                    }
                }
            } else {
                xml.skipCurrentElement();
            }
        }
        if (!screenName.isNull() && !screenName.isEmpty()) {
            mPreviews.insert(screenName, map);
        }
    }
    file.close();
}

Preview ScreensTree::getPreview(const QString& screen, const QString& widget)
{
    auto screen_it = mPreviews.find(screen);
    if (screen_it == mPreviews.end())
        return Preview();
    auto widget_it = screen_it.value().find(widget);
    if (widget_it == screen_it.value().end())
        return Preview();
    return widget_it.value();
}

// ScreensModel

ScreensModel::ScreensModel(QObject* parent)
    : QAbstractItemModel(parent)
    , mRoot(new WidgetData(true))
{
}

ScreensModel::~ScreensModel()
{
    delete mRoot;
}
QVariant ScreensModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
        case ColumnElement:
            return "Element";
        case ColumnName:
            return "Name";
        }
    }
    return QVariant();
}

QModelIndex ScreensModel::index(int row, int column, const QModelIndex& parent) const
{
    // check that row and column are in range
    if (!hasIndex(row, column, parent)) {
        return QModelIndex();
    }

    Item* parentItem;
    if (!parent.isValid()) {
        parentItem = mRoot;
    } else {
        parentItem = castItem(parent);
    }

    Item* childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

QModelIndex ScreensModel::parent(const QModelIndex& index) const
{
    if (!index.isValid())
        return QModelIndex();

    Item* item = castItem(index)->parent()->self();
    if (item == mRoot)
        return QModelIndex();
    return createIndex(item->myIndex(), ColumnElement, item);
}

int ScreensModel::rowCount(const QModelIndex& parent) const
{
    Item* parentItem;
    if (!parent.isValid())
        parentItem = mRoot;
    else
        parentItem = castItem(parent);

    return parentItem->childCount();
}

int ScreensModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent)
    return ColumnsCount;
}

QVariant ScreensModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    WidgetData* widget = static_cast<WidgetData*>(index.internalPointer());

    switch (role) {
    case Qt::DisplayRole:
    case Qt::EditRole:
        switch (index.column()) {
        case ColumnElement:
            return widget->typeStr();
        case ColumnName:
            switch (widget->type()) {
            case WidgetData::Label:
                return widget->getAttr(Property::text, role);
            case WidgetData::Pixmap:
                return widget->getAttr(Property::pixmap, role);
            case WidgetData::Screen:
            case WidgetData::Widget:
                return widget->getAttr(Property::name, role);
            }
        }
    case ScreensModel::IdRole:
        return widget->id();
    case ScreensModel::TypeRole:
        return widget->type();
    default:
        return QVariant();
    }
}

bool ScreensModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (!index.isValid())
        return false;

    WidgetData* widget = static_cast<WidgetData*>(index.internalPointer());

    switch (role) {
    case Qt::EditRole:
        switch (index.column()) {
        case ColumnName:
            switch (widget->type()) {
            case WidgetData::Label:
                return widget->setAttr(Property::text, value, role);
            case WidgetData::Pixmap:
                return widget->setAttr(Property::pixmap, value, role);
            case WidgetData::Screen:
            case WidgetData::Widget:
                return widget->setAttr(Property::name, value, role);
            }
        default:
            return false;
        }
    case ScreensModel::TypeRole:
        return widget->setType(value.toInt());
    default:
        return false;
    }
}

Qt::ItemFlags ScreensModel::flags(const QModelIndex& index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    if (index.column() == ColumnElement) {
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled
            | Qt::ItemIsDropEnabled;
    } else {
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsDragEnabled
            | Qt::ItemIsDropEnabled;
    }
}

bool ScreensModel::insertRows(int row, int count, const QModelIndex& parent)
{
    Item* parentItem;
    if (!parent.isValid())
        parentItem = mRoot;
    else
        parentItem = castItem(parent);

    if (row < 0 || row > rowCount(parent)) {
        return false;
    }

    beginInsertRows(parent, row, row + count - 1);
    for (int i = 0; i < count; ++i) {
        parentItem->insertChild(row, new WidgetData());
    }
    endInsertRows();
    return true;
}

bool ScreensModel::removeRows(int row, int count, const QModelIndex& parent)
{
    Item* parentItem;
    if (!parent.isValid())
        parentItem = mRoot;
    else
        parentItem = castItem(parent);

    if (count <= 0 || row < 0 || row + count > parentItem->childCount()) {
        return false;
    }
    beginRemoveRows(parent, row, row + count - 1);
    parentItem->removeChildren(row, count);
    endRemoveRows();
    return true;
}

void ScreensModel::clear()
{
    beginResetModel();
    QModelIndex rootIndex;
    removeRows(0, rowCount(rootIndex), rootIndex);
    endResetModel();
}

void ScreensModel::appendFromXml(QXmlStreamReader& xml)
{
    Q_ASSERT(xml.isStartElement() && xml.name() == "screen");

    beginInsertRows(QModelIndex(), mRoot->childCount(), mRoot->childCount());

    WidgetData* w = new WidgetData();
    w->fromXml(xml);
    mRoot->appendChild(w);

    endInsertRows();
}

void ScreensModel::toXml(QXmlStreamWriter& xml)
{
    for (int i = 0; i < mRoot->childCount(); ++i) {
        mRoot->child(i)->toXml(xml);
    }
}

QVariant ScreensModel::getWidgetAttr(const QModelIndex& index, int attrKey, int role) const
{
    if (!index.isValid())
        return QVariant();

    WidgetData* widget = static_cast<WidgetData*>(index.internalPointer());
    return widget->getAttr(attrKey, role);
}

bool ScreensModel::setWidgetAttr(const QModelIndex& index, int attrKey, const QVariant& value,
                                 int role)
{
    if (!index.isValid())
        return false;

    WidgetData* widget = static_cast<WidgetData*>(index.internalPointer());
    return widget->setAttr(attrKey, value, role);
}

WidgetData* ScreensModel::getWidget(const QModelIndex& index)
{
    if (!index.isValid())
        return nullptr;

    return static_cast<WidgetData*>(index.internalPointer());
}

void ScreensModel::widgetAttrHasChanged(const WidgetData* widget, int attrKey)
{
    Item* item = const_cast<WidgetData*>(widget);
    QModelIndex idx = createIndex(item->myIndex(), ColumnElement, item);

    switch (widget->type()) {
    case WidgetData::Label:
        if (attrKey != Property::text)
            return;
        break;
    case WidgetData::Pixmap:
        if (attrKey != Property::pixmap)
            return;
        break;
    case WidgetData::Screen:
    case WidgetData::Widget:
        if (attrKey != Property::name)
            return;
        break;
    }
    emit dataChanged(idx, idx);
}

ScreensModel::Item* ScreensModel::castItem(const QModelIndex& index)
{
    Q_ASSERT(index.isValid());
    return static_cast<Item*>(index.internalPointer());
}
