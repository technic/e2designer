#include "propertiesmodel.hpp"
#include "repository/skinrepository.hpp"

PropertiesModel::PropertiesModel(ScreensModel* model, QObject* parent)
    : QAbstractItemModel(parent)
    , m_dummyRoot(Property::invalid)
    , m_root(&m_dummyRoot)
    , m_model(model)
    , m_observer(new WidgetObserverRegistrator(model, QModelIndex()))
{
    Q_CHECK_PTR(m_model);
    connect(m_model, &ScreensModel::widgetChanged, this, &PropertiesModel::onAttributeChanged);

    connect(m_model,
            &ScreensModel::modelAboutToBeReset,
            this,
            &PropertiesModel::onModelAboutToBeReset);
}

PropertiesModel::~PropertiesModel() = default;

void PropertiesModel::setWidget(const QModelIndex& index)
{
    beginResetModel();
    m_index = index;
    if (m_index.isValid()) {
        m_tree = std::make_unique<PropertyTree>(&m_model->widget(m_index));
        m_root = m_tree->root();
    } else {
        m_tree.reset();
        m_root = &m_dummyRoot;
    }
    m_observer->setIndex(index);

    //    if (mData != nullptr) {
    //        disconnect(mData, &WidgetData::attrChanged,
    //                   this, &PropertiesModel::onAttributeChanged);
    //        disconnect(mData, &QObject::destroyed,
    //                   this, &PropertiesModel::onWidgetDestroyed);
    //    }

    //    mData = widget;

    //    if (mData != nullptr) {
    //        mRoot = mData->adaptersRoot();
    //        connect(mData, &WidgetData::attrChanged, this, &PropertiesModel::onAttributeChanged);
    //        connect(mData, &QObject::destroyed, this, &PropertiesModel::onWidgetDestroyed);
    //    } else {
    //        mRoot = &dummyItem;
    //    }

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
        parentItem = m_root;
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
    if (item == m_root)
        return QModelIndex();
    return createIndex(item->myIndex(), ColumnKey, item);
}

int PropertiesModel::rowCount(const QModelIndex& parent) const
{
    Item* parentItem;
    if (!parent.isValid())
        parentItem = m_root;
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

    auto* attr = static_cast<AttrItem*>(index.internalPointer());

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

    AttrItem& item = *static_cast<AttrItem*>(index.internalPointer());

    switch (index.column()) {
    case ColumnValue: {
        const QVariant& newValue = item.convert(value, role);
        if (newValue.isValid()) {
            m_model->setWidgetAttr(m_index, item.key(), newValue);
        }
        return true;
    }
    default:
        return false;
    }
}

Qt::ItemFlags PropertiesModel::flags(const QModelIndex& index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    auto* attr = static_cast<AttrItem*>(index.internalPointer());

    if (index.column() == ColumnKey || attr->key() == Property::invalid) {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    } else {
        return Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    }
}

void PropertiesModel::onAttributeChanged(const QModelIndex& index, int key)
{
    if (index != m_index)
        return;
    AttrItem* item = m_tree->getItemPtr(key);
    if (item) {
        QModelIndex index = createIndex(item->myIndex(), ColumnValue, item);
        emit dataChanged(index, index);
        // TODO: emit also for childs
    }
}

void PropertiesModel::onModelAboutToBeReset()
{
    // Our widget becomes invalid
    setWidget(QModelIndex());
}

// void PropertiesModel::onWidgetDestroyed()
//{
//    mData = nullptr;
//    beginResetModel();
//    mRoot = &dummyItem;
//    endResetModel();
//}

PropertiesModel::Item* PropertiesModel::castItem(QModelIndex index)
{
    Q_ASSERT(index.isValid());
    return static_cast<Item*>(index.internalPointer());
}
