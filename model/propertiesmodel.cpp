#include "propertiesmodel.hpp"
#include "attr/attritem.hpp"
#include "repository/skinrepository.hpp"

PropertiesModel::PropertiesModel(QObject* parent)
    : QAbstractItemModel(parent)
    , dummyItem(nullptr, Property::invalid)
    , mData(nullptr)
    , mRoot(&dummyItem)
{
}

PropertiesModel::~PropertiesModel()
{
}

void PropertiesModel::setWidget(QModelIndex index)
{
    beginResetModel();

    mData = SkinRepository::screens()->getWidget(index);
    if (mData != nullptr) {
        mRoot = mData->adaptersRoot();
        QObject::connect(mData, &WidgetData::attrChanged, this,
                         &PropertiesModel::onAttributeChanged);
    } else {
        mRoot = &dummyItem;
    }

    endResetModel();
}

QVariant PropertiesModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
        case ColumnKey:
            return "Property";
        case ColumnValue:
            return "Value";
        }
    }
    return QVariant();
}

QModelIndex PropertiesModel::index(int row, int column, const QModelIndex& parent) const
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

    AttrItem* childIitem = parentItem->child(row);
    if (childIitem)
        return createIndex(row, column, childIitem);
    else
        return QModelIndex();
}

QModelIndex PropertiesModel::parent(const QModelIndex& index) const
{
    if (!index.isValid())
        return QModelIndex();

    Item* item = castItem(index)->parent()->self();
    if (item == mRoot)
        return QModelIndex();
    return createIndex(item->myIndex(), ColumnKey, item);
}

int PropertiesModel::rowCount(const QModelIndex& parent) const
{
    Item* parentItem;
    if (!parent.isValid())
        parentItem = mRoot;
    else
        parentItem = castItem(parent);

    return parentItem->childCount();
}

int PropertiesModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent)
    return ColumnsCount;
}

QVariant PropertiesModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    Q_ASSERT(hasIndex(index.row(), index.column(), parent(index)));

    AttrItem* attr = static_cast<AttrItem*>(index.internalPointer());

    switch (index.column()) {
    case ColumnKey:
        return attr->keyData(role);
    case ColumnValue:
        return attr->data(role);
    default:
        return QVariant();
    }
}

bool PropertiesModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (!index.isValid())
        return false;

    AttrItem* attr = static_cast<AttrItem*>(index.internalPointer());

    switch (index.column()) {
    case ColumnValue:
        return attr->setData(value, role);
    default:
        return false;
    }
}

Qt::ItemFlags PropertiesModel::flags(const QModelIndex& index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    AttrItem* attr = static_cast<AttrItem*>(index.internalPointer());

    if (index.column() == ColumnKey || attr->key() == Property::invalid) {
        return Qt::ItemIsEnabled;
    } else {
        return Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    }
}

void PropertiesModel::onAttributeChanged(int attrKey)
{
    if (mData == nullptr) {
        return;
    }
    AttrItem* item = mData->getAttrAdapterPtr(attrKey);
    if (item) {
        QModelIndex attrIndex = createIndex(item->myIndex(), ColumnValue, item);
        emit dataChanged(attrIndex, attrIndex);
        // TODO: emit also for childs
    }
}

PropertiesModel::Item* PropertiesModel::castItem(QModelIndex index)
{
    Q_ASSERT(index.isValid());
    return static_cast<Item*>(index.internalPointer());
}
