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
    SizeAttr size() const { return m_size; }
    void resize(const QSizeF& size);
    void setSize(const SizeAttr& size);

    // postion
    PositionAttr position() const { return m_position; }
    void move(const QPointF& pos);
    void setPosition(const PositionAttr& pos);

    // Access to absolute size and position
    QPoint absolutePosition() const;
    QSize selfSize() const;
    QSize parentSize() const;

    // Common
    int zPosition() const { return m_zValue; }
    void setZPosition(int z);
    bool transparent() const { return m_transparent; }
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
    FontAttr font() const { return m_font; }
    void setFont(const FontAttr& font);

    // Borders
    int borderWidth() const { return m_borderWidth; }
    void setBorderWidth(int px);

    // Label
    QString text() const { return m_text; }
    void setText(const QString& text);
    PropertyHAlign::Enum halign() const { return m_halign; }
    void setHalign(PropertyHAlign::Enum align);
    PropertyVAlign::Enum valign() const { return m_valign; }
    void setValign(PropertyVAlign::Enum align);
    OffsetAttr shadowOffset() const { return m_shadowOffset; }
    void setShadowOffset(const OffsetAttr& offset);
    bool noWrap() const { return m_noWrap; }
    void setNoWrap(bool value);

    // List
    int itemHeight() const { return m_itemHeight; }
    void setItemHeight(int px);
    Property::ScrollbarMode scrollbarMode() const { return m_scrollbarMode; }
    void setScrollbarMode(Property::ScrollbarMode mode);

    // Pixmap
    Property::Alphatest alphatest() const { return m_alphatest; }
    void setAlphatest(Property::Alphatest value);
    int scale() const { return m_scale; }
    void setScale(int scale);

    // Slider
    Property::Orientation orientation() const { return m_orientation; }
    void setOrientation(Property::Orientation orientation);

    // Widget
    QString name() const { return m_name; }
    void setName(const QString& name);
    Property::Render render() const { return m_render; }
    void setRender(Property::Render render);
    QString source() const { return m_source; }
    void setSource(const QString& source);

    // Widget preview
    Property::Render previewRender() const { return m_previewRender; }
    void setPreviewRender(Property::Render render);
    QVariant previewValue() const { return m_previewValue; }
    void setPreviewValue(const QVariant& value);
    void loadPreview();
    // Render to use on scene
    Property::Render sceneRender() const;
    QVariant scenePreview() const;

    // Screen
    QString title() const { return m_title; }
    void setTitle(const QString& text);
    Property::Flags flags() const { return m_flags; }
    void setFlags(Property::Flags flags);

    // Xml
    virtual bool fromXml(QXmlStreamReader& xml);
    virtual void toXml(XmlStreamWriter& xml) const;

    // Attribute get/set QVariant methods
    QVariant getAttr(int key) const;
    bool setAttr(int key, const QVariant& value);

    // Other attributes
    QString getAttr(const QString& key) const;

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
    void sizeChanged();
    void parentSizeChanged();
    void notifyAttrChange(int key);
    void setAttrFromXml(int key, const QString& str);

    // Size and position
    Size m_size;
    Position m_position;

    // Common
    QString m_name;
    int m_zValue;
    bool m_transparent;
    //    ColorAttr m_borderColor;
    int m_borderWidth;

    // Pixmap
    //    PixmapAttr m_pixmap;
    Property::Alphatest m_alphatest;
    int m_scale;

    // Label
    QString m_text;
    FontAttr m_font;
    PropertyVAlign::Enum m_valign;
    PropertyHAlign::Enum m_halign;
    //    ColorAttr m_shadowColor;
    OffsetAttr m_shadowOffset;
    bool m_noWrap;

    // Screen
    QString m_title;
    Property::Flags m_flags;

    // List
    int m_itemHeight;
    //    PixmapAttr m_selectionPixmap;
    //    bool m_selectionDisabled;
    Property::ScrollbarMode m_scrollbarMode;
    //    bool m_enableWrapAround;

    // Slider
    //    PixmapAttr m_sliderPixmap;
    //    PixmapAttr m_backgroundPixmap;
    Property::Orientation m_orientation;

    // Widget
    Property::Render m_render;
    QString m_source;

    // Applet
    QString m_appletCode;

    // Preview
    Property::Render m_previewRender;
    QVariant m_previewValue;

    // named by key
    QHash<int, CachedColor> m_colors;
    QHash<int, PixmapAttr> m_pixmaps;
    QHash<int, bool> m_switches;

    // Other
    WidgetType m_type;
    QVector<QString> m_propertiesOrder;
    std::vector<std::unique_ptr<Converter>> m_converters;
    ScreensModel* m_model;
    QMap<QString, QString> m_otherAttributes;
};
