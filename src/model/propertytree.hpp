#pragma once

#include "base/tree.hpp"
#include "skin/widgetdata.hpp"
#include "skin/enumattr.hpp"
#include <QHash>

class AttrItem;

class PropertyTree
{
public:
    PropertyTree(const WidgetData* widget);
    AttrItem* root() const { return m_root; }
    AttrItem* getItemPtr(const int key) const;
    ~PropertyTree();

    const WidgetData& widget();
    void setWidget(const WidgetData* widget);

private:
    template<typename T>
    void add(int k, AttrItem* parent);

    // ref
    const WidgetData* m_widget;
    // own
    AttrItem* m_root;
    // refs within root
    QHash<int, AttrItem*> m_adapters;
};

/**
 * @brief Provides interface to manipulate WidgetData attribute
 * Has tree structure for QAbstractItemModel
 */
class AttrItem : public MixinTreeNode<AttrItem>
{
public:
    explicit AttrItem(int key = Property::invalid);
    ~AttrItem() override {}
    void setRoot(PropertyTree* tree);
    int key() const { return m_key; }
    // Iterface for Qt Models
    virtual QVariant keyData(int role) const;
    virtual QVariant data(int role) const;
    virtual QVariant convert(const QVariant& value, int role);

protected:
    inline const WidgetData& widget() const { return m_tree->widget(); }

private:
    int m_key;
    PropertyTree* m_tree;
    // const WidgetData *mWidget;
};

class AttrGroupItem : public AttrItem
{
public:
    AttrGroupItem()
        : AttrItem()
    {}
    AttrGroupItem(const QString& title)
        : AttrItem()
        , m_title(title)
    {}
    QVariant keyData(int role) const override;
    QVariant data(int role = Qt::UserRole + 1) const override;

private:
    QString m_title;
};

class ColorItem : public AttrItem
{
public:
    explicit ColorItem(int key)
        : AttrItem(key)
    {}
    QVariant data(int role) const override;
    QVariant convert(const QVariant& value, int role) override;
};

typedef QPair<QString, QStringList> SkinEnumList;
Q_DECLARE_METATYPE(SkinEnumList)

template<class Enum>
class EnumItem : public AttrItem
{
public:
    using AttrType = EnumAttr<Enum>;
    EnumItem(int key)
        : AttrItem(key)
    {}
    QVariant data(int role) const override;
    QVariant convert(const QVariant& value, int role) override;
};

using VAlignItem = EnumItem<PropertyVAlign::Enum>;
using HAlignItem = EnumItem<PropertyHAlign::Enum>;
using AlphatestItem = EnumItem<Property::Alphatest>;
using OrientationItem = EnumItem<Property::Orientation>;
using ScrollbarModeItem = EnumItem<Property::ScrollbarMode>;
using RenderItem = EnumItem<Property::Render>;
using FlagsItem = EnumItem<Property::Flags>;

class FontItem : public AttrItem
{
public:
    explicit FontItem(int key)
        : AttrItem(key)
    {}
    QVariant data(int role) const override;
    QVariant convert(const QVariant& value, int role) override;
};

class IntegerItem : public AttrItem
{
public:
    IntegerItem(int key)
        : AttrItem(key)
    {}
    QVariant data(int role) const override;
    QVariant convert(const QVariant& value, int role) override;
};

class PixmapItem : public AttrItem
{
public:
    PixmapItem(int key);
    QVariant data(int role) const override;
    QVariant convert(const QVariant& value, int role) override;
};

class PositionItem : public AttrItem
{
public:
    PositionItem(int key);
    enum AxisName
    {
        X,
        Y
    };

    QVariant data(int role) const final;
    QVariant convert(const QVariant& value, int role) final;

    class CoordinateItem : public AttrItem
    {
    public:
        CoordinateItem(AxisName axes)
            : AttrItem()
            , axis(axes)
        {}
        QVariant data(int role) const final;
        QVariant convert(const QVariant& value, int role) final;

    private:
        AxisName axis;
    };
};

class AbsolutePositionItem : public AttrItem
{
public:
    AbsolutePositionItem()
        : AttrItem()
    {}
    QVariant data(int role = Qt::UserRole + 1) const override;
};

class SizeItem : public AttrItem
{
public:
    SizeItem(int key)
        : AttrItem(key)
    {}
    QVariant data(int role = Qt::UserRole + 1) const override;
    QVariant convert(const QVariant& value, int role) override;
};

class TextItem : public AttrItem
{
public:
    TextItem(int key)
        : AttrItem(key)
    {}
    QVariant data(int role = Qt::UserRole + 1) const override;
    QVariant convert(const QVariant& value, int role = Qt::UserRole + 1) override;
};

class VariantItem : public AttrItem
{
public:
    VariantItem(int key)
        : AttrItem(key)
    {}
    QVariant data(int role) const override;
    QVariant convert(const QVariant& value, int role) override;
};

