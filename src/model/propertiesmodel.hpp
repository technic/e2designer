#ifndef PROPERTIESMODEL_H
#define PROPERTIESMODEL_H

#include "repository/widgetdata.hpp"
#include <QAbstractItemModel>
#include <memory>

class AttrItem;

class PropertiesModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    typedef AttrItem Item;

    explicit PropertiesModel(QObject* parent = Q_NULLPTR);
    ~PropertiesModel() override;

    enum { ColumnKey, ColumnValue, ColumnsCount };

    void setWidget(WidgetData *widget);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

    // Basic functionality:
    QModelIndex index(int row, int column,
                      const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& index) const override;

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    // Editable:
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;

private slots:
    void onAttributeChanged(int attrKey);
    void onWidgetDestroyed();

private:
    static Item* castItem(QModelIndex index);
    AttrItem dummyItem;
    // weak reference to QObject containing data for the model
    // reset it to nullptr when QObject::destroyed signal arrives
    WidgetData* mData;
    // ref to current model root
    AttrItem* mRoot;
};

#endif // PROPERTIESMODEL_H
