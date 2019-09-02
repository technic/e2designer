#ifndef PROPERTIESMODEL_H
#define PROPERTIESMODEL_H

#include "skin/widgetdata.hpp"
#include "propertytree.hpp"
#include <QAbstractItemModel>
#include <memory>

class AttrItem;
class WidgetObserverRegistrator;

class PropertiesModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    typedef AttrItem Item;

    explicit PropertiesModel(ScreensModel* model, QObject* parent = Q_NULLPTR);
    ~PropertiesModel() override;

    enum
    {
        ColumnKey,
        ColumnValue,
        ColumnsCount
    };

    void setWidget(const QModelIndex& index);

    // Header:
    QVariant headerData(int section,
                        Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

    // Basic functionality:
    QModelIndex index(int row,
                      int column,
                      const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& index) const override;

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    // Editable:
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;

private slots:
    void onAttributeChanged(const QModelIndex& index, int key);
    void onModelAboutToBeReset();

private:
    static Item* castItem(QModelIndex index);
    AttrItem m_dummyRoot;
    std::unique_ptr<PropertyTree> m_tree;
    AttrItem* m_root;
    ScreensModel* m_model;
    QPersistentModelIndex m_index;
    QScopedPointer<WidgetObserverRegistrator> m_observer;
};

#endif // PROPERTIESMODEL_H
