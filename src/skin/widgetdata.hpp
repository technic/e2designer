#pragma once

#include "base/tree.hpp"
#include "repository/xmlnode.hpp"
#include "converter.hpp"
#include "attributes.hpp"
#include "sizeattr.hpp"
#include "positionattr.hpp"
#include "enumattr.hpp"
#include "fontattr.hpp"
#include "colorattr.hpp"
#include "offsetattr.hpp"
#include "model/windowstyle.hpp"
#include <QRgb>
#include <QVariant>
#include <vector>

class QXmlStreamReader;
class QXmlStreamWriter;
class ScreensModel;
class Font;

using PixmapAttr = QString; // FIXME: HACK

class WidgetData : public MixinTreeNode<WidgetData>, public XmlData
{
    Q_GADGET

    using Size = SizeAttr;
    using Position = PositionAttr;

    using Base = MixinTreeNode<WidgetData>;

public:
    using MixinTreeNode<WidgetData>::parent;

    // Widget tag
    enum class WidgetType
    {
        Screen,
        Widget,
        Label,
        Pixmap,
        Applet,
        Panel,
        Include,
    };
    Q_ENUM(WidgetType)

    explicit WidgetData(WidgetType type);
    static WidgetData* createFromXml(QXmlStreamReader& xml);

    // expose tree functions
    using Base::child;
    using Base::childCount;
    using Base::myIndex;

    // override tree functions to manage model pointer
    // takes ownership
    bool insertChild(int position, WidgetData* child) final;
    bool insertChildren(int position, QVector<WidgetData*> list) final;
    // releases ownership
    QVector<WidgetData*> takeChildren(int position, int count) final;

    // model
    ScreensModel* model() const { return m_model; }
    void setModel(ScreensModel* model);

    WidgetType type() const { return m_type; }
    QString typeStr() const;
    static WidgetType strToType(const QStringRef& str, bool& ok);

    // size
    SizeAttr size() const { return m_attrs.size; }
    void resize(const QSizeF& size);
    void setSize(const SizeAttr& size);

    // postion
    PositionAttr position() const { return m_attrs.position; }
    void move(const QPointF& pos);
    void setPosition(const PositionAttr& pos);

    // Access to absolute size and position
    QPoint absolutePosition() const;
    QSize selfSize() const;
    QSize parentSize() const;

    // Common
    int zPosition() const { return m_attrs.zValue; }
    void setZPosition(int z);
    bool transparent() const { return m_attrs.transparent; }
    void setTransparent(bool val);

    // Colors
    ColorAttr color(int key) const;
    void setColor(int key, const ColorAttr& color);
    QColor getQColor(int key) const;
    // Pixmaps
    PixmapAttr pixmap(int key) const;
    void setPixmap(int key, const PixmapAttr& p);
    // Booleans
    bool hasFlag(int key) const;
    void setFlag(int key, bool value);

    // Font
    FontAttr font() const { return m_attrs.font; }
    void setFont(const FontAttr& font);

    // Borders
    int borderWidth() const { return m_attrs.borderWidth; }
    void setBorderWidth(int px);

    // Label
    QString text() const { return m_attrs.text; }
    void setText(const QString& text);
    PropertyHAlign::Enum halign() const { return m_attrs.halign; }
    void setHalign(PropertyHAlign::Enum align);
    PropertyVAlign::Enum valign() const { return m_attrs.valign; }
    void setValign(PropertyVAlign::Enum align);
    OffsetAttr shadowOffset() const { return m_attrs.shadowOffset; }
    void setShadowOffset(const OffsetAttr& offset);
    bool noWrap() const { return m_attrs.noWrap; }
    void setNoWrap(bool value);

    // List
    int itemHeight() const { return m_attrs.itemHeight; }
    void setItemHeight(int px);
    Property::ScrollbarMode scrollbarMode() const { return m_attrs.scrollbarMode; }
    void setScrollbarMode(Property::ScrollbarMode mode);

    // Pixmap
    Property::Alphatest alphatest() const { return m_attrs.alphatest; }
    void setAlphatest(Property::Alphatest value);
    int scale() const { return m_attrs.scale; }
    void setScale(int scale);

    // Slider
    Property::Orientation orientation() const { return m_attrs.orientation; }
    void setOrientation(Property::Orientation orientation);

    // Widget
    QString name() const { return m_attrs.name; }
    void setName(const QString& name);
    Property::Render render() const { return m_attrs.render; }
    void setRender(Property::Render render);
    QString source() const { return m_attrs.source; }
    void setSource(const QString& source);

    // Widget preview
    Property::Render previewRender() const { return m_attrs.previewRender; }
    void setPreviewRender(Property::Render render);
    QVariant previewValue() const { return m_attrs.previewValue; }
    void setPreviewValue(const QVariant& value);
    void loadPreview();
    // Render to use on scene
    Property::Render sceneRender() const;
    QVariant scenePreview() const;

    // Screen
    QString title() const { return m_attrs.title; }
    void setTitle(const QString& text);
    Property::Flags flags() const { return m_attrs.flags; }
    void setFlags(Property::Flags flags);

    // Xml
    virtual void toXml(XmlStreamWriter& xml) const;

    // Attribute get/set QVariant methods
    QVariant getAttr(int key) const;
    bool setAttr(int key, const QVariant& value);

    // Other attributes
    virtual QString getAttr(const QString& key) const;

    // Deep copy
    WidgetData* clone() const;

    // Required for the optimisation:
    // only widgets which are viewed by someone
    // listen to color/font changed signals
    //    void connectNotify(const QMetaMethod& signal) override;
    //    void disconnectNotify(const QMetaMethod& signal) override;

    // signals:
    //    void attrChanged(int key);
    //    void typeChanged(WidgetType type);

    // private slots:
    void onColorChanged(const QString& name, QRgb value);
    void onStyledColorChanged(WindowStyleColor::ColorRole role, QRgb value);
    void onFontChanged(const QString& name, const Font& value);
    void updateCache();

protected:
    void parseAttributes(QXmlStreamAttributes attrs);
    void writeAttributes(XmlStreamWriter& xml) const;

private:
    bool fromXml(QXmlStreamReader& xml);

    void appendConverterFromXml(QXmlStreamReader& xml);
    void convertersToXml(XmlStreamWriter& xml) const;

    void sizeChanged();
    void parentSizeChanged();
    void notifyAttrChange(int key);
    void setAttrFromXml(int key, const QString& str);

    class WidgetAttributes
    {
    public:
        // Constructor which initializes default values
        WidgetAttributes();

        // Size and position
        Size size;
        Position position;

        // Common
        QString name;
        int zValue;
        bool transparent;
        //    ColorAttr m_borderColor;
        int borderWidth;

        // Pixmap
        //    PixmapAttr m_pixmap;
        Property::Alphatest alphatest;
        int scale;

        // Label
        QString text;
        FontAttr font;
        PropertyVAlign::Enum valign;
        PropertyHAlign::Enum halign;
        //    ColorAttr m_shadowColor;
        OffsetAttr shadowOffset;
        bool noWrap;

        // Screen
        QString title;
        Property::Flags flags;

        // List
        int itemHeight;
        //    PixmapAttr m_selectionPixmap;
        //    bool m_selectionDisabled;
        Property::ScrollbarMode scrollbarMode;
        //    bool m_enableWrapAround;

        // Slider
        //    PixmapAttr m_sliderPixmap;
        //    PixmapAttr m_backgroundPixmap;
        Property::Orientation orientation;

        // Widget
        Property::Render render;
        QString source;

        // Applet
        QString appletCode;

        // Preview
        Property::Render previewRender;
        QVariant previewValue;

        // named by key
        QHash<int, CachedColor> colors;
        QHash<int, PixmapAttr> pixmaps;
        QHash<int, bool> switches;

        // Unknown attributes
        QMap<QString, QString> otherAttributes;
    };

    // Store
    WidgetAttributes m_attrs;

    // Order of attributes that was in xml. We want to preserve it upon serialization
    QVector<QString> m_propertiesOrder;

    // Other
    WidgetType m_type;
    std::vector<std::unique_ptr<Converter>> m_converters;

    // Reference to the model, that owns this widget
    ScreensModel* m_model;
};
